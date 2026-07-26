#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =========================
// Wi-Fi Configuration
// =========================

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// =========================
// Pin Configuration
// =========================

#define TRIG_PIN 5
#define ECHO_PIN 18
#define BUZZER_PIN 27

#define OLED_SDA 21
#define OLED_SCL 22

// =========================
// OLED Configuration
// =========================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// =========================
// Web Server
// =========================

WebServer server(80);

// =========================
// Bin Configuration
// =========================

// Change this according to your actual bin.
const float BIN_HEIGHT_CM = 30.0;

// Alert when waste reaches 80%
const int FULL_THRESHOLD = 80;

// =========================
// Variables
// =========================

float distanceCM = 0;
int wasteLevel = 0;

String binStatus = "EMPTY";

// =========================
// Read Distance
// =========================

float readDistance() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return -1;
  }

  float distance = duration * 0.0343 / 2.0;

  return distance;
}

// =========================
// Calculate Waste Level
// =========================

int calculateWasteLevel(float distance) {

  if (distance < 0) {
    return 0;
  }

  float level =
    ((BIN_HEIGHT_CM - distance) / BIN_HEIGHT_CM) * 100.0;

  int percentage = (int)level;

  percentage = constrain(percentage, 0, 100);

  return percentage;
}

// =========================
// Determine Bin Status
// =========================

String getBinStatus(int level) {

  if (level >= FULL_THRESHOLD) {
    return "FULL";
  }

  if (level >= 50) {
    return "HALF FULL";
  }

  if (level >= 20) {
    return "FILLING";
  }

  return "EMPTY";
}

// =========================
// Update OLED
// =========================

void updateOLED() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);

  display.println("SMART WASTE BIN");

  display.drawLine(
    0, 11,
    127, 11,
    SSD1306_WHITE
  );

  display.setTextSize(2);
  display.setCursor(0, 18);

  display.print(wasteLevel);
  display.println("%");

  display.setTextSize(1);
  display.setCursor(0, 43);

  display.print("Status: ");
  display.println(binStatus);

  display.display();
}

// =========================
// Buzzer Alert
// =========================

void updateBuzzer() {

  if (wasteLevel >= FULL_THRESHOLD) {

    digitalWrite(BUZZER_PIN, HIGH);

  } else {

    digitalWrite(BUZZER_PIN, LOW);
  }
}

// =========================
// Web Page
// =========================

String createWebPage() {

  String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>

<meta name="viewport"
content="width=device-width, initial-scale=1">

<meta http-equiv="refresh" content="3">

<title>Smart Waste Management</title>

<style>

body {
  font-family: Arial, sans-serif;
  text-align: center;
  background: #f2f2f2;
  margin: 0;
  padding: 20px;
}

.container {
  max-width: 500px;
  margin: auto;
}

h1 {
  font-size: 28px;
}

.card {
  background: white;
  padding: 25px;
  margin-top: 20px;
  border-radius: 15px;
  box-shadow: 0 4px 12px rgba(0,0,0,0.15);
}

.level {
  font-size: 55px;
  font-weight: bold;
  margin: 10px;
}

.status {
  font-size: 24px;
  margin: 15px;
}

.bar {
  width: 100%;
  height: 30px;
  background: #ddd;
  border-radius: 15px;
  overflow: hidden;
}

.fill {
  height: 100%;
  width: )rawliteral";

  page += String(wasteLevel);

  page += R"rawliteral(%;
  background: #333;
}

.info {
  margin-top: 20px;
  font-size: 17px;
}

</style>

</head>

<body>

<div class="container">

<h1>Smart Waste Management</h1>

<div class="card">

<div class="level">)rawliteral";

  page += String(wasteLevel);

  page += R"rawliteral(%</div>

<div class="bar">
<div class="fill"></div>
</div>

<div class="status">
Status: )rawliteral";

  page += binStatus;

  page += R"rawliteral(
</div>

<div class="info">
Distance: )rawliteral";

  page += String(distanceCM, 1);

  page += R"rawliteral( cm
</div>

<div class="info">
Alert Threshold: 80%
</div>

</div>

<p>ESP32 IoT Smart Bin</p>

</div>

</body>
</html>
)rawliteral";

  return page;
}

// =========================
// Web Server Handler
// =========================

void handleRoot() {

  server.send(
    200,
    "text/html",
    createWebPage()
  );
}

// =========================
// Setup
// =========================

void setup() {

  Serial.begin(115200);

  // Pin setup
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);

  // OLED setup
  Wire.begin(
    OLED_SDA,
    OLED_SCL
  );

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C
      )) {

    Serial.println("OLED initialization failed!");

    while (true) {
      delay(1000);
    }
  }

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(0, 0);

  display.println("SMART WASTE");

  display.println("MANAGEMENT");

  display.println();

  display.println("Starting...");

  display.display();

  delay(2000);

  // Wi-Fi
  WiFi.begin(
    ssid,
    password
  );

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println("Wi-Fi connected!");

  Serial.print("IP Address: ");

  Serial.println(
    WiFi.localIP()
  );

  // Web server
  server.on(
    "/",
    handleRoot
  );

  server.begin();

  Serial.println(
    "Web server started!"
  );
}

// =========================
// Main Loop
// =========================

void loop() {

  server.handleClient();

  distanceCM = readDistance();

  wasteLevel =
    calculateWasteLevel(
      distanceCM
    );

  binStatus =
    getBinStatus(
      wasteLevel
    );

  updateOLED();

  updateBuzzer();

  Serial.print("Distance: ");

  Serial.print(distanceCM);

  Serial.print(" cm | Waste: ");

  Serial.print(wasteLevel);

  Serial.print("% | Status: ");

  Serial.println(binStatus);

  delay(1000);
}
