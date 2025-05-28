# Smart Home v2.1 - Feature Configuration Guide

## 🎛️ Configurable Features

The Smart Home v2.1 system now supports **modular features** that can be easily enabled or disabled based on your requirements. This allows you to customize the system for different use cases.

## 📝 Configuration File

All feature toggles are located in `include/config.h`:

```cpp
// Feature Configuration - Enable/Disable Components
#define ENABLE_WEB_SERVER true     // Set to false to disable web server
#define ENABLE_MQTT true           // Set to false to disable MQTT
#define ENABLE_SCHEDULER true      // Set to false to disable automation
#define ENABLE_ENERGY_MONITOR true // Set to false to disable energy monitoring
```

## 🔧 Available Features

### 1. Web Server (`ENABLE_WEB_SERVER`)
**Default: Enabled**

**When Enabled:**
- ✅ Full web dashboard accessible at ESP32 IP address
- ✅ Real-time sensor data visualization
- ✅ Device control via web interface
- ✅ Emergency stop functionality
- ✅ Mobile-responsive design

**When Disabled:**
- ❌ Web server completely removed from compilation
- ✅ Reduces memory usage by ~15KB Flash
- ✅ Perfect for Node-RED only setups
- ✅ Lower power consumption

**Use Cases:**
- **Enable**: When you want standalone device access or mobile control
- **Disable**: When using only Node-RED for visualization and control

### 2. MQTT Communication (`ENABLE_MQTT`)
**Default: Enabled**

**When Enabled:**
- ✅ Full MQTT broker connectivity
- ✅ Real-time data publishing
- ✅ Remote device control
- ✅ Integration with Node-RED, Home Assistant, etc.

**When Disabled:**
- ❌ No MQTT functionality
- ✅ Standalone operation only
- ✅ Reduced network dependencies

### 3. Automated Scheduler (`ENABLE_SCHEDULER`)
**Default: Enabled**

**When Enabled:**
- ✅ Time-based automation (6 pre-configured tasks)
- ✅ Daily schedule execution
- ✅ Energy-saving routines

**When Disabled:**
- ❌ No automatic scheduling
- ✅ Manual control only
- ✅ Simpler operation

**Default Schedule:**
- 7:00 AM - AC ON
- 8:00 AM - Open blinds
- 6:30 PM - Warm lighting
- 8:00 PM - Close blinds
- 10:00 PM - AC OFF
- 11:00 PM - Lights OFF

### 4. Energy Monitoring (`ENABLE_ENERGY_MONITOR`)
**Default: Enabled**

**When Enabled:**
- ✅ AC runtime tracking
- ✅ Lighting usage monitoring
- ✅ Performance metrics

**When Disabled:**
- ❌ No energy tracking
- ✅ Simplified data structure

## 🎯 Common Configuration Scenarios

### Scenario 1: Node-RED Only Setup
```cpp
#define ENABLE_WEB_SERVER false    // Use Node-RED for visualization
#define ENABLE_MQTT true           // Keep MQTT for Node-RED communication
#define ENABLE_SCHEDULER true      // Keep automation
#define ENABLE_ENERGY_MONITOR true // Keep energy tracking
```

**Benefits:**
- Reduces flash usage by ~20KB
- Lower memory consumption
- Focus on Node-RED dashboard
- Maintains full automation

### Scenario 2: Standalone Web Interface
```cpp
#define ENABLE_WEB_SERVER true     // Full web dashboard
#define ENABLE_MQTT false          // No external dependencies
#define ENABLE_SCHEDULER true      // Keep automation
#define ENABLE_ENERGY_MONITOR false // Simplified operation
```

**Benefits:**
- Self-contained system
- No network dependencies beyond WiFi
- Perfect for local control

### Scenario 3: Minimal Sensor Node
```cpp
#define ENABLE_WEB_SERVER false    // No web interface
#define ENABLE_MQTT true           // Data publishing only
#define ENABLE_SCHEDULER false     // Manual control
#define ENABLE_ENERGY_MONITOR false // Basic monitoring
```

**Benefits:**
- Smallest memory footprint
- Pure sensor data collection
- External automation systems

### Scenario 4: Development/Testing
```cpp
#define ENABLE_WEB_SERVER true     // Quick access for testing
#define ENABLE_MQTT true           // Full connectivity
#define ENABLE_SCHEDULER false     // Manual testing
#define ENABLE_ENERGY_MONITOR true // Performance monitoring
```

**Benefits:**
- Full debugging capabilities
- Manual control for testing
- Performance monitoring

## 🔄 How to Change Configuration

1. **Edit Configuration:**
   ```cpp
   // In include/config.h
   #define ENABLE_WEB_SERVER false  // Disable web server
   ```

2. **Clean and Rebuild:**
   ```bash
   platformio run --target clean
   platformio run
   ```

3. **Upload to Device:**
   ```bash
   platformio run --target upload
   ```

## 📊 Memory Usage Comparison

| Configuration | Flash Usage | RAM Usage | Features |
|--------------|-------------|-----------|----------|
| **Full (All Enabled)** | ~851KB | ~46KB | Complete system |
| **Node-RED Only** | ~830KB | ~42KB | MQTT + Automation |
| **Standalone Web** | ~845KB | ~44KB | Web + Local control |
| **Minimal Sensor** | ~820KB | ~40KB | Basic monitoring |

## 🚀 Runtime Feature Control

You can also enable/disable the web server at runtime:

```cpp
// In your code
webServer.disable();  // Turn off web server
webServer.enable();   // Turn on web server

// Check status
if (webServer.isWebServerEnabled()) {
  Serial.println("Web server is running");
}
```

## 🛠️ Feature Status Display

The system automatically displays the current feature configuration on startup:

```
=== Smart Home v2.1 Feature Configuration ===
✓ Web Server: ENABLED (Port 80)
✓ MQTT: ENABLED (test.mosquitto.org)
✓ Automation Scheduler: ENABLED
✓ Energy Monitoring: ENABLED
===========================================
```

## 📋 Best Practices

1. **Start with Full Configuration** - Test all features first
2. **Disable Unused Features** - Optimize for your specific use case
3. **Document Your Choice** - Note why certain features are disabled
4. **Test After Changes** - Verify system works as expected
5. **Backup Working Configs** - Keep known-good configurations

## 🔗 Integration Guidelines

### With Node-RED:
- Keep MQTT enabled
- Web server optional (disable to save memory)
- Enable scheduler for automation
- Energy monitoring useful for dashboards

### With Home Assistant:
- Keep MQTT enabled
- Web server as backup interface
- Scheduler for local automation
- Energy monitoring for statistics

### Standalone Operation:
- Enable web server for control
- MQTT optional
- Scheduler recommended
- Energy monitoring optional

This modular approach makes the Smart Home v2.1 system highly adaptable to different deployment scenarios while maintaining optimal performance.
