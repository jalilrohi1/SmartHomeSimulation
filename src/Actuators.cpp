#include "Actuators.h"
#include "pins.h"

Servo blindsServo;

void Actuators::begin() {
  blindsServo.attach(SERVO_PIN);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initialize PWM channels
  ledcSetup(PWM_RED, 5000, 8);
  ledcAttachPin(RGB_R, PWM_RED);
  ledcSetup(PWM_GREEN, 5000, 8);
  ledcAttachPin(RGB_G, PWM_GREEN);
  ledcSetup(PWM_BLUE, 5000, 8);
  ledcAttachPin(RGB_B, PWM_BLUE);
}

void Actuators::controlAC(bool state) {
  digitalWrite(RELAY2_PIN, state);
}

void Actuators::buzzerAlert(bool state) {
  digitalWrite(BUZZER_PIN, state);
}

void Actuators::controlBlinds(int angle) {
  blindsServo.write(angle);
}

void Actuators::rgbControl(int r, int g, int b) {
  ledcWrite(PWM_RED, r);
  ledcWrite(PWM_GREEN, g);
  ledcWrite(PWM_BLUE, b);
}