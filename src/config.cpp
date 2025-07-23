#include "config.h"
#include <Arduino.h>

// WiFi/MQTT Credentials

const char* WIFI_SSID = "Wokwi-GUEST";
//const char *WIFI_SSID = "GenuaWifi";
const char* WIFI_PASS = "";
// Try alternative brokers if broker.hivemq.com fails
//const char* MQTT_BROKER = "test.mosquitto.org";  // More reliable for testing
const char* MQTT_BROKER = "broker.hivemq.com";   // Original
//const char* MQTT_BROKER = "130.251.39.35";
//const char* MQTT_BROKER = "3.121.166.18";
const int MQTT_PORT = 1883;

// Feature status logging
void printFeatureStatus() {
  Serial.println("\n=== Smart Home v2.1 Feature Configuration ===");
  #if ENABLE_WEB_SERVER
    Serial.println("✓ Web Server: ENABLED (Port " + String(WEB_SERVER_PORT) + ")");
  #else
    Serial.println("✗ Web Server: DISABLED");
  #endif
  
  #if ENABLE_MQTT
    Serial.println("✓ MQTT: ENABLED (" + String(MQTT_BROKER) + ")");
  #else
    Serial.println("✗ MQTT: DISABLED");
  #endif
  
  #if ENABLE_SCHEDULER
    Serial.println("✓ Automation Scheduler: ENABLED");
  #else
    Serial.println("✗ Automation Scheduler: DISABLED");
  #endif
  
  #if ENABLE_ENERGY_MONITOR
    Serial.println("✓ Energy Monitoring: ENABLED");
  #else
    Serial.println("✗ Energy Monitoring: DISABLED");
  #endif
  Serial.println("===========================================\n");
}