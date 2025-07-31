#include "Sensors.h"
#include "Actuators.h"
#include "Display.h"
#include "WiFiMQTT.h"
#include "pins.h"
#include "config.h"
#include <Arduino.h>
#include <ArduinoJson.h>

// Simplified core objects
Sensors sensors;
Actuators actuators;
Display display;
NetworkManager* network = nullptr;

// System configuration thresholds - simplified
const int TEMP_THRESHOLD_AC = 20;     // Temperature threshold for AC activation
const int GAS_THRESHOLD = 3000;        // Gas leak detection threshold
//const int BLINDS_OPEN = 0;            // Servo position for open blinds
//const int BLINDS_CLOSED = 180;        // Servo position for closed blinds

// System timing variables
unsigned long lastDisplayUpdate = 0;
unsigned long lastDataPublish = 0;
const unsigned long DISPLAY_INTERVAL = 3000;    // Display update interval
const unsigned long PUBLISH_INTERVAL = 5000;    // MQTT publish interval

// System start time for uptime calculation
unsigned long systemStartTime = 0;

unsigned long lastHallMotionTime = 0;
const unsigned long motionTimeout = 5000; // 5 seconds for more stable lighting
bool lastLEDState = false; // To track if the LED was on/off
long lightLevel = 0; // To store the last light level reading 
//long potOldValue = 0; // To store the last potentiometer value

void setup() {
  
  Serial.begin(115200); //Fast, standard for ESP32 ⭐
  //Serial.begin(57600); // Medium speed
  //Serial.begin(230400); // Very fast for Wokwi simulation
  //Serial.begin(921600); // Maximum speed
  //Serial.begin(9600); // Slow but reliable for real hardware

  delay(1000);

  Serial.println("=================================");
  Serial.println("Smart Home Simulation - Simplified");
  Serial.println("=================================");
  Serial.println("Core Functionalities:");
  Serial.println("1. Bedroom AC control (DHT1 + Relay1)");
  Serial.println("2. Kitchen gas detection + Exhaust fan");
  Serial.println("3. Blinds control (Potentiometer ONLY → Servo)");
  Serial.println("4. Hall RGB lighting (Motion + Photoresistor color)");
  Serial.println("5. Toilet lighting (Motion + LED)");
  Serial.println("=================================");
  
  // Initialize system components
  sensors.begin();
  actuators.begin();
  display.initialize();
  systemStartTime = millis();
  
  // Initialize network dynamically
  network = new NetworkManager();

  // Show startup message on display
  display.showStatus("Smart Home", "Simplified Mode");
  delay(1000);
  

  // Connect to WiFi
  display.showStatus("Connecting WiFi", "Please wait...");
  //network.connectWiFi();
  network->connectWiFi();

  // Wait for WiFi connection
  int wifiAttempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifiAttempts < 30) {
    delay(1000);
    wifiAttempts++;
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected successfully!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    network->connectMQTT();
    display.showStatus("System Ready", "IP: " + WiFi.localIP().toString());  
  } else {
    Serial.println("WiFi connection failed!");
    display.showStatus("WiFi Failed", "Check config");
    delete network;
    network = nullptr;
  }

 

  Serial.println("Setup completed. Starting main loop...");
}


