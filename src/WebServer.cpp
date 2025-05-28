#include "WebServer.h"
#include "pins.h"

SmartHomeWebServer::SmartHomeWebServer() : server(80) {}

void SmartHomeWebServer::begin(Sensors* sensorPtr, Actuators* actuatorPtr) {
  sensors = sensorPtr;
  actuators = actuatorPtr;
  
  // Set up routes
  server.on("/", [this]() { handleRoot(); });
  server.on("/api/data", [this]() { handleAPI(); });
  server.on("/api/control", HTTP_POST, [this]() { handleControl(); });
  server.onNotFound([this]() { handleNotFound(); });
  
  // Enable CORS
  server.enableCORS(true);
  
  server.begin();
  Serial.println("Web server started on port 80");
  Serial.print("Access dashboard at: http://");
  Serial.println(WiFi.localIP());
}

void SmartHomeWebServer::handleClient() {
  server.handleClient();
}

void SmartHomeWebServer::handleRoot() {
  server.send(200, "text/html", getHomePage());
}

void SmartHomeWebServer::handleAPI() {
  server.send(200, "application/json", getApiData());
}

void SmartHomeWebServer::handleControl() {
  if (server.hasArg("action")) {
    String action = server.arg("action");
    String response = "OK";
    
    if (action == "ac_on") {
      actuators->controlAC(true);
    } else if (action == "ac_off") {
      actuators->controlAC(false);
    } else if (action == "blinds_open") {
      actuators->controlBlinds(0);
    } else if (action == "blinds_close") {
      actuators->controlBlinds(90);
    } else if (action.startsWith("rgb_")) {
      // Parse RGB values
      int r = server.arg("r").toInt();
      int g = server.arg("g").toInt();
      int b = server.arg("b").toInt();
      actuators->rgbControl(r, g, b);
    } else {
      response = "Unknown action";
    }
    
    server.send(200, "text/plain", response);
  } else {
    server.send(400, "text/plain", "Missing action parameter");
  }
}

void SmartHomeWebServer::handleNotFound() {
  server.send(404, "text/plain", "Not found");
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
            <h1>🏠 Smart Home Dashboard v2.0</h1>
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
        setInterval(updateData, 2000);
        updateData(); // Initial load
    </script>
</body>
</html>
)HTML";
}

void SmartHomeWebServer::stop() {
  server.stop();
}
