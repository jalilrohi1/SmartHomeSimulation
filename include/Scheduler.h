#pragma once
#include "config.h"

#if ENABLE_SCHEDULER

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

#else

// Dummy class when scheduler is disabled
class Scheduler {
public:
  Scheduler() {}
  void begin(void* actuatorPtr) {}
  void addTask(int hour, int minute, String action, String parameter = "") {}
  void removeTask(int index) {}
  void checkTasks() {}
  void executeTask(const void* task) {}
  String getTasksList() { return "[]"; }
  void resetDailyTasks() {}
  int getCurrentHour() { return 12; }
  int getCurrentMinute() { return 0; }
};

#endif // ENABLE_SCHEDULER
