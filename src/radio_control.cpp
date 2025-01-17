#include "radio_control.h"
#include "config.h"
#include <SPI.h>
#include "SparkFunSi4703.h"

Si4703_Breakout radio(ESP32_RESET_PIN, ESP32_I2C_SDA, ESP32_I2C_SCL);
int channel = 941; // Example initial channel
int volume = 5;    // Initial volume level

void initRadio() {
  radio.powerOn();
  radio.setVolume(volume);
  radio.setChannel(channel);
}

void setRadioVolume(int newVolume) {
  volume = constrain(newVolume, 0, 15); // use newVolume here
  radio.setVolume(volume);
}

void setRadioChannel(int newChannel) { // use newChannel here
  channel = newChannel;
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
