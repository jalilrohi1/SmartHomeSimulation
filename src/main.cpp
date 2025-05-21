#include "Sensors.h"
#include "Actuators.h"
#include "WiFiMQTT.h"
#include "Display.h"
#include "config.h"
#include <ArduinoJson.h>


Sensors sensors;
Actuators actuators;
NetworkManager network;
Display display;

// Thresholds
const int GAS_THRESHOLD = 0;
const int TEMP_THRESHOLD = 25;
const int LDR_DARK = 500;
const int LDR_BRIGHT = 1000;
const int BLINDS_OPEN = 0;
const int BLINDS_CLOSED = 90;

String createJSON(const SensorData& data) {
  StaticJsonDocument<256> doc;
  doc["temperature"]["room1"] = data.temp1;
  doc["temperature"]["room2"] = data.temp2;
  doc["humidity"]["room1"] = data.humidity1;
  doc["humidity"]["room2"] = data.humidity2;
  doc["gas"] = data.gasValue;
  doc["light"] = data.ldrValue;
  doc["motion"]["hall"] = data.motionHall;
  doc["motion"]["room2"] = data.motionRoom2;
  doc["door"] = data.doorDistance;

  String payload;
  serializeJson(doc, payload);
  return payload;
}

void setup() {
  Serial.begin(115200);
  sensors.begin();
  actuators.begin();
  display.initialize();
  network.connectWiFi();
  while (WiFi.status() != WL_CONNECTED) delay(100);
  network.connectMQTT();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    network.connectWiFi(); // Reconnect WiFi if dropped
  }
  network.mqttLoop();
  SensorData data = sensors.readAll();

  // Environmental controls
  actuators.controlAC(data.temp1 > TEMP_THRESHOLD);
  actuators.controlBlinds(data.ldrValue > LDR_BRIGHT ? BLINDS_CLOSED : BLINDS_OPEN);
  
  if (data.gasValue > GAS_THRESHOLD) {
    actuators.buzzerAlert(true);
    display.showStatus("GAS ALERT!", "Evacuate Now!");
  } else {
    actuators.buzzerAlert(false);
    // Combine status messages
    display.showStatus("Air Quality: OK", 
      data.doorDistance > 50 ? "Door Open!" : "Door Closed");
  }

  // Lighting control
  actuators.rgbControl((data.motionHall || data.ldrValue < LDR_DARK) ? 255 : 0, 0, 0);

  // Door status display
  display.updateDoorStatus(data.doorDistance > 50);
  
  network.publishData("smart_home/sensors", createJSON(data));
  delay(5000);
}