#include "Actuators.h"
#include "pins.h"

void Actuators::begin() {
  // Initialize servo for blinds with enhanced setup
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  
  // Initialize relay pins
  pinMode(RELAY1_PIN, OUTPUT);  // AC control
  pinMode(RELAY2_PIN, OUTPUT);  // Exhaust fan control
  
  // Initialize buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initialize toilet LED
  pinMode(TOILET_LED_PIN, OUTPUT);
  
  // Initialize PWM channels for RGB LED
  ledcSetup(PWM_RED, 5000, 8);
  ledcAttachPin(RGB_R, PWM_RED);
  ledcSetup(PWM_GREEN, 5000, 8);
  ledcAttachPin(RGB_G, PWM_GREEN);
  ledcSetup(PWM_BLUE, 5000, 8);
  ledcAttachPin(RGB_B, PWM_BLUE);

  ledcSetup(PWM_TOILET_LED, 5000, 8);
  ledcAttachPin(TOILET_LED_PIN, PWM_TOILET_LED);
  
  // Initialize all outputs to OFF state
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(TOILET_LED_PIN, LOW);
  hallRGBControl(0, 0, 0);
  
  delay(100);  // Give servo time to move

  Serial.println("Actuators initialized successfully");
}

void Actuators::controlAC(bool state) {
  digitalWrite(RELAY1_PIN, state);
  //Serial.println("AC " + String(state ? "ON" : "OFF"));
}

void Actuators::controlExhaustFan(bool state) {
  digitalWrite(RELAY2_PIN, state);
  //Serial.println("Exhaust Fan " + String(state ? "ON" : "OFF"));
}

void Actuators::buzzerAlert(bool state) {
  delay(50); // Short delay to avoid rapid toggling
  digitalWrite(BUZZER_PIN, state);
  delay(50); // Ensure buzzer is stable before next action
  if (state) {
    //Serial.println("⚠️  BUZZER ALERT ACTIVATED ⚠️");
  }
}

void Actuators::hallRGBControl(int r, int g, int b) {
  r = constrain(r, 0, 255);
  g = constrain(g, 0, 255);
  b = constrain(b, 0, 255);
  ledcWrite(PWM_RED, r);
  ledcWrite(PWM_GREEN, g);
  ledcWrite(PWM_BLUE, b);
  return;
}

void Actuators::toiletLED(bool state) {
  if (state) {
    ledcWrite(PWM_TOILET_LED, 255); // Full brightness
  } else {
    ledcWrite(PWM_TOILET_LED, 0);   // Off
  }
  //Serial.println("Toilet LED " + String(state ? "ON" : "OFF"));
}