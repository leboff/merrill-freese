#ifndef RADIO_CONTROL_H
#define RADIO_CONTROL_H

#include <SparkFunSi4703.h>

// moved radio into the cpp file as it is not needed here
// extern Si4703_Breakout radio; // Declare radio as extern
extern int channel;
extern int volume;

void initRadio();
void setRadioVolume(int volume);
void setRadioChannel(int channel);
void seekRadioUp();
void seekRadioDown();
String getRDSData();

#endif
