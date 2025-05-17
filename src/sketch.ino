#include <DHT.h>
#include <ESP32Servo.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WebServer.h>

#include <PubSubClient.h>

WebServer server(8185);




// WiFi & MQTT Credentials
//const char* ssid = "Galaxy A51 6C73";
const char* ssid = "Wokwi-GUEST"; // Your WiFi SSID
const char* password = "";
const char* mqttServer = "broker.hivemq.com"; // Public MQTT broker
//const char* mqttServer = "192.168.167.23";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// MQTT Topics
#define TEMP_TOPIC "smart_home/temperature"
#define GAS_TOPIC "b/smart_home/esp32/gas "
#define RELAY_TOPIC "c/smart_home/esp32/relay1 "
// LCD2 (I2C)
LiquidCrystal_I2C lcd2(0x27, 16, 2); // SDA=21, SCL=22
// DHT22 Sensors
#define DHT1_PIN 4
#define DHT2_PIN 16
DHT dht1(DHT1_PIN, DHT22);
DHT dht2(DHT2_PIN, DHT22);

// PIR Motion Sensors
#define PIR1_PIN 5     // Hall
#define PIR2_PIN 17    // Room 2

// Gas Sensor (MQ-2)
#define GAS_PIN 36     // Analog VP pin

// LDR (Light Sensor)
#define LDR_PIN 35     // Analog VN pin

// Ultrasonic Sensor (Door)
#define TRIG_PIN 18
#define ECHO_PIN 19

// Actuators
#define RELAY1_PIN 27  // Exhaust Fan (Kitchen)
#define RELAY2_PIN 14  // AC (Room 1)
#define SERVO_PIN 12   // Blinds
#define BUZZER_PIN 23  // Buzzer

// RGB LED
#define RGB_R 13
#define RGB_G 26
#define RGB_B 32

// Buttons
#define BTN1_PIN 34    // Manual Override 1
#define BTN2_PIN 33    // Manual Override 2


// Servo
Servo blindsServo;

// Thresholds
#define GAS_THRESHOLD 500   // Adjust based on simulation
#define TEMP_THRESHOLD 25   // 25°C

////
// MQTT Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println("Message arrived [" + String(topic) + "]: " + message);
}

// MQTT connection function
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Subscribe to topics
      client.subscribe(TEMP_TOPIC);
      
      // Publish connection message
      client.publish("esp32/status", "connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Simple WiFi Connection for Wokwi
  Serial.println("\nConnecting to Wokwi-GUEST");
  WiFi.begin(ssid, password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // MQTT Setup
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  Serial.println("\nConnecting to WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }
  
  // MQTT Setup
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  Serial.println("Hello, ESP32!");
 
 
  // Initialize DHT sensors
  dht1.begin();
  dht2.begin();

  // Initialize PIR sensors
  pinMode(PIR1_PIN, INPUT);
  pinMode(PIR2_PIN, INPUT);

  // Initialize Actuators
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  blindsServo.attach(SERVO_PIN);

  // RGB LED
  // pinMode(RGB_R, OUTPUT);
  // pinMode(RGB_G, OUTPUT);
  // pinMode(RGB_B, OUTPUT);
  ledcSetup(0, 5000, 8);  // Channel 0 for Red
  ledcAttachPin(RGB_R, 0);
  ledcSetup(1, 5000, 8);  // Channel 1 for Green
  ledcAttachPin(RGB_G, 1);

  // Buttons
  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);

  // Initialize LCD2
  lcd2.init();
  lcd2.backlight();
  lcd2.print("Smart Home Ready");

  // Ultrasonic Sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost!");
    WiFi.reconnect();
    delay(5000);
    return;
  }

  // Check MQTT connection
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read Sensors
  float temp1 = dht1.readTemperature();
  float temp2 = dht2.readTemperature();
  int gasValue = analogRead(GAS_PIN);
  int ldrValue = analogRead(LDR_PIN);
  bool motionHall = digitalRead(PIR1_PIN);
  bool motionRoom2 = digitalRead(PIR2_PIN);

  // Ultrasonic Distance Measurement
  long duration, distance;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  // Control Logic
  // Gas Leak Detection
  if (gasValue > GAS_THRESHOLD) {
    lcd2.setCursor(0, 0);
    lcd2.print("Gas Leak Detected!");
  } else {
    lcd2.setCursor(0, 0);
    lcd2.print("No Gas Leakage");
  }

  // 2. Room Temperature Control
  if (temp1 > TEMP_THRESHOLD) {
    digitalWrite(RELAY2_PIN, HIGH); // Turn on AC
  } else {
    digitalWrite(RELAY2_PIN, LOW);
  }



  // 3. Automated Lighting (Hall)
  if (motionHall || ldrValue < 500) { // Motion OR low light
    //analogWrite(RGB_R, 255);
    ledcWrite(0, 255); // Red ON          // Turn on RGB LED (Red)
  } else {
    //analogWrite(RGB_R, 0);
    ledcWrite(1, 0);   // Green OFF
  }

  // 4. Servo Blinds Control (Based on LDR)
  if (ldrValue > 1000) {             // Bright light
    blindsServo.write(90);           // Close blinds
  } else {
    blindsServo.write(0);            // Open blinds
  }

  // 5. Door Status (Ultrasonic)
  if (distance > 50) {
    lcd2.setCursor(0, 1);
    lcd2.print("Door Open!, something went wrong");
  } else {
    lcd2.setCursor(0, 1);
    lcd2.print("Door Close!, No Worries");
  }

    // Publish sensor data to MQTT
  client.publish(TEMP_TOPIC, String(temp1).c_str());
  client.publish(GAS_TOPIC, String(gasValue).c_str());
  delay(1000); // Slow down the loop for stability
}
