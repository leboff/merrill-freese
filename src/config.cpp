#include "config.h"

#if __has_include("secrets.h")
#include "secrets.h"
#else
#error "Copy src/secrets.h.example to src/secrets.h and fill in your WiFi credentials"
#endif

// WiFi Credentials
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
