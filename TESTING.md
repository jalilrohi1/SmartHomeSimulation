# Smart Home Simulation v2.1 - Testing Guide

## 🧪 Testing Checklist

### 1. Basic Functionality Tests
- [ ] ESP32 boots and connects to WiFi
- [ ] MQTT connection established
- [ ] LCD display shows rotating information
- [ ] All sensors provide readings
- [ ] Web dashboard accessible

### 2. Web Dashboard Tests
- [ ] Dashboard loads at ESP32 IP address
- [ ] Real-time data updates every 2 seconds
- [ ] AC control buttons work
- [ ] Blinds control buttons work
- [ ] RGB sliders control lighting
- [ ] Emergency stop button works
- [ ] Mobile responsive design

### 3. MQTT Tests
Send these commands to test MQTT control:

```bash
# Relay Control
mosquitto_pub -h test.mosquitto.org -t "smart_home/relay" -m "AC_ON"
mosquitto_pub -h test.mosquitto.org -t "smart_home/relay" -m "AC_OFF"

# Lighting Control
mosquitto_pub -h test.mosquitto.org -t "smart_home/lighting" -m "RGB:255,100,50"

# Blinds Control
mosquitto_pub -h test.mosquitto.org -t "smart_home/blinds" -m "90"

# Emergency Stop
mosquitto_pub -h test.mosquitto.org -t "smart_home/control" -m "EMERGENCY_STOP"
```

### 4. Automated Scheduling Tests
- [ ] Tasks execute at scheduled times
- [ ] Daily task reset works
- [ ] Scheduler status visible in web dashboard

### 5. Safety Features Tests
- [ ] Gas threshold triggers alarm
- [ ] Emergency MQTT alerts sent
- [ ] AC auto-shutdown during gas leak
- [ ] Door monitoring alerts

### 6. Energy Monitoring Tests
- [ ] AC runtime tracked
- [ ] Lighting time recorded
- [ ] Data visible in web dashboard

### 7. Network Resilience Tests
- [ ] WiFi reconnection works
- [ ] MQTT reconnection works
- [ ] System continues operating during network outages

## 📊 Expected Performance
- Sensor update rate: 100ms
- MQTT publish rate: 5 seconds
- Web update rate: 2 seconds
- Memory usage: < 50% heap
- Response time: < 200ms

## 🐛 Known Issues
1. Servo control in MQTT callback needs actuator reference
2. Time simulation based on millis() (use RTC for production)
3. Energy monitoring resets on system restart

## 🔧 Debugging Tips
1. Monitor Serial output at 115200 baud
2. Check WiFi signal strength in web dashboard
3. Verify MQTT broker connectivity
4. Use browser developer tools for web debugging
