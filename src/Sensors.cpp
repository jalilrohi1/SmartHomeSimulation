#include "Sensors.h"
#include "pins.h" // Your pin definitions

DHT dht1(DHT1_PIN, DHT22);
DHT dht2(DHT2_PIN, DHT22);

void Sensors::begin() {
  dht1.begin();
  dht2.begin();
  pinMode(PIR1_PIN, INPUT);
  pinMode(PIR2_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

SensorData Sensors::readAll() {
  SensorData data;
  
  // Read DHT sensors with error checking
  data.temp1 = dht1.readTemperature();
  data.temp2 = dht2.readTemperature();
  data.humidity1 = dht1.readHumidity();
  data.humidity2 = dht2.readHumidity();
  
  // Check for failed readings
  if (isnan(data.temp1)) data.temp1 = 0.0;
  if (isnan(data.temp2)) data.temp2 = 0.0;
  if (isnan(data.humidity1)) data.humidity1 = 0.0;
  if (isnan(data.humidity2)) data.humidity2 = 0.0;
  
  data.gasValue = analogRead(GAS_PIN);
  data.ldrValue = analogRead(LDR_PIN);
  data.motionHall = digitalRead(PIR1_PIN);
  data.motionRoom2 = digitalRead(PIR2_PIN);

  // Ultrasonic measurement
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  data.doorDistance = pulseIn(ECHO_PIN, HIGH) * 0.034 / 2;

  return data;
}