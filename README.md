# merrill-freese

ESP32 (Arduino framework, PlatformIO) FM radio streamer. An Si4703 FM tuner is
controlled over I2C, audio is captured via I2S, and streamed as WAV over HTTP
(port 81) with a control API on port 80.

## Setup

WiFi credentials are kept out of version control. Before building:

```sh
cp src/secrets.h.example src/secrets.h
```

Then edit `src/secrets.h` and fill in your WiFi `WIFI_SSID` and
`WIFI_PASSWORD`. `src/secrets.h` is gitignored and will not be committed.

Build with:

```sh
pio run
```
