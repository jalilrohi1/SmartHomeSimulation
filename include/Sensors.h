#pragma once
#include <DHT.h>

struct SensorData {
  float temp1;
  float temp2;
  float humidity1;
  float humidity2;
  int gasValue;
  int ldrValue;
  bool motionHall;
  bool motionRoom2;
  long doorDistance;
};

class Sensors {
public:
  void begin();
  SensorData readAll();
};