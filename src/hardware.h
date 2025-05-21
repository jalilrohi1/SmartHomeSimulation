#ifndef HARDWARE_H
#define HARDWARE_H

void readSensors();
void controlOutputs();
void updateLCD();
void controlBuzzer(bool state);
void handleButtons();
void readPotentiometer();

#endif // HARDWARE_H