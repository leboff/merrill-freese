#include "web_server.h"
#include "radio_control.h"
#include "config.h"
#include "audio_stream.h"

WebServer webServer(80);

void initWebServer() {
  webServer.on("/", handleRoot);
  webServer.on("/volume", handleVolume);
  webServer.on("/channel", handleChannel);
  webServer.on("/seekup", handleSeekUp);
  webServer.on("/seekdown", handleSeekDown);
  webServer.on("/rds", handleRDS);
  webServer.on("/info", handleRadioInfo); // Add this line
  webServer.enableCORS();
  webServer.begin();
}

void handleRoot() {
  webServer.send(200, "text/plain", "FM Radio Control Server");
}

void handleVolume() {
  if (webServer.hasArg("value")) {
    int newVolume = webServer.arg("value").toInt();
    setRadioVolume(newVolume);
    webServer.send(200, "text/plain", "Volume set to " + String(newVolume));
  } else {
    webServer.send(400, "text/plain", "Bad Request: Missing 'value' parameter");
  }
}

void handleChannel() {
  if (webServer.hasArg("value")) {
    int newChannel = webServer.arg("value").toInt();
    if (!startTune(newChannel)) {
      webServer.send(409, "text/plain", "tune in progress");
      return;
    }
    webServer.send(200, "text/plain", "Tuning to " + String(newChannel));
  } else {
    webServer.send(400, "text/plain", "Bad Request: Missing 'value' parameter");
  }
}

void handleSeekUp() {
  if (!startSeek(true)) {
    webServer.send(409, "text/plain", "tune in progress");
    return;
  }
  webServer.send(200, "text/plain", "Seeking up...");
}

void handleSeekDown() {
  if (!startSeek(false)) {
    webServer.send(409, "text/plain", "tune in progress");
    return;
  }
  webServer.send(200, "text/plain", "Seeking down...");
}

void handleRDS() {
  String rdsData = getRDSData();
  webServer.send(200, "text/plain", rdsData);
}

void handleRadioInfo() {
  String info = getRadioInfo();
  webServer.send(200, "text/plain", info);
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
