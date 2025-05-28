#include "Sensors.h"
#include "Actuators.h"
#include "WiFiMQTT.h"
#include "Display.h"
#include "WebServer.h"
#include "Scheduler.h"
#include "config.h"
#include <ArduinoJson.h>

Sensors sensors;
Actuators actuators;
NetworkManager network;
Display display;
SmartHomeWebServer webServer;
Scheduler scheduler;

// Enhanced thresholds and configurations
const int GAS_THRESHOLD = 100;        // Increased for better sensitivity
const int TEMP_THRESHOLD = 25;
const int LDR_DARK = 500;
const int LDR_BRIGHT = 1000;
const int BLINDS_OPEN = 0;
const int BLINDS_CLOSED = 90;

// System state variables
bool systemEnabled = true;
bool autoMode = true;
unsigned long lastDataPublish = 0;
unsigned long lastDisplayUpdate = 0;
const unsigned long PUBLISH_INTERVAL = 5000;    // 5 seconds
const unsigned long DISPLAY_INTERVAL = 1000;    // 1 second

// Energy monitoring
unsigned long energyStartTime = 0;
int acRunTime = 0;
int lightingTime = 0;

String createJSON(const SensorData& data) {
  StaticJsonDocument<512> doc;
  
  // Basic sensor data
  doc["temperature"]["room1"] = data.temp1;
  doc["temperature"]["room2"] = data.temp2;
  doc["humidity"]["room1"] = data.humidity1;
  doc["humidity"]["room2"] = data.humidity2;
  doc["gas"] = data.gasValue;
  doc["light"] = data.ldrValue;
  doc["motion"]["hall"] = data.motionHall;
  doc["motion"]["room2"] = data.motionRoom2;
  doc["door"]["distance"] = data.doorDistance;
  doc["door"]["status"] = data.doorDistance > 50 ? "open" : "closed";
  
  // System information
  doc["system"]["version"] = "2.1";
  doc["system"]["uptime"] = millis() / 1000;
  doc["system"]["wifi_signal"] = WiFi.RSSI();
  doc["system"]["free_heap"] = ESP.getFreeHeap();
  doc["system"]["auto_mode"] = autoMode;
  doc["system"]["enabled"] = systemEnabled;
  
  // Energy monitoring
  doc["energy"]["ac_runtime"] = acRunTime;
  doc["energy"]["lighting_time"] = lightingTime;
  
  doc["timestamp"] = millis();

  String payload;
  serializeJson(doc, payload);
  return payload;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Print feature configuration
  printFeatureStatus();
  
  Serial.println("=================================");
  Serial.println("Smart Home Simulation v2.1");
  Serial.println("=================================");
  
  // Initialize components
  sensors.begin();
  actuators.begin();
  display.initialize();
  
  // Show startup message
  display.showStatus("Smart Home v2.1", "Initializing...");
  delay(2000);
  
  // Connect to WiFi
  display.showStatus("Connecting WiFi", "Please wait...");
  network.connectWiFi();
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.printf("WiFi Connected! IP: %s\n", WiFi.localIP().toString().c_str());
  
  // Initialize web server (conditional)
  #if ENABLE_WEB_SERVER
    webServer.begin(&sensors, &actuators);
  #else
    Serial.println("✗ Web server disabled - using Node-RED for visualization");
  #endif
  
  // Initialize scheduler (conditional)
  #if ENABLE_SCHEDULER
    scheduler.begin(&actuators);
  #else
    Serial.println("✗ Automation scheduler disabled");
  #endif
  
  // Set actuator reference for MQTT callbacks
  network.setActuators(&actuators);
  
  // Connect to MQTT
  display.showStatus("Connecting MQTT", "Broker: " + String(MQTT_BROKER));
  network.connectMQTT();
  
  // Subscribe to additional topics for v2
  // This will be handled in the reconnect function
  
  display.showStatus("System Ready", "v2.1 Online");
  Serial.println("Smart Home v2.1 - All systems ready!");
  
  energyStartTime = millis();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Handle web server requests (conditional)
  #if ENABLE_WEB_SERVER
    webServer.handleClient();
  #endif
  
  // Check WiFi and MQTT connections
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, attempting reconnect...");
    display.showStatus("WiFi Lost", "Reconnecting...");
    network.connectWiFi();
    return; // Skip this loop iteration
  }
  
  network.mqttLoop();
  
  // Check scheduled tasks
  scheduler.checkTasks();
  
  // Read sensor data
  SensorData data = sensors.readAll();

  // Automated controls (only if system is enabled and in auto mode)
  if (systemEnabled && autoMode) {
    handleAutomaticControls(data);
  }

  // Safety checks (always active regardless of mode)
  handleSafetyChecks(data);

  // Update display periodically
  if (currentTime - lastDisplayUpdate >= DISPLAY_INTERVAL) {
    updateDisplay(data);
    lastDisplayUpdate = currentTime;
  }
  
  // Publish data to MQTT periodically
  if (currentTime - lastDataPublish >= PUBLISH_INTERVAL) {
    if (network.isConnected()) {
      network.publishData("smart_home/sensors", createJSON(data));
      network.publishData("smart_home/status", getSystemStatus());
    } else {
      Serial.println("MQTT not connected, skipping publish");
    }
    lastDataPublish = currentTime;
  }
  
  delay(100); // Small delay for system stability
}

