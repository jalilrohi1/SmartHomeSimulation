#pragma once
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "Sensors.h"
#include "Actuators.h"

class SmartHomeWebServer {
private:
  WebServer server;
  Sensors* sensors;
  Actuators* actuators;
  
  // Web page content
  String getHomePage();
  String getApiData();
  void handleRoot();
  void handleAPI();
  void handleControl();
  void handleNotFound();

public:
  SmartHomeWebServer();
  void begin(Sensors* sensorPtr, Actuators* actuatorPtr);
  void handleClient();
  void stop();
};
