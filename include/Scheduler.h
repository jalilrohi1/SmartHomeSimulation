#pragma once
#include <Arduino.h>
#include "Actuators.h"

struct ScheduleTask {
  int hour;
  int minute;
  String action;
  String parameter;
  bool enabled;
  bool executed;
};

class Scheduler {
private:
  ScheduleTask tasks[10]; // Maximum 10 scheduled tasks
  int taskCount;
  Actuators* actuators;
  unsigned long lastTimeCheck;

public:
  Scheduler();
  void begin(Actuators* actuatorPtr);
  void addTask(int hour, int minute, String action, String parameter = "");
  void removeTask(int index);
  void checkTasks();
  void executeTask(const ScheduleTask& task);
  String getTasksList();
  void resetDailyTasks();
  int getCurrentHour();
  int getCurrentMinute();
};
