#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <ESP32Servo.h>
#include <Ultrasonic.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

#include "button/button.h"
#include "config.h"
#include "timer.h"

IPAddress ip;

Ultrasonic u1(US_1_TRIG, US_1_ECHO);
Ultrasonic u2(US_2_TRIG, US_2_ECHO);
Ultrasonic u3(US_3_TRIG, US_3_ECHO);
Ultrasonic u4(US_4_TRIG, US_4_ECHO);

Adafruit_NeoPixel strip(4, LED_PIN, NEO_GRB + NEO_KHZ800);

Servo myservo;

Timer myTimer(2000);
Timer usTimer(100);

WiFiUDP udp;
Coap coap(udp);

Button button(BTN_PIN, 1000);

uint32_t last_millis = 0;

bool p1 = false;
bool p2 = false;
bool p3 = false;
bool p4 = false;

void checkBays() {
  unsigned int t = 8;

  delay(10);
  unsigned int u1d = u1.read(CM);
  delay(10);
  unsigned int u2d = u2.read(CM);
  delay(10);
  unsigned int u3d = u3.read(CM);
  delay(10);
  unsigned int u4d = u4.read(CM);

  if (u1d <= t)
    p1 = true;
  else
    p1 = false;

  if (u2d <= t)
    p2 = true;
  else
    p2 = false;

  if (u3d <= t)
    p3 = true;
  else
    p3 = false;

  if (u4d <= t)
    p4 = true;
  else
    p4 = false;
}

void setBayLEDs(bool b1, bool b2, bool b3, bool b4) {
  static uint32_t used = strip.Color(255, 0, 0);
  static uint32_t free = strip.Color(0, 255, 0);

  if (b1) {
    strip.setPixelColor(0, used);
  } else {
    strip.setPixelColor(0, free);
  }

  if (b2) {
    strip.setPixelColor(1, used);
  } else {
    strip.setPixelColor(1, free);
  }

  if (b3) {
    strip.setPixelColor(2, used);
  } else {
    strip.setPixelColor(2, free);
  }

  if (b4) {
    strip.setPixelColor(3, used);
  } else {
    strip.setPixelColor(3, free);
  }

  strip.show();
}

void displayArrow(int dir) {
  digitalWrite(AL_PIN, dir & 0b01);
  digitalWrite(AR_PIN, dir & 0b10);
}

void openGate() { myservo.write(80); }

void closeGate() { myservo.write(170); }

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing");

  pinMode(AL_PIN, OUTPUT);
  pinMode(AR_PIN, OUTPUT);

  displayArrow(0b11);

  myservo.attach(SERVO_PIN);
  closeGate();

  strip.begin();
  strip.show();
  strip.setBrightness(50);
  usTimer.start();

#ifndef TESTING
  // Try and wait until a connection to WiFi was made
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println("My IP: ");
  Serial.println(WiFi.localIP());

  // Resolve the IP address of the server using DNS
  IPAddress ip;
  if (WiFi.hostByName(coap_server_str, ip)) {
    Serial.print("Resolved IP for ");
    Serial.print(coap_server_str);
    Serial.print(": ");
    Serial.println(ip);
  } else {
    Serial.println("DNS resolution failed!");
    return;
  }

  // Connect CoAP client to a server
  coap.start();
#endif
}

void loop() {
  if (usTimer.hasElapsed()) {
    checkBays();
    usTimer.start();
  }
  int left = ((int)!p3) + ((int)!p4);
  int right = ((int)!p1) + ((int)!p2);
  setBayLEDs(p1, p2, p3, p4);

  if (left + right == 0) {
    displayArrow(0b11);
  } else if (!myTimer.isRunning()) {
    displayArrow(0b00);
  }

  if (button.isShortPress()) {
    myTimer.start();
    if (left > right) {
      displayArrow(0b10);
    } else if (right > left) {
      displayArrow(0b01);
    } else if (left == 0 && right == 0) {
      displayArrow(0b11);
    } else {
      displayArrow(0b01);
    }

    if (left + right != 0) {
      openGate();
    }
  }

  if (myTimer.hasElapsed()) {
    closeGate();
    displayArrow(0b00);
  }

#ifndef TESTING
  if (millis() - last_millis > 1000) {
    last_millis = millis();
    // Construct payload as a JSON-like string
    String payload = "{";
    payload += "\"p1\":" + String(p1 ? "true" : "false") + ",";
    payload += "\"p2\":" + String(p2 ? "true" : "false") + ",";
    payload += "\"p3\":" + String(p3 ? "true" : "false") + ",";
    payload += "\"p4\":" + String(p4 ? "true" : "false");
    payload += "}";

    // Send data to the CoAP server
    // coap.put(ip, coap_port, "boolean", payload.c_str(), payload.length());
  }

  coap.loop();  // Keep the CoAP client running
#endif

  button.handle();
}