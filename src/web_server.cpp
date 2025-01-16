#include "web_server.h"
#include "radio_control.h"
#include "config.h"
#include "audio_stream.h"

WebServer webServer(80);
AudioEncoder *audioEncoder = nullptr;
EncodedAudioOutput encodedStream;
StreamCopy copier; // Create a StreamCopy object

void initWebServer() {
  webServer.on("/", handleRoot);
  webServer.on("/volume", handleVolume);
  webServer.on("/channel", handleChannel);
  webServer.on("/seekup", handleSeekUp);
  webServer.on("/seekdown", handleSeekDown);
  webServer.on("/rds", handleRDS);
  webServer.on("/audio.wav", handleAudioStream);
  webServer.onNotFound(handleNotFound);
  webServer.begin();
}

void handleRoot() {
  webServer.send(200, "text/plain", "FM Radio Control Server");
}

void handleVolume() {
  if (webServer.hasArg("value")) {
    volume = webServer.arg("value").toInt();
    setRadioVolume(volume);
    webServer.send(200, "text/plain", "Volume set to " + String(volume));
  } else {
    webServer.send(400, "text/plain", "Bad Request: Missing 'value' parameter");
  }
}

void handleChannel() {
  if (webServer.hasArg("value")) {
    channel = webServer.arg("value").toInt();
    setRadioChannel(channel);
    webServer.send(200, "text/plain", "Channel set to " + String(channel));
  } else {
    webServer.send(400, "text/plain", "Bad Request: Missing 'value' parameter");
  }
}

void handleSeekUp() {
  seekRadioUp();
  webServer.send(200, "text/plain", "Seek Up, new channel: " + String(channel));
}

void handleSeekDown() {
  seekRadioDown();
  webServer.send(200, "text/plain", "Seek Down, new channel: " + String(channel));
}

void handleRDS() {
  String rdsData = getRDSData();
  webServer.send(200, "text/plain", rdsData);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";
  for (uint8_t i = 0; i < webServer.args(); i++) {
    message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
  }
  webServer.send(404, "text/plain", message);
}

void handleAudioStream() {

  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);
  if (audioEncoder == nullptr) {
    audioEncoder = new WAVEncoder();
    audioEncoder->begin();
    audioEncoder->setAudioInfo(audioInfo);
  }
  // Get a reference to the client
  NetworkClient client = webServer.client();

  // Setup output stream with the current client
  encodedStream.setOutput(client);
  encodedStream.setEncoder(audioEncoder);
  encodedStream.begin(audioInfo);

  // Send headers
  webServer.sendHeader("Cache-Control", "no-cache");
  webServer.sendHeader("Content-Type", "audio/wav");
  webServer.sendHeader("Connection", "keep-alive");
  webServer.send(200); // OK
  copier.setCheckAvailableForWrite(false);
  // Use StreamCopy to copy data from I2S to encodedStream
  copier.begin(encodedStream, i2sStream);

  // Stream data until client disconnects
  while (client.connected()) {
    if (copier.copy() == 0) {
      delay(10); // No data available, yield to other tasks
    }
  }

  // Clean up
  copier.end();
  encodedStream.end();
  webServer.client().stop();
  delete audioEncoder;
  audioEncoder = nullptr;
}
