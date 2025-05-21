#include "globals.h"
#include "hardware.h"
#include "mqtt.h"
#include "utils.h"

// Define global variables
LiquidCrystal_I2C lcd2(0x27, 16, 2);
DHT dht1(4, DHT22);
DHT dht2(16, DHT22);
Servo blindsServo;
float tempReadings[TEMP_AVG_WINDOW] = {0};
int tempIndex = 0;
float temp1, temp2, humidity1, humidity2, avgTemp;
int gasValue, ldrValue;
bool motionHall, motionRoom2;
long distance;
unsigned long lastPublish = 0;
bool manualMode = false;
bool relay1State = false;
bool relay2State = false;
int currentGasThreshold = GAS_THRESHOLD; // Replace with actual value, e.g., 400

void setup() {
  // Your setup code here
  Serial.begin(115200);
  lcd2.init();
  lcd2.backlight();
  dht1.begin();
  dht2.begin();
  blindsServo.attach(SERVO_PIN); // Replace SERVO_PIN with actual pin
  // Add WiFi, MQTT, and other initializations
}

void loop() {
  // Your loop code here
  readSensors();
  controlOutputs();
  handleMQTT();
  // Add other function calls as needed
}