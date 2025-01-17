#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WebServer.h>
#include "AudioTools.h"

// forward declarations
// extern StreamCopy *copier; // removed as no longer needed
// extern WAVEncoder *wavEncoder; // removed as no longer needed
// extern EncodedAudioOutput *encodedStream; // removed as no longer needed
extern WebServer webServer; 

void initWebServer();
void handleRoot();
void handleVolume();
void handleChannel();
void handleSeekUp();
void handleSeekDown();
void handleRDS();
void handleNotFound();
// void handleAudioStream(); // removed as no longer needed

#endif
