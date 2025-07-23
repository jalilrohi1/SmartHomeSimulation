# Smart Home Simulation - Refined Version

## Overview
This is a refined version of the Smart Home Simulation project that focuses on essential home automation functionalities using ESP32 and various sensors/actuators.

## Features Implemented

### 1. Bedroom Climate Control
- **Sensor**: DHT22 (DHT1_PIN = 4) - Temperature and humidity monitoring
- **Actuator**: Relay Module 1 (RELAY1_PIN = 27) - AC control
- **Function**: Automatically turns on AC when bedroom temperature exceeds 28°C

### 2. Hall Temperature Monitoring
- **Sensor**: DHT22 (DHT2_PIN = 16) - Temperature and humidity monitoring
- **Function**: Provides temperature data for the hall area

### 3. Kitchen Gas Detection System
- **Sensor**: Gas Sensor (GAS_PIN = 36/VP) - Detects gas leaks
- **Actuators**: 
  - Buzzer (BUZZER_PIN = 23) - Alert system
  - Relay Module 2 (RELAY2_PIN = 14) - Exhaust fan control
- **Function**: Triggers alarm and activates exhaust fan when gas levels exceed threshold

### 4. Automated Blinds Control
- **Sensor**: Potentiometer (POT_PIN = 39/VN) - Manual control input
- **Actuator**: Servo Motor (SERVO_PIN = 12) - Blinds positioning
- **Function**: Controls blinds position based on potentiometer setting (0-90 degrees)

### 5. Hall Intelligent Lighting
- **Sensors**: 
  - PIR Motion Sensor (PIR1_PIN = 5) - Motion detection
  - LDR Photoresistor (LDR_PIN = 35) - Light level detection
- **Actuator**: RGB LED (RGB_R=13, RGB_G=26, RGB_B=32) - Smart lighting
- **Function**: Automatically adjusts lighting based on motion and ambient light levels

### 6. Toilet Lighting
- **Sensor**: PIR Motion Sensor (PIR2_PIN = 17) - Motion detection
- **Actuator**: Simple LED (TOILET_LED_PIN = 2) - Basic lighting
- **Function**: Turns on LED when motion is detected in toilet

### 7. Door Security Monitoring
- **Sensor**: Ultrasonic Sensor HC-SR04 (TRIG_PIN=18, ECHO_PIN=19) - Distance measurement
- **Function**: Monitors door status (open/closed) based on distance readings

### 8. System Control Interface
- **Button 1** (BTN1_PIN = 34): Toggle auto mode on/off
- **Button 2** (BTN2_PIN = 33): System power control (enable/disable all actuators)

### 9. Information Display
- **LCD Display** (I2C): Shows system status, sensor readings, and alerts

## System Thresholds

- **AC Activation Temperature**: 28°C
- **Gas Alert Threshold**: 300 (analog reading)
- **Light Darkness Threshold**: 1000 (analog reading)
- **Door Open Distance**: 50cm

## System Modes

### Auto Mode (Default: ON)
- All automated controls are active
- Sensors trigger appropriate actuators
- Can be toggled with Button 1

### Manual Mode
- Automated controls are disabled
- Safety checks remain active
- Manual control via MQTT/Web interface

### System Enable/Disable
- Master control for all actuators
- Button 2 toggles system state
- Safety checks remain active when disabled

## Safety Features

1. **Gas Detection**: Always active regardless of system mode
2. **Emergency Shutdown**: Turns off AC during gas leak
3. **Visual Alerts**: LCD displays emergency messages
4. **MQTT Alerts**: Sends emergency notifications via MQTT

## Communication

### MQTT Topics
- `smarthome/sensors` - Sensor data
- `smarthome/status` - System status
- `smarthome/emergency` - Emergency alerts
- `smarthome/security` - Door status changes

### Data Format
All data is transmitted in JSON format with timestamps and device identification.

## Hardware Requirements

### Microcontroller
- ESP32 DevKit C v4

### Sensors
- 2x DHT22 (Temperature/Humidity)
- 1x Gas Sensor (MQ-2 or similar)
- 1x LDR Photoresistor
- 2x PIR Motion Sensors
- 1x HC-SR04 Ultrasonic Sensor
- 1x Potentiometer

### Actuators
- 2x Relay Modules
- 1x Servo Motor
- 1x RGB LED
- 1x Simple LED
- 1x Buzzer

### Interface
- 1x LCD Display (I2C)
- 2x Push Buttons

## Pin Configuration

| Component | Pin | Description |
|-----------|-----|-------------|
| DHT1 | 4 | Bedroom temperature sensor |
| DHT2 | 16 | Hall temperature sensor |
| PIR1 | 5 | Hall motion sensor |
| PIR2 | 17 | Toilet motion sensor |
| Gas Sensor | 36 (VP) | Kitchen gas sensor |
| LDR | 35 | Light level sensor |
| Potentiometer | 39 (VN) | Blinds control |
| Ultrasonic Trig | 18 | Door sensor trigger |
| Ultrasonic Echo | 19 | Door sensor echo |
| Relay 1 | 27 | AC control |
| Relay 2 | 14 | Exhaust fan |
| Servo | 12 | Blinds motor |
| RGB Red | 13 | Hall light red |
| RGB Green | 26 | Hall light green |
| RGB Blue | 32 | Hall light blue |
| Toilet LED | 2 | Toilet light |
| Buzzer | 23 | Gas alert |
| Button 1 | 34 | Auto mode toggle |
| Button 2 | 33 | System control |
| LCD SDA | 21 | Display data |
| LCD SCL | 22 | Display clock |

## Building and Uploading

1. Install PlatformIO
2. Open project in VS Code with PlatformIO extension
3. Configure WiFi credentials in `config.h`
4. Build project: `platformio run`
5. Upload to ESP32: `platformio run --target upload`

## Configuration

Update the following in `config.h`:
- WiFi SSID and password
- MQTT broker settings
- Device identification

## Monitoring

Use Serial Monitor (115200 baud) to view system logs and debug information.

## Version History

- **v1.0 (Refined)**: Simplified implementation focusing on core functionalities
- Removed complex scheduling and unnecessary features
- Improved code clarity and maintainability
- Enhanced safety and control features
