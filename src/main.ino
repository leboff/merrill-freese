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
char rdsBuffer[10];

// Audio Tools
AudioInfo info(8000, 1, 8); // 8000 Hz sample rate, mono, 8-bit
AnalogAudioStream analogStream; // ADC Stream
AudioEncoderServer server(new WAVEncoder(), ssid, password);

void setup() {
    Serial.begin(115200);

    // Initialize FM Radio
    radio.powerOn();
    radio.setVolume(5);
    radio.setChannel(channel);

    // Initialize ADC
    auto cfg = analogStream.defaultConfig(RX_MODE);
    cfg.sample_rate = info.sample_rate;
    cfg.channels = info.channels;
    cfg.bits_per_sample = info.bits_per_sample;
    cfg.adc_pin = 36; // GPIO36 for mono
    //cfg.input_device = ADC_INPUT_LINE2; //Remove this line
    analogStream.begin(cfg);

    // Start Web Server (using WAV encoder)
    server.begin(analogStream, info);
    Serial.println("Server started");
}

void loop() {
    // Handle Web Server
    server.doLoop();
}