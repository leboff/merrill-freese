#include "Arduino.h"
#include "AudioTools.h"
#include "AudioTools/AudioLibs/AudioBoardStream.h"
#include "AudioTools/AudioCodecs/CodecMP3LAME.h"
#include "SparkFunSi4703.h" // Include the SparkFun SI4703 library
#include "Wire.h"

// Set static IP address and stuff (optional)
IPAddress IPA_address(192, 168, 50, 11);
IPAddress IPA_gateway(192, 168, 50, 1);
IPAddress IPA_subnet(255, 255, 0, 0);
IPAddress IPA_primaryDNS(192, 168, 50, 1);  //optional
IPAddress IPA_secondaryDNS(8, 8, 8, 8);    //optional

// WIFI
const char *ssid = "Kimboff";
const char *password = "youdontknow6";

AudioInfo info(16000,1,16);
MP3EncoderLAME mp3;
AudioEncoderServer server(&mp3, ssid, password);
AudioBoardStream kit(AudioKitAnalog);

// Define the ADC pins for the left and right channels
#define LOUT_ADC_PIN 36  // GPIO36 ADC1_CH0
#define ROUT_ADC_PIN 39 // GPIO39 ADC1_CH3

// SI4703 Radio Setup
#define SI4703_SDIO 21    // ESP32 Pin connected to SI4703 SDIO pin.
#define SI4703_SCLK 22    // ESP32 Pin connected to SI4703 SCLK pin.
#define SI4703_RST 23    // ESP32 Pin connected to SI4703 RST pin.

Si4703_Breakout radio(SI4703_RST, SI4703_SDIO, SI4703_SCLK); // Create Si4703 object

// Arduino setup
void setup(){
  Serial.begin(115200);
    // Defining Loglevels for the different libraries
  //AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);
  //LOGLEVEL_AUDIOKIT = AudioKitInfo;

  // Configures static IP address (optional)
  if (!WiFi.config(IPA_address, IPA_gateway, IPA_subnet, IPA_primaryDNS, IPA_secondaryDNS))
  {
    Serial.println("WiFi.config: Failed to configure static IPv4...");
  }
  
  // Initialize SI4703 Radio
  Serial.println("Starting SI4703...");
  radio.powerOn(); // Call powerOn method to initialize the SI4703
  radio.setChannel(941); // Set frequency to 94.1 MHz
  Serial.println("SI4703 initialized");


  // start i2s input with default configuration
  Serial.println("starting AudioKit...");
  auto config = kit.defaultConfig(RX_MODE);
    config.input_device = ADC_INPUT_LINE; // Use ADC_INPUT_LINE for analog input
    config.copyFrom(info);
    config.input_pin_left = LOUT_ADC_PIN;   //set left input pin
    config.input_pin_right = ROUT_ADC_PIN;   //set right input pin
    config.sd_active = false; // Disable SD card
  kit.begin(config);
  Serial.println("AudioKit started");


  // start data sink
  server.begin(kit, config);
  Serial.println("Server started");

}

// Arduino loop  
void loop() {
  // Handle new connections
  server.doLoop();  
}
