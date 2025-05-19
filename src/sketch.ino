#include <DHT.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <esp_task_wdt.h>

WebServer server(8185);

// WiFi & MQTT Credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqttServer = "broker.hivemq.com";
//(current HiveMQ IP as of 2024)
//const char* mqttServer = "3.121.166.18";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// Constants and Thresholds
#define GAS_THRESHOLD 0
#define TEMP_THRESHOLD 25
#define LDR_DARK 500
#define LDR_BRIGHT 1000
#define DOOR_OPEN_DISTANCE 50
#define BLINDS_OPEN 0
#define BLINDS_CLOSED 90
#define DEBOUNCE_DELAY 200
#define TEMP_AVG_WINDOW 10


// MQTT Topics
#define TEMP_TOPIC "smart_home/temperature"
#define HUMIDITY_TOPIC "smart_home/humidity"
#define GAS_TOPIC "smart_home/gas"
#define RELAY_TOPIC "smart_home/relay"
#define MOTION_TOPIC "smart_home/motion"
#define DOOR_TOPIC "smart_home/door"

// Hardware Configuration
LiquidCrystal_I2C lcd2(0x27, 16, 2);
DHT dht1(4, DHT22);
DHT dht2(16, DHT22);
Servo blindsServo;

// Pin Definitions
const int PIR1_PIN = 5;     // Hall
const int PIR2_PIN = 17;    // Room 2
const int GAS_PIN = 36;
const int LDR_PIN = 35;
const int TRIG_PIN = 18;
const int ECHO_PIN = 19;
const int RELAY1_PIN = 27;
const int RELAY2_PIN = 14;
const int SERVO_PIN = 12;
const int BUZZER_PIN = 23;
const int RGB_R = 13;
const int RGB_G = 26;
const int RGB_B = 32;
const int BTN1_PIN = 34;
const int BTN2_PIN = 33;
const int POT_PIN = 39;

// Global Variables
float tempReadings[TEMP_AVG_WINDOW] = {0};
int tempIndex = 0;


// Variables
unsigned long lastPublish = 0;
bool manualMode = false;
bool relay1State = false;
bool relay2State = false;
int currentGasThreshold = GAS_THRESHOLD;

// PWM Channels
const int PWM_RED = 0;
const int PWM_GREEN = 1;
const int PWM_BLUE = 2;

void callback(char* topic, byte* payload, unsigned int length) {
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  
  if (strcmp(topic, RELAY_TOPIC) == 0) {
    if (strcmp(message, "ON") == 0) {
      digitalWrite(RELAY1_PIN, HIGH);
    } else if (strcmp(message, "OFF") == 0) {
      digitalWrite(RELAY1_PIN, LOW);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe(RELAY_TOPIC);
      client.publish("esp32/status", "Reconnected");
    }
    delay(5000);
  }
}

void readPotentiometer() {
  currentGasThreshold = map(analogRead(POT_PIN), 0, 4095, 300, 1000);
}

void controlBuzzer(bool state) {
  digitalWrite(BUZZER_PIN, state);
}

void handleButtons() {
  static unsigned long lastDebounce = 0;
  if ((millis() - lastDebounce) > DEBOUNCE_DELAY) {
    if (digitalRead(BTN1_PIN) == LOW) {
      relay1State = !relay1State;
      digitalWrite(RELAY1_PIN, relay1State);
      lastDebounce = millis();
    }
    if (digitalRead(BTN2_PIN) == LOW) {
      manualMode = !manualMode;
      lcd2.clear();
      lcd2.print(manualMode ? "Manual Mode" : "Auto Mode");
      lastDebounce = millis();
    }
  }
}

// Add to global variables section
long distance = 0;
bool motionHall = false;
// JSON Payload Construction
String createSensorPayload(
      float temp1, float temp2, 
      float humidity1, float humidity2,
      int gasValue, int ldrValue,
      bool motionHall, bool motionRoom2,
      long distance) 
  {
  StaticJsonDocument<256> doc;
  doc["temperature"]["room1"] = temp1;
  doc["temperature"]["room2"] = temp2;
  doc["humidity"]["room1"] = humidity1;
  doc["humidity"]["room2"] = humidity2;
  doc["gas"] = gasValue;
  doc["light"] = ldrValue;
  doc["motion"]["hall"] = motionHall;
  doc["motion"]["room2"] = motionRoom2;
  doc["door"] = distance;

  String payload;
  serializeJson(doc, payload);
  return payload;
}

// Temperature Averaging
float getAverageTemperature() {
  tempReadings[tempIndex] = dht1.readTemperature();
  tempIndex = (tempIndex + 1) % TEMP_AVG_WINDOW;
  
  float sum = 0;
  for (int i = 0; i < TEMP_AVG_WINDOW; i++) {
    sum += tempReadings[i];
  }
  return sum / TEMP_AVG_WINDOW;
}

