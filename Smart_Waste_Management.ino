#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//==================== WIFI ====================
const char* ssid = "J/20073-2.4G";
const char* password = "qwerty12";

//==================== PIN DEFINITIONS ====================
#define TRIG_PIN 5
#define ECHO_PIN 18
#define BUZZER_PIN 27

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//==================== OLED ====================
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//==================== WEB SERVER ====================
WebServer server(80);

//==================== BIN SETTINGS ====================
// Distance from sensor to opposite wall when bin is empty
const float BIN_HEIGHT = 31.0;

//==================== VARIABLES ====================
float distance = BIN_HEIGHT;
float lastDistance = BIN_HEIGHT;

int wastePercent = 0;
String statusText = "EMPTY";

float readDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0)
    return -1;

  float d = duration * 0.0343 / 2.0;

  // Ignore invalid values
  if (d < 2 || d > 40)
    return -1;

  return d;
}

void updateOLED()
{
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("SMART WASTE BIN");

  display.drawLine(0,10,127,10,SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0,18);
  display.print(wastePercent);
  display.print("%");

  display.setTextSize(1);
  display.setCursor(0,45);
  display.print("Dist: ");
  display.print(distance,1);
  display.println(" cm");

  display.print("Status: ");
  display.println(statusText);

  display.display();
}
//==================== WEB PAGE ====================
String webpage()
{
  String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="refresh" content="2">
<meta name="viewport" content="width=device-width, initial-scale=1">

<style>
body{
font-family:Arial;
background:#f2f2f2;
text-align:center;
}

.card{
background:white;
width:330px;
margin:auto;
margin-top:30px;
padding:20px;
border-radius:15px;
box-shadow:0px 0px 10px gray;
}

.bar{
width:100%;
height:25px;
background:#ddd;
border-radius:20px;
overflow:hidden;
}

.fill{
height:25px;
background:#28a745;
}
</style>

</head>

<body>

<div class="card">

<h2>Smart Waste Bin</h2>

<h1>)rawliteral";

  page += String(wastePercent);

  page += R"rawliteral(%</h1>

<div class="bar">
<div class="fill" style="width:)rawliteral";

  page += String(wastePercent);

  page += R"rawliteral(%;"></div>
</div>

<br>

<h3>Status : )rawliteral";

  page += statusText;

  page += R"rawliteral(</h3>

<p>Distance : )rawliteral";

  page += String(distance,1);

  page += R"rawliteral( cm</p>

</div>

</body>
</html>
)rawliteral";

  return page;
}

void handleRoot()
{
  server.send(200, "text/html", webpage());
}
void setup()
{
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);

  // Initialize I2C
  Wire.begin(21, 22);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED initialization failed!");
    while (true);
  }

  // Startup Screen
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.println("SMART");
  display.setCursor(20, 45);
  display.println("BIN");
  display.display();

  delay(2000);

  // Connect Wi-Fi
  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();

  Serial.println("Web Server Started");
}
void loop()
{
  server.handleClient();

  // Read sensor
  float newDistance = readDistance();

  // Keep last valid reading
  if (newDistance != -1)
  {
    lastDistance = (lastDistance * 0.8) + (newDistance * 0.2);
  }

  distance = lastDistance;

  // Calculate waste percentage
  wastePercent = (int)(((BIN_HEIGHT - distance) * 100.0) / BIN_HEIGHT);
  wastePercent = constrain(wastePercent, 0, 100);

  // Determine status
  if (wastePercent >= 80)
  {
    statusText = "FULL";
    digitalWrite(BUZZER_PIN, HIGH);
  }
  else if (wastePercent >= 50)
  {
    statusText = "HALF FULL";
    digitalWrite(BUZZER_PIN, LOW);
  }
  else if (wastePercent >= 20)
  {
    statusText = "FILLING";
    digitalWrite(BUZZER_PIN, LOW);
  }
  else
  {
    statusText = "EMPTY";
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Update OLED
  updateOLED();

  // Debug output
  Serial.print("Distance: ");
  Serial.print(distance, 1);
  Serial.print(" cm");

  Serial.print(" | Waste: ");
  Serial.print(wastePercent);
  Serial.print("%");

  Serial.print(" | Status: ");
  Serial.println(statusText);

  delay(500);
}
