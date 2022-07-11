#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <Servo.h>
#include "config.h" // wlan ssid and password

Servo servo;

#define dirPin 2
#define stepPin 0
#define stepsPerSpin 200

const char* MQTT_BROKER = "broker.hivemq.com";
WiFiClient espClient;
PubSubClient client(espClient);

const uint8_t servoPin = D2;
int delayBetweenSteps = 3000;
int servoSpeed = 15;
bool spinning = false;
bool clockwise = false;
char* topic = "this/is/my/bubble/machine/topic";

void startSpin() {
  spinning = true; //stepper
  servo.write(servoSpeed); // servo
  Serial.println("Start Spinning");
}

void stopSpin() {
  spinning = false;  //stepper
  servo.write(0); // servo
  Serial.println("Stopped Spinning");
}

void rev() {
  clockwise = !clockwise;
  Serial.println("Changed spinning direction");
}

void setDelay(String body) {
  bool isnum = true;
  for(byte i = 0; i < body.length() && isnum; i++) {
    if(!isDigit(body.charAt(i))) {
      isnum = false;
    }
  }

  if(!isnum) {
    return;
  }
  
  delayBetweenSteps = body.toInt();
  if(delayBetweenSteps < 400) {
    delayBetweenSteps = 400; // safety
  }
  Serial.println("Set Stepper speed");
}

void setServoSpeed(String body) {
  bool isnum = true;
  for(byte i = 0; i < body.length() && isnum; i++) {
    if(!isDigit(body.charAt(i))) {
      isnum = false;
    }
  }

  if(!isnum) {
    return;
  }
  
  servoSpeed = body.toInt();
  if(servoSpeed > 180) {
    servoSpeed = 180;
  }else if(servoSpeed < 0) {
    servoSpeed = 0;
  }

  if(spinning) {
    servo.write(servoSpeed);
  }
  Serial.println("Set Servo speed");
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
}

void callback(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (byte i = 0; i < length; i++) {
        char tmp = char(payload[i]);
        msg += tmp;
    }

    if(msg.startsWith("start")) {
      startSpin();
    } else if (msg.startsWith("stop")) {
      stopSpin();
    } else if (msg.startsWith("reverse")) {
      rev();
    } else if (msg.startsWith("setDelay")) {
      setDelay(msg.substring(msg.indexOf(' ') + 1));
    } else if (msg.startsWith("setServoSpeed")) {
      setServoSpeed(msg.substring(msg.indexOf(' ') + 1));
    }
    Serial.println(msg);
}

void setupMessageQueue() {
  client.setServer(MQTT_BROKER, 1883);
  client.setCallback(callback);
}
 
void setup(void) {
  Serial.begin(9600);
  
  setupServo();
  setupStepper();
  setupWifi();
  setupMessageQueue();
}
 
void loop(void) {
  if(!client.connected()) {
    reconnect();
  }
  client.loop();
  
  //server.handleClient();

  if(spinning) {
    digitalWrite(dirPin, clockwise ? HIGH : LOW);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayBetweenSteps);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayBetweenSteps);
  }
}

void reconnect() {
  while(!client.connected()) {
    Serial.println("Reconnecting MQTT...");
    if(!client.connect("ESP8266ClientBubbleMachine")) {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      Serial.println("retrying in 3 seconds");
      delay(3000);
    } else {
      Serial.print("success, rc=");
      Serial.println(client.state());
      client.subscribe(topic);
      Serial.println("Subscribed to topic");
    }
  }
}
