#pragma once
#include "config.h"

#if ENABLE_WEB_SERVER

#include <WiFi.h>
#include <WiFiServer.h>
#include <ArduinoJson.h>
#include "Sensors.h"
#include "Actuators.h"

class SmartHomeWebServer {
private:
  WiFiServer server;
  Sensors* sensors;
  Actuators* actuators;
  bool isEnabled;
  
  // Web page content
  String getHomePage();
  String getApiData();
  String getSimplePage();
  void handleRoot();
  void handleAPI();
  void handleControl();
  void handleNotFound();

public:
  SmartHomeWebServer();
  void begin(Sensors* sensorPtr, Actuators* actuatorPtr);
  void handleClient();
  void stop();
  void enable();
  void disable();
  bool isWebServerEnabled() const;
};

#else

// Dummy class when web server is disabled
class SmartHomeWebServer {
public:
  SmartHomeWebServer() {}
  void begin(void* sensorPtr, void* actuatorPtr) {}
  void handleClient() {}
  void stop() {}
  void enable() {}
  void disable() {}
  bool isWebServerEnabled() const { return false; }
};

#endif // ENABLE_WEB_SERVER
