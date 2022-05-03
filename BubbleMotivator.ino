#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Stepper.h>
#include "config.h" // wlan ssid and password

ESP8266WebServer server(80);

#define dirPin 2
#define stepPin 3
#define stepsPerRevolution 200

bool spinning = false;
bool clockwise = true;

void startSpin() {
  spinning = true;
  Serial.println("Start Spinning");
  server.send(200, "text/plain", "started");
}

void stopSpin() {
  spinning = false;
  Serial.println("Stopped Spinning");
  server.send(200, "text/plain", "stopped");
}

void rev() {
  clockwise = !clockwise;
  Serial.println("Changed spinning direction to " + clockwise ? "clockwise" : "counterclockwise");
  server.send(200, "text/plain", clockwise ? "motor spinning clockwise" : "motor spinning counterclockwise");
}

void restServerRouting() {
  server.on(F("/start"), HTTP_POST, startSpin);
  server.on(F("/stop"), HTTP_POST, stopSpin);
  server.on(F("/reverse"), HTTP_POST, rev);
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
 
void setup(void) {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  
  Serial.begin(9600);
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
  
  restServerRouting();
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void) {
  server.handleClient();
  if(spinning) {
    digitalWrite(dirPin, clockwise ? HIGH : LOW);
    for (int i = 0; i < stepsPerRevolution; i++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(2000);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(2000);
    }
  }
}
