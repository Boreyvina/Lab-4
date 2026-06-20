#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

Servo myServo;
const int servoPin = 17;  
bool autoMode = false; 

int speed = 50;            
int servoAngle = 90;      

const char* ssid = "Robotic WIFI";
const char* password = "rbtWIFI@2025";

WebServer server(80);

int channel0 = 4;
int channel1 = 5;
int channel2 = 6;
int channel3 = 7;

const int ledPin = 12; // LED pin
#define TRIG_PIN 13
#define ECHO_PIN 39
float distance;
int readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    Serial.println("No echo");
    return 999; 
  } else {
    distance = (duration * 0.0343) / 2.0;
    return (int)distance; 
  }
}

void move_backward(int speed){

  ledcWrite(channel0, speed);
  ledcWrite(channel1, speed);
  ledcWrite(channel2, speed);
  ledcWrite(channel3, speed);

  //M1
  digitalWrite(26, LOW);
  digitalWrite(25, HIGH);

  //M2
  digitalWrite(27, LOW);
  digitalWrite(32, HIGH);

  //M3
  digitalWrite(18, LOW);
  digitalWrite(21, HIGH);

  //M4
  digitalWrite(22, LOW);
  digitalWrite(23, HIGH);
}

void move_forward(int speed){
  ledcWrite(channel0, speed);
  ledcWrite(channel1, speed);
  ledcWrite(channel2, speed);
  ledcWrite(channel3, speed);

  //M1
  digitalWrite(26, HIGH);
  digitalWrite(25, LOW);

  //M2
  digitalWrite(27, HIGH);
  digitalWrite(32, LOW);

  //M3
  digitalWrite(18, HIGH);
  digitalWrite(21, LOW);

  //M4
  digitalWrite(22, HIGH);
  digitalWrite(23, LOW);
}

void move_left(int speed){
  ledcWrite(channel0, speed);
  ledcWrite(channel1, speed);
  ledcWrite(channel2, 0);
  ledcWrite(channel3, 0);
  //M1
  digitalWrite(26, HIGH);
  digitalWrite(25, LOW);

  //M2
  digitalWrite(27, HIGH);
  digitalWrite(32, LOW);

  //M3
  digitalWrite(18, HIGH);
  digitalWrite(21, LOW);

  //M4
  digitalWrite(22, HIGH);
  digitalWrite(23, LOW);
}

void move_right(int speed){
  ledcWrite(channel0, 0);
  ledcWrite(channel1, 0);
  ledcWrite(channel2, speed);
  ledcWrite(channel3, speed);
  //M1
  digitalWrite(26, HIGH);
  digitalWrite(25, LOW);

  //M2
  digitalWrite(27, HIGH);
  digitalWrite(32, LOW);

  //M3
  digitalWrite(18, HIGH);
  digitalWrite(21, LOW);

  //M4
  digitalWrite(22, HIGH);
  digitalWrite(23, LOW);
}

void stop(){
  ledcWrite(channel0, 0);
  ledcWrite(channel1, 0);
  ledcWrite(channel2, 0);
  ledcWrite(channel3, 0);
}

void handleForward() {
  if (!autoMode) {
    move_forward(speed);
  }
  handleRoot();
}

void handleBackward() {
  if (!autoMode) {
    move_backward(speed);
  }
  handleRoot();
}

void handleLeft() {
  if (!autoMode) {
    move_left(speed);
  }
  handleRoot();
}

void handleRight() {
  if (!autoMode) {
    move_right(speed);
  }
  handleRoot();
}

void handleStop() {
  if (!autoMode) {
    stop();
  }
  handleRoot();
}

void handleSpeed() {

    if (server.hasArg("value")) {
        speed = server.arg("value").toInt();

        if (speed < 0) speed = 0;
        if (speed > 100) speed = 100;
    }

    handleRoot();
}

void handleServo() {

    if (server.hasArg("angle")) {

        servoAngle = server.arg("angle").toInt();

        if (servoAngle < 0)
            servoAngle = 0;

        if (servoAngle > 180)
            servoAngle = 180;

        myServo.write(servoAngle);
        Serial.println(servoAngle);
    }

    handleRoot();
}

void automate(){
  int currentDistance = readDistance();
  Serial.print("Distance: ");
  Serial.println(currentDistance);

  // handle bad readings safely
  if (currentDistance == 0 || currentDistance == 999) {
    move_forward(speed);
    return;
  }

  if (currentDistance >= 30) {
    move_forward(speed);
  } 
  else {
    stop();
    delay(200);

    Serial.println("Obstacle detected! Turning right...");
    move_right(speed);
    delay(3800);   // adjust for ~90 degree turn
    stop();
    delay(300);

    // re-check after turning (only once, no while loop)
    int checkDist = readDistance();

    if (checkDist < 30) {
      Serial.println("Still blocked, turning left slightly...");
      move_right(speed);
      delay(300);
      stop();
      delay(200);
    }

    // ALWAYS go forward again after decision
    move_forward(speed);
  }
}

