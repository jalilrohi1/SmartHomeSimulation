#pragma once
#include <LiquidCrystal_I2C.h>

class Display {
public:
  void initialize();
  void showStatus(const char* line1, const char* line2);
  void updateDoorStatus(bool isOpen);
};