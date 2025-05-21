#ifndef GLOBALS_H
#define GLOBALS_H

#include <DHT.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <esp_task_wdt.h>

// Hardware instances
extern LiquidCrystal_I2C lcd2;
extern DHT dht1;
extern DHT dht2;
extern Servo blindsServo;

// Global variables
extern float tempReadings[TEMP_AVG_WINDOW];
extern int tempIndex;
extern float temp1, temp2, humidity1, humidity2, avgTemp;
extern int gasValue, ldrValue;
extern bool motionHall, motionRoom2;
extern long distance;
extern unsigned long lastPublish;
extern bool manualMode;
extern bool relay1State;
extern bool relay2State;
extern int currentGasThreshold;

#endif // GLOBALS_H