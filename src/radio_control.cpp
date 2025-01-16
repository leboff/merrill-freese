#include "radio_control.h"
#include "config.h"
#include <SPI.h>

int channel = 941; // Example initial channel
int volume = 5;    // Initial volume level

void initRadio(Si4703_Breakout& radio) {
  radio.powerOn();
  radio.setVolume(volume);
  radio.setChannel(channel);
}

void setRadioVolume(int volume) {
  volume = constrain(volume, 0, 15);
  radio.setVolume(volume);
}

void setRadioChannel(int channel) {
  radio.setChannel(channel);
}

void seekRadioUp() {
  radio.seekUp();
  channel = radio.getCurrentChannel();
}

void seekRadioDown() {
  radio.seekDown();
  channel = radio.getCurrentChannel();
}

String getRDSData() {
  char rdsData[9] = {0};
  radio.readRDS(rdsData, 10000);
  return String(rdsData);
}
