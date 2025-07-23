#include "Sensors.h"
#include "pins.h"

// Add static arrays for filtering
static int lightHistory[5] = {0, 0, 0, 0, 0};
static int potHistory[5] = {0, 0, 0, 0, 0};
static int gasHistory[3] = {0, 0, 0};
static int lightIndex = 0;
static int potIndex = 0;
static int gasIndex = 0;

Sensors::Sensors() : dht1(DHT1_PIN, DHT22), dht2(DHT2_PIN, DHT22) {
  // Constructor initializes DHT sensors with proper pins
}

void Sensors::begin() {
  dht1.begin();
  dht2.begin();
  
  // Motion sensors
  pinMode(PIR1_PIN, INPUT);
  pinMode(PIR2_PIN, INPUT);
  
  // Ultrasonic sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Control buttons
  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);
  
  // Gas sensor initialization
  pinMode(GAS_PIN, INPUT);
  
 Serial.println("Sensors initialized successfully");
}

SensorData Sensors::readAll() {
  SensorData data;
  
  // Read temperature and humidity sensors
  data.bedroomTemp = dht1.readTemperature();
  data.bedroomHumidity = dht1.readHumidity();
  data.hallTemp = dht2.readTemperature();
  data.hallHumidity = dht2.readHumidity();
  
  // Check for failed readings and provide defaults
  if (isnan(data.bedroomTemp)) data.bedroomTemp = 25.0;
  if (isnan(data.bedroomHumidity)) data.bedroomHumidity = 50.0;
  if (isnan(data.hallTemp)) data.hallTemp = 25.0;
  if (isnan(data.hallHumidity)) data.hallHumidity = 50.0;
  
  // // Environmental sensors 
 data.gasValue = analogRead(GAS_PIN);
  delay(10);  // Small delay between ADC readings
  data.lightLevel = analogRead(LDR_PIN);
  delay(10);  // Delay to let ADC settle
  data.potValue = analogRead(POT_PIN);
  delay(10);  // Final delay
  
  // Motion sensors
  data.hallMotion = digitalRead(PIR1_PIN);
  data.toiletMotion = digitalRead(PIR2_PIN);

  // Ultrasonic sensor for door
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  data.doorDistance = duration * 0.034 / 2; // Convert to cm
  
  // Button readings with debouncing
  unsigned long currentTime = millis();
  
  // Auto mode button (BTN1)
  bool currentAutoBtn = !digitalRead(BTN1_PIN); // Inverted because of pullup
  if (currentAutoBtn != lastAutoBtn && (currentTime - lastDebounceTime1) > DEBOUNCE_DELAY) {
    data.autoModeBtn = currentAutoBtn;
    lastAutoBtn = currentAutoBtn;
    lastDebounceTime1 = currentTime;
  } else {
    data.autoModeBtn = false; // Only true on button press
  }
  
  // System button (BTN2)
  bool currentSystemBtn = !digitalRead(BTN2_PIN); // Inverted because of pullup
  if (currentSystemBtn != lastSystemBtn && (currentTime - lastDebounceTime2) > DEBOUNCE_DELAY) {
    data.systemBtn = currentSystemBtn;
    lastSystemBtn = currentSystemBtn;
    lastDebounceTime2 = currentTime;
  } else {
    data.systemBtn = false; // Only true on button press
  }

  return data;
}