void setup() {
  Serial.begin(115200);
    // Add DNS configuration FIRST
  //WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, IPAddress(8, 8, 8, 8));
  WiFi.begin(ssid, password);
  // Set defaults directly
  manualMode = false;
  relay1State = false;
  relay2State = false;


  lcd2.init();
  lcd2.backlight();
 
  // Initialize Hardware  
  dht1.begin();
  dht2.begin();
  blindsServo.attach(SERVO_PIN);




  digitalWrite(RELAY1_PIN, relay1State);
  digitalWrite(RELAY2_PIN, relay2State);

  pinMode(PIR1_PIN, INPUT);
  pinMode(PIR2_PIN, INPUT);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);
  
  // RGB LED PWM Setup
  ledcSetup(PWM_RED, 5000, 8);
  ledcAttachPin(RGB_R, PWM_RED);
  ledcSetup(PWM_GREEN, 5000, 8);
  ledcAttachPin(RGB_G, PWM_GREEN);
  ledcSetup(PWM_BLUE, 5000, 8);
  ledcAttachPin(RGB_B, PWM_BLUE);

  // WiFi Connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  
  // MQTT Setup
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  static unsigned long stableCounter = 0; 
  // Stability check
  if(millis() > 10000 && stableCounter == 0) {
    Serial.println("System stable for 10s");
    stableCounter = millis();
  }
  if (!client.connected()) reconnect();
  client.loop();
  handleButtons();
  readPotentiometer();

  // Sensor Readings
  float temp1 = dht1.readTemperature();
  float temp2 = dht2.readTemperature();
  if (isnan(temp1)) {
    Serial.println("Failed to read DHT1 temperature!");
    temp1 = -999; // Special error value
  }
  float humidity1 = dht1.readHumidity();
  float humidity2 = dht2.readHumidity();
  if (isnan(humidity1)) {
    Serial.println("Failed to read DHT1 humidity!");
    humidity1 = -999;
  }
  int gasValue = analogRead(GAS_PIN);
  int ldrValue = analogRead(LDR_PIN);
  bool motionHall = digitalRead(PIR1_PIN);
  bool motionRoom2 = digitalRead(PIR2_PIN);

  // Ultrasonic Measurement
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long distance = pulseIn(ECHO_PIN, HIGH) * 0.034 / 2;
  
  // Use averaged temperature
  float avgTemp = getAverageTemperature();
  // Updated Control Logic
  if (!manualMode) {
    digitalWrite(RELAY2_PIN, (avgTemp > TEMP_THRESHOLD) ? HIGH : LOW);
  }
  // Control Logic
  if (!manualMode) {
    // Gas System
    if (gasValue > currentGasThreshold) {
      lcd2.setCursor(0, 0);
      lcd2.print("Gas Alert!  ");
      controlBuzzer(true);
    } else {
      lcd2.setCursor(0, 0);
      lcd2.print("Air Quality OK  ");
      controlBuzzer(false);
    }

    // Temperature Control
    digitalWrite(RELAY2_PIN, (temp1 > TEMP_THRESHOLD) ? HIGH : LOW);

    // Lighting Control
    if (motionHall || ldrValue < LDR_DARK) {
      ledcWrite(PWM_RED, 255);
    } else {
      ledcWrite(PWM_RED, 0);
    }

    // Blinds Control
    blindsServo.write((ldrValue > LDR_BRIGHT) ? BLINDS_CLOSED : BLINDS_OPEN);
  }


  // Save state on mode change
  static bool lastManualMode = manualMode;
  if (manualMode != lastManualMode) {
    lastManualMode = manualMode;
  }
  // Door Status
  lcd2.setCursor(0, 1);
  lcd2.print(distance > DOOR_OPEN_DISTANCE ? "Door Open!  " : "Door Closed  ");

  // MQTT Publishing
  if (millis() - lastPublish > 2000) {
    // Publish JSON payload
    client.publish("smart_home/sensors", createSensorPayload(
      temp1, temp2, humidity1, humidity2, 
      gasValue, ldrValue, motionHall, motionRoom2, distance
    ).c_str());
    lastPublish = millis();
  }
  // MQTT Publishing
  // if (millis() - lastPublish > 2000) {
  //   // Publish individual values (optional)
  //   client.publish(TEMP_TOPIC, String(temp1).c_str());
  //   client.publish(HUMIDITY_TOPIC, String(humidity1).c_str());
  //   client.publish(GAS_TOPIC, String(gasValue).c_str());
  //   client.publish(MOTION_TOPIC, String(motionHall).c_str());
  //   client.publish(DOOR_TOPIC, String(distance).c_str());
  //   lastPublish = millis();
  // }
}