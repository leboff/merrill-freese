#ifndef RADIO_CONTROL_H
#define RADIO_CONTROL_H

#include "SI4703.h"

extern SI4703 radio; // Declare the radio object
extern int channel;
extern int volume;

enum TuneState { TUNE_IDLE, TUNE_TUNING, TUNE_SEEKING };
extern TuneState tuneState;

void initRadio();
void setRadioVolume(int volume);
void setRadioChannel(int channel);
void seekRadioUp();
void seekRadioDown();

// Non-blocking tune/seek. Return false (caller should respond 409) if a
// tune/seek is already in progress.
bool startTune(int newChannel);
bool startSeek(bool up);
void pollTuneComplete(); // call every loop() iteration

String getRDSData();
String getRadioInfo(); // Add this line

#endif
