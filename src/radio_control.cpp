#include "radio_control.h"
#include "config.h"
#include <Wire.h>
#include "SI4703.h"
#include "RDSParser.h"

RDSParser rds;

SI4703 radio; // Create an instance of the SI4703 class
int channel = 9410; // Frequency in kHz
int volume = 5;    // Initial volume level

TuneState tuneState = TUNE_IDLE;
static unsigned long tuneStartTime = 0;
static const unsigned long TUNE_TIMEOUT_MS = 4000;

// Add these lines:
String rdsServiceName = "";
String rdsText = "";
// Add these lines:
RADIO_INFO radioInfo;
char frequencyString[10];

void rdsProcess(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4) {
  rds.processData(block1, block2, block3, block4);
}

void serviceNameCallback(const char *name) {
  Serial.print("Service Name: ");
  Serial.println(name);
  rdsServiceName = name; // Store the service name
}

void rdsTextCallback(const char *text) {
  Serial.print("RDS Text: ");
  Serial.println(text);
  rdsText = text; // Store the RDS text
}
void initRadio() {
  // Set up the reset pin
  radio.setup(RADIO_RESETPIN, ESP32_RESET_PIN);
  // Configure for US region: 75µs de-emphasis, 200 kHz channel spacing
  radio.setup(RADIO_DEEMPHASIS, RADIO_DEEMPHASIS_75);
  radio.setup(RADIO_FMSPACING, RADIO_FMSPACING_200);
  // Initialize the radio
  radio.initWire(Wire); // Initialize with the Wire (I2C) library
  Wire.setClock(100000); // Explicit 100 kHz I2C clock for SI4703
  // Set the band to FM
  radio.setBand(RADIO_BAND_FM);
  // Set the frequency
  setRadioChannel(channel);
  
  // Set the volume
  setRadioVolume(volume);
  rds.init();
  radio.attachReceiveRDS(rdsProcess);
  rds.attachServiceNameCallback(serviceNameCallback);
  rds.attachTextCallback(rdsTextCallback);
}


void setRadioVolume(int newVolume) {
  volume = constrain(newVolume, 0, radio.getMaxVolume());
  radio.setVolume(volume);
}

void setRadioChannel(int newChannel) {
  channel = newChannel;
  radio.setFrequency(channel);
}

void seekRadioUp() {
  radio.seekUp();
  channel = radio.getFrequency();
}

void seekRadioDown() {
  radio.seekDown();
  channel = radio.getFrequency();
}

bool startTune(int newChannel) {
  if (tuneState != TUNE_IDLE)
    return false;
  channel = newChannel;
  radio.tuneAsync(channel);
  tuneState = TUNE_TUNING;
  tuneStartTime = millis();
  return true;
}

bool startSeek(bool up) {
  if (tuneState != TUNE_IDLE)
    return false;
  radio.seekAsync(up);
  tuneState = TUNE_SEEKING;
  tuneStartTime = millis();
  return true;
}

void pollTuneComplete() {
  if (tuneState == TUNE_IDLE)
    return;

  bool timedOut = (millis() - tuneStartTime) > TUNE_TIMEOUT_MS;
  if (radio.tuneComplete(timedOut)) {
    channel = radio.getFrequency();
    tuneState = TUNE_IDLE;
  }
}

String getRDSData() {
  // No need to call radio.checkRDS() here; we'll call it in the main loop
  String rdsData = "Service Name: " + rdsServiceName + "\nRDS Text: " + rdsText;
  return rdsData;
}
// Add this function:
String getRadioInfo() {
  radio.getRadioInfo(&radioInfo);
  radio.formatFrequency(frequencyString, sizeof(frequencyString));

  String info = "Frequency: ";
  info += frequencyString;
  info += "\nRSSI: ";
  info += String(radioInfo.rssi);
  info += "\nStereo: ";
  info += (radioInfo.stereo ? "Yes" : "No");
  info += "\nRDS: ";
  info += (radioInfo.rds ? "Available" : "Not Available");
  return info;
}