void handleManualMode() {
    autoMode = false;
    stop();
    handleRoot();
}

void handleAutoMode() {
    autoMode = true;
    speed = 40;          // Auto mode always runs at speed 50
    Serial.println("Mode: AUTO");
    handleRoot();
}

void handleRoot() {

String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">

<style>

body{
    font-family:Arial;
    text-align:center;
    background:#f4f4f4;
    margin-top:30px;
}

h1{
    color:#333;
}

button{
    width:120px;
    height:60px;
    font-size:18px;
    margin:8px;
    border:none;
    border-radius:8px;
    cursor:pointer;
}

button:hover{
    opacity:0.85;
}

.stop{
    background:red;
    color:white;
}

.forward,
.backward,
.left,
.right{
    background:#4CAF50;
    color:white;
}

.manual{
    background:#28a745;
    color:white;
}

.auto{
    background:#ff9800;
    color:white;
}

.slider{
    width:300px;
}

.card{
    background:white;
    width:380px;
    margin:auto;
    padding:20px;
    border-radius:10px;
    box-shadow:0px 0px 10px rgba(0,0,0,.2);
}

</style>

</head>

<body>

<div class="card">

<h1>ESP32 Robot Control</h1>

<h3>Mode Selection</h3>

<p>
    <a href="/manual">
        <button class="manual">Manual Mode</button>
    </a>

    <a href="/auto">
        <button class="auto">Auto Mode</button>
    </a>
</p>

<p>
    <strong>Current Mode: %MODE%</strong>
</p>

<hr>

<h3>Speed</h3>

<input
type="range"
class="slider"
min="0"
max="255"
value="%SPEED%"
oninput="document.getElementById('speedValue').innerHTML=this.value;
fetch('/speed?value='+this.value);">

<p>
Current Speed :
<span id="speedValue">%SPEED%</span>
</p>

<hr>

<h3>Servo Control</h3>

<input
type="range"
class="slider"
min="0"
max="180"
value="%SERVO%"
oninput="document.getElementById('servoValue').innerHTML=this.value;
fetch('/servo?angle='+this.value);">

<p>
Servo Angle :
<span id="servoValue">%SERVO%</span>°
</p>

<hr>

<p>
<a href="/forward">
<button class="forward">Forward</button>
</a>
</p>

<p>

<a href="/left">
<button class="left">Left</button>
</a>

<a href="/stop">
<button class="stop">Stop</button>
</a>

<a href="/right">
<button class="right">Right</button>
</a>

</p>

<p>

<a href="/backward">
<button class="backward">Backward</button>
</a>

</p>

</div>

</body>
</html>
)rawliteral";

html.replace("%MODE%", autoMode ? "AUTO" : "MANUAL");
html.replace("%SPEED%", String(speed));
html.replace("%SERVO%", String(servoAngle));

server.send(200, "text/html", html);
}

// -------- LED ON --------
void handleOn() {
  digitalWrite(ledPin, HIGH);
  handleRoot();   // Refresh page
}

// -------- LED OFF --------
void handleOff() {
  digitalWrite(ledPin, LOW);
  handleRoot();   // Refresh page
}

void setup() {
  Serial.begin(115200);
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  ledcSetup(channel0, 20000, 8);
  ledcAttachPin(33, channel0);

  pinMode(32, OUTPUT);
  pinMode(27, OUTPUT);
  ledcSetup(channel1, 20000, 8);
  ledcAttachPin(14, channel1);

  pinMode(21, OUTPUT);
  pinMode(18, OUTPUT);
  ledcSetup(channel2, 20000, 8);
  ledcAttachPin(05, channel2);

  pinMode(23, OUTPUT);
  pinMode(22, OUTPUT);
  ledcSetup(channel3, 20000, 8);
  ledcAttachPin(19, channel3);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  //servo 
  myServo.attach(17);
  myServo.write(90);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  server.on("/speed", handleSpeed);
  server.on("/servo", handleServo);
  server.on("/", handleRoot);
  server.on("/manual", handleManualMode);
  server.on("/auto", handleAutoMode);

  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.on("/stop", handleStop);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Routes
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);

  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
    if (autoMode) {
    automate();
  }
}