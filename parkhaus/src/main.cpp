#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <Ultrasonic.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

#include "config.h"

IPAddress ip;

#define LED_PIN 2

Ultrasonic u1(22, 23);
Ultrasonic u2(24, 25);
Ultrasonic u3(26, 27);
Ultrasonic u4(28, 29);

uint32_t last_millis = 0;

bool p1 = false;
bool p2 = false;
bool p3 = false;
bool p4 = false;

void checkBays() {
  if (u1.read(CM) < 10) {
    p1 = true;
  } else {
    p1 = false;
  }

  if (u2.read(CM) < 10) {
    p2 = true;
  } else {
    p2 = false;
  }

  if (u3.read(CM) < 10) {
    p3 = true;
  } else {
    p3 = false;
  }

  if (u4.read(CM) < 10) {
    p4 = true;
  } else {
    p4 = false;
  }
}

Adafruit_NeoPixel strip(4, LED_PIN, NEO_GRB + NEO_KHZ800);

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

WiFiUDP udp;
Coap coap(udp);

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing");

  strip.begin();
  strip.show();
  strip.setBrightness(50);

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
}

void loop() {
  checkBays();
  setBayLEDs(p1, p2, p3, p4);

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
    coap.put(ip, coap_port, "boolean", payload.c_str(), payload.length());
  }

  coap.loop();  // Keep the CoAP client running
}