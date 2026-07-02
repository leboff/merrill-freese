#include "audio_stream.h"
#include "config.h"

// 256 KB =~ 1.5s of 44.1kHz/stereo/16-bit audio (176,400 B/s). Allocated from
// PSRAM: audio-tools' DefaultAllocator prefers ps_malloc() on ESP32 whenever
// PSRAM is available, falling back to internal RAM otherwise.
#define AUDIO_BUFFER_SIZE (256 * 1024)

AudioInfo audioInfo(44100, 2, 16);
I2SStream i2sStream;

// writeMaxWait=0 so a full buffer never blocks the capture side; excess
// samples are simply dropped for that cycle instead of stalling I2S.
BufferRTOS<uint8_t> audioBuffer(AUDIO_BUFFER_SIZE, 1, 0, portMAX_DELAY);
QueueStream<uint8_t> bufferedStream(audioBuffer);
StreamCopy captureCopy(bufferedStream, i2sStream);

void initAudio() {
  auto cfg = i2sStream.defaultConfig(RX_MODE);
  cfg.copyFrom(audioInfo);
  cfg.i2s_format = I2S_STD_FORMAT;
  cfg.is_master = true;
  cfg.use_apll = false;
  cfg.buffer_count = 8;    // deepen DMA queue (default 6) to absorb brief stalls
  cfg.buffer_size = 1024;  // bytes per DMA buffer (default 512)
  cfg.pin_mck = I2S_PIN_MCK;
  cfg.pin_bck = I2S_PIN_BCK;
  cfg.pin_ws = I2S_PIN_WS;
  cfg.pin_data = I2S_PIN_DATA;
  if (!i2sStream.begin(cfg)) {
    Serial.println("ERROR: I2S init failed - check pin assignments and ADC wiring");
  }
  // Prevent infinite blocking if ADC produces no data
  i2sStream.driver()->setWaitTimeReadMs(50);

  bufferedStream.begin();
  // Disable StreamCopy's write-retry backoff: on a full buffer we want to
  // drop this cycle's data and immediately return, never delay() the task.
  captureCopy.setRetry(0);
}

void captureAudio() { captureCopy.copy(); }

void logMemoryStats() {
  Serial.printf("Free heap: %u bytes, Free PSRAM: %u bytes\n",
                (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getFreePsram());
}
