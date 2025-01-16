#ifndef RADIO_CONTROL_H
#define RADIO_CONTROL_H

#include <SparkFunSi4703.h>

extern Si4703_Breakout radio; // Declare radio as extern
extern int channel;
extern int volume;

void initRadio(Si4703_Breakout& radio);
void setRadioVolume(int volume);
void setRadioChannel(int channel);
void seekRadioUp();
void seekRadioDown();
String getRDSData();

#endif
