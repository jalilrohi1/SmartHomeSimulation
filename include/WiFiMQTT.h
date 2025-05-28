#pragma once
#include <WiFi.h>
#include <PubSubClient.h>

class NetworkManager {
  private:
  static void mqttCallback(char* topic, byte* payload, unsigned int length);
public:
  void connectWiFi();
  void connectMQTT();
  void publishData(const char* topic, const String& payload);
  void mqttLoop();
  bool isConnected(); // Add connection status check

private:
  void reconnect(); // Add missing declaration
};
