#pragma once
#include <ESP32Servo.h>

class Actuators {
public:
  void begin();
  void controlAC(bool state);
  void controlBlinds(int angle);
  void buzzerAlert(bool state);
  void rgbControl(int r, int g, int b);
};