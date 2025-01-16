#include <SparkFunSi4703.h>
#include <Wire.h>
#include <SPI.h>
#include "AudioTools.h"

// SI4703 Connections
#define ESP32_RESET_PIN 23
#define ESP32_I2C_SDA 21
#define ESP32_I2C_SCL 22

// WiFi Credentials
const char *ssid = "Kimboff";
const char *password = "youdontknow6";

// FM Radio
Si4703_Breakout radio(ESP32_RESET_PIN, ESP32_I2C_SDA, ESP32_I2C_SCL);
int channel = 941; // Example initial channel

AudioInfo info(44100, 1, 24); // 44100 Hz sample rate, mono, 16-bit
I2SStream i2sStream;
CsvOutput<int24_t> csvStream(Serial);
AudioEncoderServer server(new WAVEncoder(), ssid, password);
StreamCopy copier(csvStream, i2sStream); // copy i2sStream to csvStream
void setup() {
    Serial.begin(115200);
    AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info); //Warning, Info, Error, Debug
 
    // Initialize FM Radio
    radio.powerOn();
    radio.setVolume(15);
    radio.setChannel(channel);
    // Configure ADC
    auto cfg = i2sStream.defaultConfig(RX_MODE);
    cfg.copyFrom(info);
    cfg.i2s_format = I2S_PHILIPS_FORMAT;
    cfg.is_master = true;
    // this module nees a master clock if the ESP32 is master
    cfg.use_apll = false; // try with yes
    cfg.pin_mck = 3;
    cfg.pin_bck = 14;
    cfg.pin_ws = 15;
    cfg.pin_data = 32;

    i2sStream.begin(cfg);

    // Start Web Server (using WAV encoder)
    server.begin(i2sStream, info);
    // csvStream.begin(info);
    Serial.println("Server started");
    AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning); 
}

void loop() {
    // Handle Web Server
    server.doLoop(); 
    // copier.copy();
}