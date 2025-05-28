#include "config.h"

// WiFi/MQTT Credentials

const char* WIFI_SSID = "Wokwi-GUEST";
//const char *WIFI_SSID = "GenuaWifi";
const char* WIFI_PASS = "";
// Try alternative brokers if broker.hivemq.com fails
const char* MQTT_BROKER = "test.mosquitto.org";  // More reliable for testing
//const char* MQTT_BROKER = "broker.hivemq.com";   // Original
//const char* MQTT_BROKER = "130.251.39.35";
//const char* MQTT_BROKER = "3.121.166.18";
//const int MQTT_PORT = 1883;