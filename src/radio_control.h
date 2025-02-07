#ifndef RADIO_CONTROL_H
#define RADIO_CONTROL_H

#include "SI4703.h"

extern SI4703 radio; // Declare the radio object
extern int channel;
extern int volume;

void initRadio();
void setRadioVolume(int volume);
void setRadioChannel(int channel);
void seekRadioUp();
void seekRadioDown();
String getRDSData();
String getRadioInfo(); // Add this line

#endif
