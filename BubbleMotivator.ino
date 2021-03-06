#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Servo.h>
#include "config.h" // wlan ssid and password

ESP8266WebServer server(80);
Servo servo;

#define dirPin 2
#define stepPin 0
#define stepsPerSpin 200

const uint8_t servoPin = D2;
int delayBetweenSteps = 3000;
int servoSpeed = 15;
bool spinning = false;
bool clockwise = true;

void startSpin() {
  spinning = true; //stepper
  servo.write(servoSpeed); // servo
  Serial.println("Start Spinning");
  server.send(200, "text/plain", "started");
}

void stopSpin() {
  spinning = false;  //stepper
  servo.write(0); // servo
  Serial.println("Stopped Spinning");
  server.send(200, "text/plain", "stopped");
}

void rev() {
  clockwise = !clockwise;
  Serial.println("Changed spinning direction to " + clockwise ? "clockwise" : "counterclockwise");
  server.send(200, "text/plain", !clockwise ? "motor spinning clockwise" : "motor spinning counterclockwise");
}

void setDelay() {
  String body;
  
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Body not received");
    return;
  }

  body = server.arg("plain");

  bool isnum = true;
  for(byte i = 0; i < body.length() && isnum; i++) {
    if(!isDigit(body.charAt(i))) {
      isnum = false;
    }
  }

  if(!isnum) {
    server.send(400, "text/plain", "Body ist not a digit");
    return;
  }
  
  delayBetweenSteps = server.arg("plain").toInt();
  if(delayBetweenSteps < 400) {
    delayBetweenSteps = 400; // safety
  }
  server.send(200, "text/plain", "Changed Stepper speed");
}

void setServoSpeed() {
  String body;
  
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Body not received");
    return;
  }

  body = server.arg("plain");

  bool isnum = true;
  for(byte i = 0; i < body.length() && isnum; i++) {
    if(!isDigit(body.charAt(i))) {
      isnum = false;
    }
  }

  if(!isnum) {
    server.send(400, "text/plain", "Body ist not a digit");
    return;
  }
  
  servoSpeed = server.arg("plain").toInt();
  if(servoSpeed > 180) {
    servoSpeed = 180;
  }else if(servoSpeed < 0) {
    servoSpeed = 0;
  }
  server.send(200, "text/plain", "Changed Servo speed");

  if(spinning) {
    servo.write(servoSpeed);
  }
}
 
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setupStepper() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  digitalWrite(stepPin, LOW); // Currently no stepper motor movement
  digitalWrite(dirPin, LOW);
  Serial.println("Stepper initialized");
}

void setupServo() {
  servo.attach(servoPin);
  for(int pos = 0; pos <= 180; pos += 30)
  {
    servo.write(pos);
    Serial.println(pos);
    delay(15);
  }
  delay(2500);
  for(int pos = 180; pos >= 0; pos -= 30)
  {
    servo.write(pos);
    Serial.println(pos);
    delay(15);
  }
  delay(2500);
  servo.write(0);

  Serial.println("Servo initialized");
}

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(100);
  Serial.print("Connecting wlan");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
}

void setupHttpServer() {
  server.on(F("/start"), HTTP_POST, startSpin);
  server.on(F("/stop"), HTTP_POST, stopSpin);
  server.on(F("/reverse"), HTTP_POST, rev);
  server.on(F("/setDelay"), HTTP_POST, setDelay);
  server.on(F("/setServoSpeed"), HTTP_POST, setServoSpeed);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}
 
void setup(void) {
  Serial.begin(9600);
  
  setupServo();
  setupStepper();
  setupWifi();
  setupHttpServer();
}
 
void loop(void) {
  server.handleClient();

  if(spinning) {
    digitalWrite(dirPin, clockwise ? HIGH : LOW);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayBetweenSteps);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayBetweenSteps);
  }
}
