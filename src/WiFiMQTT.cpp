#include "WiFiMQTT.h"
#include "config.h"
#include "pins.h"

WiFiClient espClient;
PubSubClient client(espClient);

void NetworkManager::connectMQTT() {
  client.setServer(MQTT_BROKER, 1883);
  client.setCallback(mqttCallback);
  reconnect();
}

// Keep only ONE mqttLoop() implementation
void NetworkManager::mqttLoop() {
  if (!client.connected()) reconnect();
  client.loop();
}

void NetworkManager::reconnect() {

  if (WiFi.status() != WL_CONNECTED) return; // Skip if WiFi is down
  while (!client.connected()) {
    if (!client.connect("ESP32Client")) { // Simplified client ID
      delay(5000); // Retry after 5 seconds
      continue;
    }
    client.subscribe("smart_home/relay");
  }
}

void NetworkManager::connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

void NetworkManager::publishData(const char* topic, const String& payload) {
  client.publish(topic, payload.c_str());
}

void NetworkManager::mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Your callback logic here
}