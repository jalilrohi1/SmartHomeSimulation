#pragma once
#include <WiFi.h>
#include <PubSubClient.h>

// Forward declaration
class Actuators;

class NetworkManager {
  private:
  static void mqttCallback(char* topic, byte* payload, unsigned int length);
  static Actuators* actuatorsPtr; // Static pointer for callback access
  
public:
  void connectWiFi();
  void connectMQTT();
  void publishData(const char* topic, const String& payload);
  void mqttLoop();
  bool isConnected();
  void setActuators(Actuators* actuatorPtr); // Set actuator reference

private:
  void reconnect();
};
