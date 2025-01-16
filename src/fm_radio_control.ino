#include "radio_control.h"
#include "web_server.h"
#include "audio_stream.h"
#include "config.h"

Si4703_Breakout radio(ESP32_RESET_PIN, ESP32_I2C_SDA, ESP32_I2C_SCL);
void setup() {
  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
  
  // Initialize FM Radio
  initRadio(radio);

  initAudio(i2sStream);
  
  initWebServer();

  Serial.println("Setup complete");
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);
}

void loop() {
  // Handle Web Server requests
  webServer.handleClient();

}
