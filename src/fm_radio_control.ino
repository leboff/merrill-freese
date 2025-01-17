#include "radio_control.h"
#include "web_server.h"
#include "audio_stream.h"
#include "config.h"


AudioWAVServer wavServer(81);
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
  initRadio();

  initAudio();
  
  initWebServer();

  wavServer.begin(i2sStream, audioInfo);

  Serial.println("Setup complete");
}

void loop() {
  // Handle Web Server requests
    webServer.handleClient(); 
    wavServer.copy();

}
