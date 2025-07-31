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

  static bool relayAC_status;
  static bool relayEX_status;
  static bool toilet_status;
  static bool hallRGB_status;
  static int hallRGB_level;

  // ✅ NEW: Store RGB values from Node-RED
  static int nodeRED_R;
  static int nodeRED_G;
  static int nodeRED_B;
  static bool useNodeRedColors;
  
private:
  void reconnect();
};
