#include "WebServer.h"

#if ENABLE_WEB_SERVER

#include "pins.h"

SmartHomeWebServer::SmartHomeWebServer() : server(WEB_SERVER_PORT), isEnabled(true) {}

void SmartHomeWebServer::begin(Sensors* sensorPtr, Actuators* actuatorPtr) {
  sensors = sensorPtr;
  actuators = actuatorPtr;
  
  if (isEnabled) {
           <h1>🏠 Smart Home v2.1 - Quick Access</h1>server.begin();
    Serial.println("✓ Web Server started on port " + String(WEB_SERVER_PORT));
    Serial.print("✓ Access dashboard at: http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("✗ Web Server is disabled");
  }
}

void SmartHomeWebServer::enable() {
  isEnabled = true;
  server.begin();
  Serial.println("✓ Web Server enabled");
}

void SmartHomeWebServer::disable() {
  isEnabled = false;
  server.stop();
  Serial.println("✗ Web Server disabled");
}

bool SmartHomeWebServer::isWebServerEnabled() const {
  return isEnabled;
}

void SmartHomeWebServer::handleClient() {
  if (!isEnabled) return; // Skip if web server is disabled
  
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New web client connected");
    String currentLine = "";
    String requestLine = "";
    bool isFirstLine = true;
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Parse request type
            if (requestLine.indexOf("GET /api/data") >= 0) {
              // API data request
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type: application/json");
              client.println("Access-Control-Allow-Origin: *");
              client.println("Connection: close");
              client.println();
              client.println(getApiData());
            } else if (requestLine.indexOf("POST /api/control") >= 0) {
              // Control request (simplified)
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type: text/plain");
              client.println("Connection: close");
              client.println();
              client.println("OK");
            } else {
              // Default homepage
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type: text/html");
              client.println("Connection: close");
              client.println();
              client.println(getSimplePage());
            }
            break;
          } else {
            if (isFirstLine) {
              requestLine = currentLine;
              isFirstLine = false;
            }
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Web client disconnected");
  }
}

void SmartHomeWebServer::stop() {
  server.stop();
  Serial.println("Web server stopped");
}

String SmartHomeWebServer::getApiData() {
  SensorData data = sensors->readAll();
  
  StaticJsonDocument<512> doc;
  doc["temperature"]["room1"] = data.temp1;
  doc["temperature"]["room2"] = data.temp2;
  doc["humidity"]["room1"] = data.humidity1;
  doc["humidity"]["room2"] = data.humidity2;
  doc["gas"] = data.gasValue;
  doc["light"] = data.ldrValue;
  doc["motion"]["hall"] = data.motionHall;
  doc["motion"]["room2"] = data.motionRoom2;
  doc["door"]["distance"] = data.doorDistance;
  doc["door"]["status"] = data.doorDistance > 50 ? "open" : "closed";
  doc["timestamp"] = millis();
  doc["wifi_signal"] = WiFi.RSSI();
  doc["uptime"] = millis() / 1000;
  
  String output;
  serializeJson(doc, output);
  return output;
}

