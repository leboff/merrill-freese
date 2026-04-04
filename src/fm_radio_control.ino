#include "radio_control.h"
#include "web_server.h"
#include "audio_stream.h"
#include "config.h"
#include <esp_task_wdt.h>

AudioWAVServer wavServer(81);

// Dedicated audio streaming task on Core 1 to avoid I2C blocking stalls
void audioTask(void *param) {
  for (;;) {
    wavServer.copy();
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

  // Initialize FM Radio
  initRadio();

  initAudio();

  initWebServer();

  wavServer.begin(i2sStream, audioInfo);

  // Launch audio streaming on Core 1, isolated from I2C/web on Core 0
  xTaskCreatePinnedToCore(audioTask, "audio", 4096, NULL, 2, NULL, 1);

  // Enable hardware watchdog (10s timeout, auto-reset on hang)
  esp_task_wdt_init(10, true);
  esp_task_wdt_add(NULL);

  Serial.println("Setup complete");
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);
}

void loop() {
  esp_task_wdt_reset();

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