void handleAutomaticControls(const SensorData& data) {
  // Environmental controls
  bool shouldRunAC = (data.temp1 > TEMP_THRESHOLD || data.temp2 > TEMP_THRESHOLD);
  actuators.controlAC(shouldRunAC);
  
  // Track AC runtime for energy monitoring
  static unsigned long acStartTime = 0;
  static bool acWasRunning = false;
  if (shouldRunAC && !acWasRunning) {
    acStartTime = millis();
    acWasRunning = true;
  } else if (!shouldRunAC && acWasRunning) {
    acRunTime += (millis() - acStartTime) / 1000;
    acWasRunning = false;
  }

  // Automated blinds control
  int blindsPosition = (data.ldrValue > LDR_BRIGHT) ? BLINDS_CLOSED : BLINDS_OPEN;
  actuators.controlBlinds(blindsPosition);

  // Intelligent lighting control
  bool needsLight = (data.motionHall || data.motionRoom2) && (data.ldrValue < LDR_DARK);
  int lightIntensity = needsLight ? 255 : 0;
  
  // Warm light in the evening (simulated)
  int currentHour = scheduler.getCurrentHour();
  if (needsLight && currentHour >= 18) {
    actuators.rgbControl(255, 150, 50); // Warm light
  } else {
    actuators.rgbControl(lightIntensity, lightIntensity, lightIntensity); // White light
  }
  
  // Track lighting time
  static bool lightWasOn = false;
  static unsigned long lightStartTime = 0;
  if (lightIntensity > 0 && !lightWasOn) {
    lightStartTime = millis();
    lightWasOn = true;
  } else if (lightIntensity == 0 && lightWasOn) {
    lightingTime += (millis() - lightStartTime) / 1000;
    lightWasOn = false;
  }
}

void handleSafetyChecks(const SensorData& data) {
  // Gas leak detection
  if (data.gasValue > GAS_THRESHOLD) {
    actuators.buzzerAlert(true);
    display.showStatus("⚠️ GAS ALERT!", "Evacuate Now!");
    
    // Send emergency MQTT alert
    if (network.isConnected()) {
      network.publishData("smart_home/emergency", "{\"type\":\"gas_leak\",\"level\":" + String(data.gasValue) + "}");
    }
    
    // Emergency actions
    actuators.controlAC(false); // Turn off AC to prevent circulation
    delay(2000);
    actuators.buzzerAlert(false);
  } else {
    actuators.buzzerAlert(false);
  }
  
  // Security check - unexpected door opening
  static bool doorWasOpen = false;
  if (data.doorDistance > 50 && !doorWasOpen) {
    if (network.isConnected()) {
      network.publishData("smart_home/security", "{\"event\":\"door_opened\",\"timestamp\":" + String(millis()) + "}");
    }
    doorWasOpen = true;
  } else if (data.doorDistance <= 50) {
    doorWasOpen = false;
  }
}

void updateDisplay(const SensorData& data) {
  // Cycle through different display screens
  static int displayMode = 0;
  static unsigned long modeChangeTime = 0;
  
  if (millis() - modeChangeTime > 3000) { // Change every 3 seconds
    displayMode = (displayMode + 1) % 4;
    modeChangeTime = millis();
  }
  
  switch (displayMode) {
    case 0:
      display.showStatus("Temp: " + String(data.temp1, 1) + "°C", 
                        "Humidity: " + String(data.humidity1, 1) + "%");
      break;
    case 1:
      display.showStatus("Gas: " + String(data.gasValue), 
                        "Light: " + String(data.ldrValue));
      break;
    case 2:
      display.showStatus("Door: " + String(data.doorDistance > 50 ? "Open" : "Closed"),
                        "Motion: " + String(data.motionHall ? "Yes" : "No"));
      break;
    case 3:
      display.showStatus("WiFi: " + String(WiFi.RSSI()) + "dBm",
                        "Uptime: " + String(millis()/1000) + "s");
      break;
  }
}

String getSystemStatus() {
  StaticJsonDocument<256> doc;
  doc["version"] = "2.1";
  doc["system_enabled"] = systemEnabled;
  doc["auto_mode"] = autoMode;
  doc["uptime"] = millis() / 1000;
  doc["free_heap"] = ESP.getFreeHeap();
  doc["wifi_signal"] = WiFi.RSSI();
  doc["mqtt_connected"] = network.isConnected();
  doc["web_server"] = "active";
  doc["scheduler_tasks"] = scheduler.getTasksList();
  
  String output;
  serializeJson(doc, output);
  return output;
}