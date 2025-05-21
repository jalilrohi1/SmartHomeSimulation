#include "utils.h"
#include "globals.h"

String createSensorPayload() {
  StaticJsonDocument<200> doc;
  doc["temp1"] = temp1;
  doc["temp2"] = temp2;
  doc["humidity1"] = humidity1;
  doc["humidity2"] = humidity2;
  doc["gas"] = gasValue;
  doc["ldr"] = ldrValue;
  String payload;
  serializeJson(doc, payload);
  return payload;
}