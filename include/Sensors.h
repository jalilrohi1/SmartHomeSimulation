#pragma once
#include <DHT.h>

struct SensorData {
  // Temperature sensors
  float bedroomTemp;      // DHT1 - bedroom temperature
  float bedroomHumidity;  // DHT1 - bedroom humidity
  float hallTemp;         // DHT2 - hall temperature  
  float hallHumidity;     // DHT2 - hall humidity
  
  // Environmental sensors
  int gasValue;           // Gas sensor reading
  int lightLevel;         // LDR reading for brightness
  int potValue;           // Potentiometer reading for blinds
  
  // Motion sensors
  bool hallMotion;        // PIR1 - hall motion detection
  bool toiletMotion;      // PIR2 - toilet motion detection
  
  // Door sensor
  long doorDistance;      // Ultrasonic sensor reading
  
  // Control buttons
  bool autoModeBtn;       // Button 1 state
  bool systemBtn;         // Button 2 state
};

class Sensors {
public:
  Sensors(); // Constructor declaration
  void begin();
  SensorData readAll();
  
private:
  DHT dht1;
  DHT dht2;
  
  // Button state management
  bool lastAutoBtn = false;
  bool lastSystemBtn = false;
  unsigned long lastDebounceTime1 = 0;
  unsigned long lastDebounceTime2 = 0;
  static const unsigned long DEBOUNCE_DELAY = 50;
};