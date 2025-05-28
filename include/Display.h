#pragma once
#include <LiquidCrystal_I2C.h>

class Display {
public:
  void initialize();
  void showStatus(const char* line1, const char* line2);
  void showStatus(String line1, String line2); // Add overload for String
  void updateDoorStatus(bool isOpen);
};