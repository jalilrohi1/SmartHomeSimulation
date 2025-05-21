#include "Display.h"
#include "pins.h"
#include <Wire.h>

LiquidCrystal_I2C lcd2(0x27, 16, 2);

void Display::initialize() {
  //lcd2.setSimulatedWokwi(true);
  lcd2.init();
  lcd2.backlight();
}
// void Display::initialize() {
//   lcd2.init();
//   lcd2.backlight();
//   lcd2.setBacklight(255);
//   delay(100); // Add small delay
//   lcd2.clear();
//   lcd2.print("System Ready"); // Test message
// }
void Display::showStatus(const char* line1, const char* line2) {
  lcd2.clear();
  lcd2.setCursor(0, 0);
  lcd2.print(line1);
  lcd2.setCursor(0, 1);
  lcd2.print(line2);
}

void Display::updateDoorStatus(bool isOpen) {
  lcd2.setCursor(0, 1);
  lcd2.print(isOpen ? "Door Open!  " : "Door Closed  ");
}