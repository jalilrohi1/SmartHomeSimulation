#include "hardware.h"
#include "globals.h"

void readSensors() {
  temp1 = dht1.readTemperature();
  if (isnan(temp1)) {
    Serial.println("Failed to read DHT1 temperature!");
    temp1 = -999;
  } else {
    tempReadings[tempIndex] = temp1;
    tempIndex = (tempIndex + 1) % TEMP_AVG_WINDOW;
    float sum = 0;
    for (int i = 0; i < TEMP_AVG_WINDOW; i++) {
      sum += tempReadings[i];
    }
    avgTemp = sum / TEMP_AVG_WINDOW;
  }
  temp2 = dht2.readTemperature();
  humidity1 = dht1.readHumidity();
  humidity2 = dht2.readHumidity();
  // Add gas, LDR, PIR, ultrasonic readings as needed
}

void controlOutputs() {
  if (!manualMode) {
    if (gasValue > currentGasThreshold) {
      controlBuzzer(true);
    } else {
      controlBuzzer(false);
    }
    digitalWrite(RELAY2_PIN, (avgTemp > TEMP_THRESHOLD) ? HIGH : LOW); // Replace RELAY2_PIN, TEMP_THRESHOLD
    // Add relay, servo, RGB LED controls
  }
}

void updateLCD() {
  lcd2.setCursor(0, 0);
  lcd2.print("Temp: ");
  lcd2.print(avgTemp);
  lcd2.print("C");
  // Add more LCD updates
}

void controlBuzzer(bool state) {
  digitalWrite(BUZZER_PIN, state ? HIGH : LOW); // Replace BUZZER_PIN
}

void handleButtons() {
  // Implement button handling logic
}

void readPotentiometer() {
  // Implement potentiometer reading logic
}