#pragma once
#include <ESP32Servo.h>

class Actuators {
public:
  void begin();
  
  // Climate control
  void controlAC(bool state);           // Bedroom AC control (Relay1)
  void controlExhaustFan(bool state);   // Kitchen exhaust fan (Relay2)
  
  // Blinds control
  void controlBlinds(int angle);        // Servo motor for blinds

  // Alert system
  void buzzerAlert(bool state);         // Gas leak buzzer
  
  // Lighting control
  void hallRGBControl(int r, int g, int b);  // RGB LED for hall
  void toiletLED(bool state);           // Simple LED for toilet
  
  static Servo blindsServo;
  
};