#include "Scheduler.h"

#if ENABLE_SCHEDULER

#include <time.h>

Scheduler::Scheduler() : taskCount(0), lastTimeCheck(0) {}

void Scheduler::begin(Actuators* actuatorPtr) {
  actuators = actuatorPtr;
  
  // Add some default scheduled tasks
  addTask(7, 0, "ac_on");           // Turn on AC at 7:00 AM
  addTask(22, 0, "ac_off");         // Turn off AC at 10:00 PM
  addTask(18, 30, "rgb", "255,100,0"); // Warm light at 6:30 PM
  addTask(23, 0, "rgb", "0,0,0");   // Turn off lights at 11:00 PM
  addTask(8, 0, "blinds_open");     // Open blinds at 8:00 AM
  addTask(20, 0, "blinds_close");   // Close blinds at 8:00 PM
}

void Scheduler::addTask(int hour, int minute, String action, String parameter) {
  if (taskCount < 10) {
    tasks[taskCount] = {hour, minute, action, parameter, true, false};
    taskCount++;
    Serial.printf("Scheduled task added: %02d:%02d - %s %s\n", hour, minute, action.c_str(), parameter.c_str());
  }
}

void Scheduler::removeTask(int index) {
  if (index >= 0 && index < taskCount) {
    for (int i = index; i < taskCount - 1; i++) {
      tasks[i] = tasks[i + 1];
    }
    taskCount--;
  }
}

void Scheduler::checkTasks() {
  unsigned long currentTime = millis();
  
  // Check tasks every minute
  if (currentTime - lastTimeCheck > 60000) {
    lastTimeCheck = currentTime;
    
    int currentHour = getCurrentHour();
    int currentMinute = getCurrentMinute();
    
    for (int i = 0; i < taskCount; i++) {
      if (tasks[i].enabled && !tasks[i].executed) {
        if (tasks[i].hour == currentHour && tasks[i].minute == currentMinute) {
          executeTask(tasks[i]);
          tasks[i].executed = true;
        }
      }
    }
  }
}

void Scheduler::executeTask(const ScheduleTask& task) {
  Serial.printf("Executing scheduled task: %s %s\n", task.action.c_str(), task.parameter.c_str());
  
  if (task.action == "ac_on") {
    actuators->controlAC(true);
  } else if (task.action == "ac_off") {
    actuators->controlAC(false);
  } else if (task.action == "blinds_open") {
    actuators->controlBlinds(0);
  } else if (task.action == "blinds_close") {
    actuators->controlBlinds(90);
  } else if (task.action == "rgb") {
    // Parse RGB values from parameter (format: "r,g,b")
    int r = 0, g = 0, b = 0;
    sscanf(task.parameter.c_str(), "%d,%d,%d", &r, &g, &b);
    actuators->rgbControl(r, g, b);
  }
}

String Scheduler::getTasksList() {
  String result = "[";
  for (int i = 0; i < taskCount; i++) {
    if (i > 0) result += ",";
    result += "{";
    result += "\"time\":\"" + String(tasks[i].hour) + ":" + String(tasks[i].minute) + "\",";
    result += "\"action\":\"" + tasks[i].action + "\",";
    result += "\"parameter\":\"" + tasks[i].parameter + "\",";
    result += "\"enabled\":" + String(tasks[i].enabled ? "true" : "false") + ",";
    result += "\"executed\":" + String(tasks[i].executed ? "true" : "false");
    result += "}";
  }
  result += "]";
  return result;
}

void Scheduler::resetDailyTasks() {
  // Reset executed status for all tasks (call this daily)
  for (int i = 0; i < taskCount; i++) {
    tasks[i].executed = false;
  }
  Serial.println("Daily tasks reset");
}

int Scheduler::getCurrentHour() {
  // Simulate time based on millis() for demonstration
  // In real implementation, use RTC or NTP
  return (millis() / 3600000) % 24;
}

int Scheduler::getCurrentMinute() {
  // Simulate minutes based on millis()
  return (millis() / 60000) % 60;
}

#endif // ENABLE_SCHEDULER
