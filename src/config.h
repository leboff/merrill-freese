#ifndef CONFIG_H
#define CONFIG_H

// SI4703 Connections
#define ESP32_RESET_PIN 23
#define ESP32_I2C_SDA 21
#define ESP32_I2C_SCL 22

// I2S Connections
#define I2S_PIN_MCK 3  // GPIO3 = one of only 3 pins (0,1,3) capable of I2S MCLK on original ESP32
#define I2S_PIN_BCK 14
#define I2S_PIN_WS 15
#define I2S_PIN_DATA 32

// WiFi Credentials
extern const char *ssid;
extern const char *password;

#endif
