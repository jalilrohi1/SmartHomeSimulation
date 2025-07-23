#pragma once

// Feature Configuration - Enable/Disable Components
#define ENABLE_WEB_SERVER false    // Web server disabled
#define ENABLE_MQTT true           // Set to false to disable MQTT
#define ENABLE_SCHEDULER false     // Scheduler disabled for simplified version
#define ENABLE_ENERGY_MONITOR false // Energy monitoring disabled

// Web Server Configuration (disabled)
#if ENABLE_WEB_SERVER
  #define WEB_SERVER_PORT 80
  #define WEB_UPDATE_INTERVAL 2000  // milliseconds
#endif

// Change to extern declarations
extern const char* WIFI_SSID;
extern const char* WIFI_PASS;
extern const char* MQTT_BROKER;

// Feature control functions
void printFeatureStatus();