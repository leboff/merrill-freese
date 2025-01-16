#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WebServer.h>
#include "AudioTools.h"

extern WebServer webServer;
extern AudioEncoder* audioEncoder;
extern EncodedAudioOutput encodedStream;

void initWebServer();
void handleRoot();
void handleVolume();
void handleChannel();
void handleSeekUp();
void handleSeekDown();
void handleRDS();
void handleNotFound();
void handleAudioStream();

#endif
