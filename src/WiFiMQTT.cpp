#include "WiFiMQTT.h"
#include "config.h"
#include "pins.h"
#include "Actuators.h"

WiFiClient espClient;
PubSubClient client(espClient);
Actuators* NetworkManager::actuatorsPtr = nullptr;

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
  
  int attempts = 0;
  const int maxAttempts = 3;
  
  while (!client.connected() && attempts < maxAttempts) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
      if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Subscribe to topics after successful connection
      client.subscribe("smart_home/relay");
      client.subscribe("smart_home/control");
      client.subscribe("smart_home/lighting");
      client.subscribe("smart_home/blinds");
      client.subscribe("smart_home/system");
      client.subscribe("smart_home/schedule");
      Serial.println("Subscribed to all MQTT topics");
      break; // Exit loop on success
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      attempts++;
      delay(5000);
    }
  }
  
  if (attempts >= maxAttempts) {
    Serial.println("MQTT connection failed after maximum attempts");
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
  // Convert payload to string
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);
  
  // Handle relay control
  if (String(topic) == "smart_home/relay") {
    if (message == "AC_ON") {
      digitalWrite(RELAY2_PIN, HIGH);
      Serial.println("AC turned ON");
    } else if (message == "AC_OFF") {
      digitalWrite(RELAY2_PIN, LOW);
      Serial.println("AC turned OFF");
    } else if (message == "RELAY1_ON") {
      digitalWrite(RELAY1_PIN, HIGH);
      Serial.println("Relay 1 turned ON");
    } else if (message == "RELAY1_OFF") {
      digitalWrite(RELAY1_PIN, LOW);
      Serial.println("Relay 1 turned OFF");
    }
  }
  
  // Handle lighting control
  if (String(topic) == "smart_home/lighting") {
    if (message.startsWith("RGB:")) {
      // Format: RGB:255,128,64
      int r, g, b;
      sscanf(message.c_str(), "RGB:%d,%d,%d", &r, &g, &b);
      ledcWrite(PWM_RED, r);
      ledcWrite(PWM_GREEN, g);
      ledcWrite(PWM_BLUE, b);
      Serial.printf("RGB set to R:%d G:%d B:%d\n", r, g, b);
    }
  }
    // Handle blinds control
  if (String(topic) == "smart_home/blinds") {
    int angle = message.toInt();
    if (angle >= 0 && angle <= 180 && actuatorsPtr != nullptr) {
      actuatorsPtr->controlBlinds(angle);
      Serial.printf("Blinds set to angle: %d\n", angle);
    }
  }
  
  // Handle general control commands
  if (String(topic) == "smart_home/control") {
    if (message == "EMERGENCY_STOP") {
      // Emergency stop all actuators
      digitalWrite(RELAY1_PIN, LOW);
      digitalWrite(RELAY2_PIN, LOW);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(1000);
      digitalWrite(BUZZER_PIN, LOW);
      Serial.println("EMERGENCY STOP activated");
    } else if (message == "SYSTEM_RESET") {
      ESP.restart();
    }
  }
}

void NetworkManager::setActuators(Actuators* actuatorPtr) {
  actuatorsPtr = actuatorPtr;
}

bool NetworkManager::isConnected() {
  return client.connected();
}