void loop() {
  unsigned long currentTime = millis();
  
  // Handle network connectivity
  //network.mqttLoop();
  // Handle network connectivity (check if network exists)
  if (network && network->isConnected()) {
    network->mqttLoop();
  }
  
  // Read all sensor data
  SensorData data = sensors.readAll();

  // Process all core functions directly

  // 1. AC control - Turn on if temperature exceeds threshold
  // Turn on/off AC based on temperature if and only if it is not switched on from dashboard
  if(!NetworkManager::relayAC_status) {
  bool shouldRunAC = (data.bedroomTemp > TEMP_THRESHOLD_AC);
  actuators.controlAC(shouldRunAC);
  } 

  
  // 2. Gas sensor and exhaust fan
  if(!NetworkManager::relayEX_status) { // Only control if not manually overridden
  // If gas value exceeds threshold, turn on exhaust fan and buzzer
  if (data.gasValue > GAS_THRESHOLD) {
    actuators.buzzerAlert(true);
    actuators.buzzerAlert(true);
    actuators.controlExhaustFan(true);
    display.showStatus("GASALERT!", "Exhaust Fan ON");
  } else {
    actuators.buzzerAlert(false);
    actuators.controlExhaustFan(false);
  }  
  }
  // 3. Blinds control - Use potentiometer value to set blinds position

  // ✅ 4. Hall RGB lighting - Separate from blinds control
  if(!NetworkManager::hallRGB_status) { // Only control if not manually overridden
  if(data.hallMotion){
    lastHallMotionTime = currentTime; // Reset motion timer
    lastLEDState = true;
  } 

  if(currentTime- lastHallMotionTime < motionTimeout){
      if(data.lightLevel != lightLevel) {
        lightLevel = data.lightLevel; // Update light level only if it has changed
        int r = map(lightLevel, 0, 4095, 255, 50);
        //int g = 150;
        int g= map(lightLevel, 0, 4095, 50, 150);
        int b = map(lightLevel, 0, 4095, 50, 255);
        actuators.hallRGBControl(r, g, b);
        delay(10); // Short delay to ensure light turns on
        lastLEDState = true;
       // Serial.printf("LightLevel: %d, R: %d, G: %d, B: %d\n", lightLevel, r, g, b);
      } 
  }else if(currentTime - lastHallMotionTime > motionTimeout && lastLEDState) {
      // Timeout reached, turn off RGB light
     // Serial.println("Hall motion timeout reached, turning off RGB light.\n");
      actuators.hallRGBControl(0, 0, 0);
      delay(10); // Short delay to ensure light turns off
      lastLEDState = false;
  }
}
  //✅ 5. Toilet lighting control
  // If LED is manually controlled, respect that state
  if(!NetworkManager::toilet_status) { // Only control if not manually overridden
    // If toilet motion is detected, turn on LED
    actuators.toiletLED(data.toiletMotion);
  }
  // 6. Update display periodically with sensor readings
  if (currentTime - lastDisplayUpdate >= DISPLAY_INTERVAL) {
    // Simpler display that just shows core readings
    display.showStatus("B Temp: " + String(data.bedroomTemp) + "C", 
                      "H Temp: " + String(data.hallTemp) + "C");
    lastDisplayUpdate = currentTime;
  }  
  
  // 7. Publish data to MQTT periodically for Node-RED
  if (currentTime - lastDataPublish >= PUBLISH_INTERVAL) {
    //if (network.isConnected()) {
      if (network && network->isConnected()) {  // ✅ Add proper network check
      // Create JSON document for sensor data
      StaticJsonDocument<512> doc;

      // Temperature sensors
      doc["bedroom"]["temperature"] = data.bedroomTemp;
      doc["bedroom"]["humidity"] = data.bedroomHumidity;
      doc["hall"]["temperature"] = data.hallTemp;
      doc["hall"]["humidity"] = data.hallHumidity;
        // Environmental sensors
      doc["kitchen"]["gas_level"] = data.gasValue;
      doc["ambient"]["light_level"] = data.lightLevel;
 
      // Motion sensors
      doc["motion"]["hall"] = data.hallMotion;
      doc["motion"]["toilet"] = data.toiletMotion;
      
      // Door sensor
      doc["door"]["distance"] = data.doorDistance;
      doc["door"]["status"] = data.doorDistance > 50 ? "open" : "closed";
      
      doc["timestamp"] = millis();

      // Convert to string
      String payload;
      serializeJson(doc, payload);
      
     // Publish to Node-RED smart_home
      network->publishData("smart_home/sensors", payload);
      delay(10); // Small delay to ensure MQTT publish stability

      // Also publish basic system status
      StaticJsonDocument<300> statusDoc;
      statusDoc["uptime"] = (millis() - systemStartTime) / 1000;
      statusDoc["wifi_signal"] = WiFi.RSSI();
      statusDoc["free_heap"] = ESP.getFreeHeap();
      statusDoc["status_ind"] = true;
      String statusPayload;
      serializeJson(statusDoc, statusPayload);
      network->publishData("smart_home/status", statusPayload);
      
      lastDataPublish = currentTime;
    } else {
      Serial.println("❌ Network not connected - skipping publish");
    }
  }
     delay(1000); // Small delay for system stability
  }
   



