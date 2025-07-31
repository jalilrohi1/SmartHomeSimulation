#pragma once
// Pin Definitions for Smart Home Simulation

// Temperature Sensors
const int DHT1_PIN = 4;        // Bedroom temperature sensor
const int DHT2_PIN = 16;       // Hall temperature sensor

// Actuators
const int RELAY1_PIN = 27;     // AC control for bedroom
const int RELAY2_PIN = 14;     // Exhaust fan for kitchen
//const int SERVO_PIN = 12;      // Blinds control servo

// Motion Sensors
const int PIR1_PIN = 5;        // Hall motion sensor
const int PIR2_PIN = 17;       // Toilet motion sensor

// Environmental Sensors
const int GAS_PIN = 36;        // Kitchen gas sensor (VP pin)
const int LDR_PIN = 34;        // Light sensor for brightness control
//const int POT_PIN = 39;        // Potentiometer for blinds control (VN pin)

// Door Sensor
const int TRIG_PIN = 18;       // Ultrasonic sensor trigger
const int ECHO_PIN = 19;       // Ultrasonic sensor echo

// Alert System
const int BUZZER_PIN = 23;     // Gas leak alert buzzer

// Lighting Control
const int RGB_R = 13;          // RGB Red channel for hall
const int RGB_G = 26;          // RGB Green channel for hall
const int RGB_B = 32;          // RGB Blue channel for hall

// Control Buttons
const int BTN1_PIN = 35;       // Auto mode toggle button
const int BTN2_PIN = 33;       // System power/restart button

// Additional LED for toilet
const int TOILET_LED_PIN = 25; // Simple LED for toilet motion (using built-in LED or GPIO2)

// PWM Channels
const int PWM_RED = 0;
const int PWM_GREEN = 1;
const int PWM_BLUE = 2;
const int PWM_TOILET_LED = 3;

// #define PWM_RED       0
// #define PWM_GREEN     1
// #define PWM_BLUE      2
// #define PWM_TOILET_LED 3