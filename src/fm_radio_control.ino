#include "radio_control.h"
#include "web_server.h"
#include "audio_stream.h"
#include "config.h"
#include "AudioTools/Communication/HTTP/AudioServerWiFi.h"
#include <esp_task_wdt.h>

AudioWAVServer wavServer(81);

// Wraps the FIFO the WAV server reads from so the diagnostic below reports
// real measured throughput instead of a dead byte counter.
MeasuringStream measuredStream(bufferedStream);

// Dedicated audio streaming task on Core 1 to avoid I2C blocking stalls.
// Reads from the FIFO (filled by captureTask), never touches I2S directly.
void audioTask(void *param) {
  esp_task_wdt_add(NULL);  // watch this task too - a wedged copy() now panics/reboots
  static uint32_t lastDiag = 0;
  for (;;) {
    bool active = wavServer.copy();
    esp_task_wdt_reset();
    // Print measured throughput every 5s so we can see if data is flowing
    if (millis() - lastDiag >= 5000) {
      Serial.printf("[audio] streaming: %s, %d kbytes/sec\n",
                    active ? "active" : "idle",
                    measuredStream.bytesPerSecond() / 1000);
      lastDiag = millis();
    }
    vTaskDelay(1);
  }
}

// Drains I2S into the FIFO. Higher priority than audioTask so capture always
// wins the core when the TCP socket stalls - a stalled listener skips ahead
// instead of causing a DMA overrun.
void captureTask(void *param) {
  esp_task_wdt_add(NULL);  // watch this task too
  for (;;) {
    captureAudio();
    esp_task_wdt_reset();
    vTaskDelay(1);
  }
}

void setup() {
  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

  // Connect to Wi-Fi with timeout
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi connection failed - restarting");
    ESP.restart();
  }
  Serial.println();
  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());

  WiFi.setAutoReconnect(true);   // ESP32 SDK retries the AP automatically
  WiFi.persistent(false);        // don't wear flash rewriting credentials
  WiFi.setSleep(false);          // disable modem power save - critical for streaming latency

  // Initialize FM Radio
  initRadio();

  initAudio();

  initWebServer();

  wavServer.begin(measuredStream, audioInfo);

  // Enable hardware watchdog (10s timeout, auto-reset on hang) before
  // launching the worker tasks below, since each subscribes itself on entry
  // (esp_task_wdt_add adds the *calling* task).
  esp_task_wdt_config_t wdt_config = {
    .timeout_ms = 10000,
    .idle_core_mask = 0,
    .trigger_panic = true
  };
  esp_task_wdt_init(&wdt_config);
  esp_task_wdt_add(NULL);

  // Launch audio capture + streaming on Core 1, isolated from I2C/web on Core 0.
  // captureTask (priority 3) drains I2S into the FIFO; audioTask (priority 2)
  // serves the FIFO to the TCP client.
  xTaskCreatePinnedToCore(captureTask, "capture", 4096, NULL, 3, NULL, 1);
  xTaskCreatePinnedToCore(audioTask, "audio", 4096, NULL, 2, NULL, 1);

  logMemoryStats();

  Serial.println("Setup complete");
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);
}

void loop() {
  esp_task_wdt_reset();

  // WiFi self-recovery: restart if disconnected for > 5 minutes
  static unsigned long wifiLostSince = 0;
  if (WiFi.status() != WL_CONNECTED) {
    if (wifiLostSince == 0) {
      wifiLostSince = millis();
      Serial.println("WiFi lost - waiting for auto-reconnect");
    } else if (millis() - wifiLostSince > 5UL * 60UL * 1000UL) {
      Serial.println("WiFi down > 5 min - restarting");
      ESP.restart();
    }
  } else if (wifiLostSince != 0) {
    wifiLostSince = 0;
    Serial.print("WiFi reconnected, IP: ");
    Serial.println(WiFi.localIP());
  }

  // Handle Web Server requests
  webServer.handleClient();

  // RDS check every 40ms
  static unsigned long lastRDSCheck = 0;
  if (millis() - lastRDSCheck >= 40) {
    radio.checkRDS();
    lastRDSCheck = millis();
  }

  // Optional: Output radio info periodically
  static unsigned long lastInfoPrint = 0;
  if (millis() - lastInfoPrint >= 5000) { // Every 5 seconds
    String info = getRadioInfo();
    Serial.println(info);
    lastInfoPrint = millis();
  }
}

