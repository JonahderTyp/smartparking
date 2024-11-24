
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <coap-simple.h>

#include "config.h"
#include "timer.h"

IPAddress ip;

Timer lcdTimer(200);

WiFiUDP udp;
Coap coap(udp);
LiquidCrystal_I2C lcd(0x27, 16, 2);

uint32_t last_millis = 0;

int available_bays = 0;
int last_available_bays = -1;

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing");

  Wire.begin(33, 32);

  lcdTimer.start();

  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Warte auf Server...");

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
  if (last_available_bays != available_bays) {
    last_available_bays = available_bays;
    lcdTimer.start();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Gleis 13:");
    lcd.setCursor(0, 1);
    lcd.print(available_bays);
    lcd.setCursor(4, 1);
    lcd.print("Frei");
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
}