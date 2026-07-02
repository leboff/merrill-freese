#include "audio_stream.h"
#include "config.h"

AudioInfo audioInfo(44100, 2, 16);
I2SStream i2sStream;

void initAudio() {
  auto cfg = i2sStream.defaultConfig(RX_MODE);
  cfg.copyFrom(audioInfo);
  cfg.i2s_format = I2S_STD_FORMAT;
  cfg.is_master = true;
  cfg.use_apll = false;
  cfg.pin_mck = I2S_PIN_MCK;
  cfg.pin_bck = I2S_PIN_BCK;
  cfg.pin_ws = I2S_PIN_WS;
  cfg.pin_data = I2S_PIN_DATA;
  if (!i2sStream.begin(cfg)) {
    Serial.println("ERROR: I2S init failed - check pin assignments and ADC wiring");
  }
  // Prevent infinite blocking if ADC produces no data
  i2sStream.driver()->setWaitTimeReadMs(50);
}
