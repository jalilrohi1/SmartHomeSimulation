# Smart Home Simulation v2.0

## 🚀 Version 2.0 Features

### Major Enhancements
- **Web Dashboard**: Real-time monitoring and control via responsive web interface
- **Automated Scheduling**: Time-based device control with 6 pre-configured tasks
- **Enhanced MQTT Control**: Full actuator control with emergency features
- **Energy Monitoring**: Track AC runtime and lighting usage
- **Improved Safety**: Gas leak detection with automatic emergency response
- **Security Features**: Door monitoring with alerts
- **Multi-Screen Display**: Rotating LCD display with system information

### New Web Dashboard Features
- Real-time sensor data visualization
- RGB lighting control with sliders
- Climate control (AC, blinds)
- System status monitoring
- Emergency stop functionality
- Mobile-friendly responsive design

### Automated Scheduling
- 7:00 AM - AC ON
- 8:00 AM - Open blinds
- 6:30 PM - Warm lighting
- 8:00 PM - Close blinds
- 10:00 PM - AC OFF
- 11:00 PM - Lights OFF

### API Endpoints
- `GET /` - Web dashboard
- `GET /api/data` - Sensor data (JSON)
- `POST /api/control` - Device control

### MQTT Topics
- `smart_home/sensors` - Sensor data
- `smart_home/status` - System status
- `smart_home/emergency` - Emergency alerts
- `smart_home/security` - Security events
- `smart_home/relay` - Relay control
- `smart_home/lighting` - RGB control
- `smart_home/blinds` - Blinds control
- `smart_home/control` - General commands

## 🔧 Setup Instructions

1. Install dependencies via PlatformIO
2. Upload code to ESP32
3. Connect to WiFi network
4. Access web dashboard at ESP32's IP address
5. Monitor via MQTT broker: `test.mosquitto.org`

## 📊 Energy Monitoring
- AC runtime tracking (seconds)
- Lighting usage monitoring
- Real-time power consumption insights

## 🛡️ Safety Features
- Gas leak automatic detection and response
- Emergency stop (MQTT: `smart_home/control` → `EMERGENCY_STOP`)
- Automatic AC shutdown during gas leaks
- Security alerts for door monitoring

## 🌐 Network Features
- WiFi auto-reconnection
- MQTT auto-reconnection with retry logic
- Web server with CORS support
- Real-time data updates (2-second intervals)

## Version History
- v1.0: Basic sensor monitoring and MQTT
- v2.0: Web dashboard, scheduling, energy monitoring, enhanced safety
