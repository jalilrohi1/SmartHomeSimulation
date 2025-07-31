#include "WiFiMQTT.h"
#include "config.h"
#include "pins.h"
#include "Actuators.h"

WiFiClient espClient;
PubSubClient client(espClient);
Actuators* NetworkManager::actuatorsPtr = nullptr;
bool NetworkManager::relayAC_status = false;
bool NetworkManager::relayEX_status = false;
bool NetworkManager::toilet_status = false;
bool NetworkManager::hallRGB_status = false;
int NetworkManager::hallRGB_level = 0;

// ✅  Initialize RGB storage variables
int NetworkManager::nodeRED_R = 255;
int NetworkManager::nodeRED_G = 255;
int NetworkManager::nodeRED_B = 255;
bool NetworkManager::useNodeRedColors = false;

void NetworkManager::connectMQTT() {
  client.setServer(MQTT_BROKER, 1883);
  client.setCallback(mqttCallback);
  client.setKeepAlive(300); // 5 minutes
  client.setBufferSize(1024);

  reconnect();
}

// Keep only ONE mqttLoop() implementation
void NetworkManager::mqttLoop() {
    if (!client.connected()) {
    Serial.println("MQTT disconnected, attempting reconnect...");
    reconnect();
  }
  client.loop();
}

void NetworkManager::reconnect() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skipping MQTT reconnect");
    return;
  }
  int attempts = 0;
  const int maxAttempts = 10;
  
  while (!client.connected() && attempts < maxAttempts) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      //Subscribe to topics after successful connection
      client.subscribe("smart_home/relay");// Relay 1 control for AC
      //client.subscribe("smart_home/relay2");// Relay 2 control for Exhaust Fan
      client.subscribe("smart_home/control");
      client.subscribe("smart_home/lighting");
      client.subscribe("smart_home/toilet_led");
      client.subscribe("smart_home/system");
      //client.subscribe("smart_home/schedule");
      Serial.println("Subscribed to all MQTT topics");

      // ✅ Publish a test message to verify publishing works
    if (client.publish("smart_home/test", "ESP32 Connected and Ready")) {
      Serial.println("✅ Test message published successfully");
    } else {
      Serial.println("❌ Test message publish failed");
    }
      break; // Exit loop on success
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      attempts++;
      delay(3000);
    }
   }
  
  if (attempts >= maxAttempts) {
    Serial.println("MQTT connection failed after maximum attempts");
  }
}

void NetworkManager::connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void NetworkManager::publishData(const char* topic, const String& payload) {
  client.publish(topic, payload.c_str());
  //Serial.printf("Published to %s: %s\n", topic, payload.c_str());
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
      digitalWrite(RELAY1_PIN, HIGH);
      NetworkManager::relayAC_status = true;
      Serial.println("AC turned ON");
    } else if (message == "AC_OFF") {
      digitalWrite(RELAY1_PIN, LOW);
      NetworkManager::relayAC_status = false;
      Serial.println("AC turned OFF");
    } 
    else if (message == "EX_ON") {
      digitalWrite(RELAY2_PIN, HIGH);
      NetworkManager::relayEX_status = true;
      Serial.println("Exhaust Fan turned ON");
    } else if (message == "EX_OFF") {
      digitalWrite(RELAY2_PIN, LOW);
      NetworkManager::relayEX_status = false;
      Serial.println("Exhaust Fan turned OFF");
    }
  }
  
  // Handle lighting control
  if (String(topic) == "smart_home/lighting") 
  {
    
    if (message.startsWith("rgb(")) {
      // Format: RGB:255,128,64
    int r, g, b;
    sscanf(message.c_str(), "rgb(%d, %d, %d)", &r, &g, &b);
      // Store the received RGB values
      NetworkManager::nodeRED_R = r;
      NetworkManager::nodeRED_G = g;
      NetworkManager::nodeRED_B = b;
      //NetworkManager::useNodeRedColors = true;
      NetworkManager::hallRGB_status = true;
      Serial.printf("RGB set to R:%d G:%d B:%d\n", r, g, b);
    }
    else if(message.startsWith("Hall_led_ON")) {
      ledcWrite(PWM_RED, NetworkManager::nodeRED_R);
      ledcWrite(PWM_GREEN, NetworkManager::nodeRED_G);
      ledcWrite(PWM_BLUE, NetworkManager::nodeRED_B);
      NetworkManager::hallRGB_status = true;
      Serial.println("Hall RGB light turned ON");
    }
    else if (message == "Hall_led_OFF") {
      ledcWrite(PWM_RED, 0);
      ledcWrite(PWM_GREEN, 0);
      ledcWrite(PWM_BLUE, 0);
      NetworkManager::hallRGB_status = false;
      Serial.println("Hall RGB light turned OFF");
    }
    else if (message=="toilet_led_ON") {
      ledcWrite(PWM_TOILET_LED, 255);
      Serial.println("Toilet LED turned ON");
      NetworkManager::toilet_status = true;
    }
    else if (message == "toilet_led_OFF") {
      ledcWrite(PWM_TOILET_LED, 0);
      Serial.println("Toilet LED turned OFF");
      NetworkManager::toilet_status = false;
    }
  }

  // Handle general control commands
  if (String(topic) == "smart_home/control") {
    if (message == "EMERGENCY_STOP") {
      // Emergency stop all actuators
      digitalWrite(RELAY1_PIN, LOW);
      digitalWrite(RELAY2_PIN, LOW);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(100);
      digitalWrite(BUZZER_PIN, LOW);
      Serial.println("EMERGENCY STOP activated");
    } else if (message == "SYSTEM_RESET") {
      //ESP.restart();
    }
  }
}

void NetworkManager::setActuators(Actuators* actuatorPtr) {
  actuatorsPtr = actuatorPtr;
}

bool NetworkManager::isConnected() {
  return client.connected();
}