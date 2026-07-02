#ifndef AUDIO_STREAM_H
#define AUDIO_STREAM_H

#include "AudioTools.h"

extern AudioInfo audioInfo;
extern I2SStream i2sStream;
// FIFO the WAV server reads from; decouples TCP serving from I2S capture.
extern QueueStream<uint8_t> bufferedStream;

void initAudio();

// Drains I2S into the FIFO. Never blocks on a full buffer - call in a tight
// loop from a dedicated task so capture keeps up even when the client stalls.
void captureAudio();

// Logs free heap/PSRAM once, e.g. at the end of setup().
void logMemoryStats();

#endif