String SmartHomeWebServer::getHomePage() {
  return R"HTML(
<!DOCTYPE html>
<html>
<head>
    <title>Smart Home Dashboard v2</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background: #f5f5f5; }
        .container { max-width: 1200px; margin: 0 auto; }
        .header { background: #2c3e50; color: white; padding: 20px; border-radius: 10px; margin-bottom: 20px; }
        .card { background: white; padding: 20px; margin: 10px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; }
        .sensor-value { font-size: 2em; color: #27ae60; font-weight: bold; }
        .control-btn { background: #3498db; color: white; border: none; padding: 10px 20px; margin: 5px; border-radius: 5px; cursor: pointer; }
        .control-btn:hover { background: #2980b9; }
        .emergency { background: #e74c3c !important; }
        .status-indicator { width: 20px; height: 20px; border-radius: 50%; display: inline-block; margin-right: 10px; }
        .status-on { background: #27ae60; }
        .status-off { background: #e74c3c; }
        .rgb-control { margin: 10px 0; }
        .rgb-control input { margin: 5px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🏠 Smart Home Dashboard v2.1</h1>
            <p>Real-time monitoring and control system</p>
        </div>

        <div class="grid">
            <!-- Environmental Sensors -->
            <div class="card">
                <h3>🌡️ Environmental Sensors</h3>
                <div>Room 1 - Temp: <span id="temp1" class="sensor-value">--</span>°C</div>
                <div>Room 1 - Humidity: <span id="humidity1" class="sensor-value">--</span>%</div>
                <div>Room 2 - Temp: <span id="temp2" class="sensor-value">--</span>°C</div>
                <div>Room 2 - Humidity: <span id="humidity2" class="sensor-value">--</span>%</div>
                <div>Gas Level: <span id="gas" class="sensor-value">--</span></div>
                <div>Light Level: <span id="light" class="sensor-value">--</span></div>
            </div>

            <!-- Security & Motion -->
            <div class="card">
                <h3>🔒 Security & Motion</h3>
                <div><span id="motion-hall-indicator" class="status-indicator"></span>Hall Motion: <span id="motion-hall">--</span></div>
                <div><span id="motion-room2-indicator" class="status-indicator"></span>Room 2 Motion: <span id="motion-room2">--</span></div>
                <div><span id="door-indicator" class="status-indicator"></span>Door: <span id="door-status">--</span></div>
                <div>Distance: <span id="door-distance" class="sensor-value">--</span>cm</div>
            </div>

            <!-- Climate Control -->
            <div class="card">
                <h3>❄️ Climate Control</h3>
                <button class="control-btn" onclick="sendCommand('ac_on')">AC ON</button>
                <button class="control-btn" onclick="sendCommand('ac_off')">AC OFF</button>
                <br>
                <button class="control-btn" onclick="sendCommand('blinds_open')">Open Blinds</button>
                <button class="control-btn" onclick="sendCommand('blinds_close')">Close Blinds</button>
            </div>

            <!-- Lighting Control -->
            <div class="card">
                <h3>💡 RGB Lighting Control</h3>
                <div class="rgb-control">
                    <label>Red: <input type="range" id="red" min="0" max="255" value="0"></label>
                    <span id="red-val">0</span><br>
                    <label>Green: <input type="range" id="green" min="0" max="255" value="0"></label>
                    <span id="green-val">0</span><br>
                    <label>Blue: <input type="range" id="blue" min="0" max="255" value="0"></label>
                    <span id="blue-val">0</span><br>
                    <button class="control-btn" onclick="setRGB()">Apply RGB</button>
                </div>
            </div>

            <!-- System Status -->
            <div class="card">
                <h3>⚙️ System Status</h3>
                <div>WiFi Signal: <span id="wifi-signal" class="sensor-value">--</span> dBm</div>
                <div>Uptime: <span id="uptime" class="sensor-value">--</span> seconds</div>
                <div>Last Update: <span id="last-update">--</span></div>
                <br>
                <button class="control-btn emergency" onclick="emergencyStop()">🚨 EMERGENCY STOP</button>
            </div>
        </div>
    </div>

    <script>
        function updateData() {
            fetch('/api/data')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('temp1').textContent = data.temperature.room1.toFixed(1);
                    document.getElementById('temp2').textContent = data.temperature.room2.toFixed(1);
                    document.getElementById('humidity1').textContent = data.humidity.room1.toFixed(1);
                    document.getElementById('humidity2').textContent = data.humidity.room2.toFixed(1);
                    document.getElementById('gas').textContent = data.gas;
                    document.getElementById('light').textContent = data.light;
                    
                    // Motion indicators
                    updateIndicator('motion-hall-indicator', data.motion.hall);
                    updateIndicator('motion-room2-indicator', data.motion.room2);
                    updateIndicator('door-indicator', data.door.status === 'open');
                    
                    document.getElementById('motion-hall').textContent = data.motion.hall ? 'Detected' : 'Clear';
                    document.getElementById('motion-room2').textContent = data.motion.room2 ? 'Detected' : 'Clear';
                    document.getElementById('door-status').textContent = data.door.status;
                    document.getElementById('door-distance').textContent = data.door.distance.toFixed(1);
                    
                    document.getElementById('wifi-signal').textContent = data.wifi_signal;
                    document.getElementById('uptime').textContent = data.uptime;
                    document.getElementById('last-update').textContent = new Date().toLocaleTimeString();
                })
                .catch(error => console.error('Error fetching data:', error));
        }

        function updateIndicator(id, status) {
            const indicator = document.getElementById(id);
            indicator.className = 'status-indicator ' + (status ? 'status-on' : 'status-off');
        }

        function sendCommand(action) {
            fetch('/api/control', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: 'action=' + action
            })
            .then(response => response.text())
            .then(data => console.log('Command sent:', action, 'Response:', data))
            .catch(error => console.error('Error sending command:', error));
        }

        function setRGB() {
            const r = document.getElementById('red').value;
            const g = document.getElementById('green').value;
            const b = document.getElementById('blue').value;
            
            fetch('/api/control', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `action=rgb_set&r=${r}&g=${g}&b=${b}`
            })
            .then(response => response.text())
            .then(data => console.log('RGB set:', r, g, b))
            .catch(error => console.error('Error setting RGB:', error));
        }

        function emergencyStop() {
            if (confirm('Are you sure you want to activate emergency stop?')) {
                sendCommand('emergency_stop');
            }
        }

        // Update RGB value displays
        document.getElementById('red').oninput = function() {
            document.getElementById('red-val').textContent = this.value;
        }
        document.getElementById('green').oninput = function() {
            document.getElementById('green-val').textContent = this.value;
        }
        document.getElementById('blue').oninput = function() {
            document.getElementById('blue-val').textContent = this.value;
        }

        // Auto-refresh data every 2 seconds
        setInterval(updateData, 2000);        updateData(); // Initial load
    </script>
</body>
</html>
)HTML";
}

String SmartHomeWebServer::getSimplePage() {
  SensorData data = sensors->readAll();
  
  return R"HTML(
<!DOCTYPE html>
<html>
<head>
    <title>Smart Home v2.1 - Simple</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial; margin: 20px; background: #f0f0f0; }
        .container { max-width: 800px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; }
        .sensor-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; margin: 20px 0; }
        .sensor-card { background: #e8f4fd; padding: 15px; border-radius: 8px; text-align: center; }
        .sensor-value { font-size: 1.5em; font-weight: bold; color: #2c3e50; }
        .control-btn { background: #3498db; color: white; border: none; padding: 12px 24px; margin: 8px; border-radius: 6px; cursor: pointer; font-size: 16px; }
        .control-btn:hover { background: #2980b9; }
        .emergency { background: #e74c3c !important; }
        h1 { color: #2c3e50; text-align: center; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🏠 Smart Home v2.1 - Quick Access</h1>
        
        <div class="sensor-grid">
            <div class="sensor-card">
                <h3>🌡️ Room 1</h3>
                <div class="sensor-value">)HTML" + String(data.temp1, 1) + R"HTML(°C</div>
                <div>)HTML" + String(data.humidity1, 1) + R"HTML(%</div>
            </div>
            <div class="sensor-card">
                <h3>🌡️ Room 2</h3>
                <div class="sensor-value">)HTML" + String(data.temp2, 1) + R"HTML(°C</div>
                <div>)HTML" + String(data.humidity2, 1) + R"HTML(%</div>
            </div>
            <div class="sensor-card">
                <h3>💨 Gas Level</h3>
                <div class="sensor-value">)HTML" + String(data.gasValue) + R"HTML(</div>
            </div>
            <div class="sensor-card">
                <h3>💡 Light Level</h3>
                <div class="sensor-value">)HTML" + String(data.ldrValue) + R"HTML(</div>
            </div>
        </div>
        
        <div style="text-align: center; margin: 20px 0;">
            <button class="control-btn" onclick="fetch('/api/control', {method: 'POST', body: 'action=ac_on'})">AC ON</button>
            <button class="control-btn" onclick="fetch('/api/control', {method: 'POST', body: 'action=ac_off'})">AC OFF</button>
            <button class="control-btn" onclick="fetch('/api/control', {method: 'POST', body: 'action=blinds_open'})">Open Blinds</button>
            <button class="control-btn" onclick="fetch('/api/control', {method: 'POST', body: 'action=blinds_close'})">Close Blinds</button>
            <br><br>
            <button class="control-btn emergency" onclick="if(confirm('Emergency Stop?')) fetch('/api/control', {method: 'POST', body: 'action=emergency_stop'})">🚨 EMERGENCY STOP</button>
        </div>
        
        <div style="text-align: center; color: #7f8c8d; margin-top: 20px;">
            <small>For full dashboard with real-time updates, visit: <a href="/">Full Dashboard</a></small><br>
            <small>System Status: WiFi Connected | Uptime: )HTML" + String(millis()/1000) + R"HTML( seconds</small>
        </div>
    </div>
</body>
</html>
)HTML";
}

#endif // ENABLE_WEB_SERVER
