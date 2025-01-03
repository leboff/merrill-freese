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
AudioInfo info(44100, 1, 16); // 44100 Hz sample rate, mono, 16-bit
AnalogAudioStream analogStream; // ADC Stream
AudioEncoderServer server(new WAVEncoder(), ssid, password);

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10); // Wait for serial to become available
    }

    // Initialize FM Radio
    radio.powerOn();
    radio.setVolume(15);
    radio.setChannel(channel);

    // Initialize ADC using AnalogConfigESP32V1
    auto cfg = analogStream.defaultConfig(RX_MODE);
    cfg.sample_rate = info.sample_rate;
    cfg.channels = info.channels;
    cfg.bits_per_sample = info.bits_per_sample;
    cfg.is_auto_center_read = false;
    cfg.adc_attenuation = ADC_ATTEN_DB_0;
    cfg.adc_calibration_active = true;
    cfg.adc_channels[0] = ADC_CHANNEL_0; // for esp32: GPIO 36, also remove line cfg.adc_pin
    analogStream.begin(cfg);

    // Start Web Server (using WAV encoder)
    server.begin(analogStream, info);
    Serial.println("Server started");
}

void loop() {
    // Handle Web Server
    server.doLoop();
}