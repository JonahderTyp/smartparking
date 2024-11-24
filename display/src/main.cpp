
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
Timer coapTimer(3000);

WiFiUDP udp;
Coap coap(udp);
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define TOTAL_BAYS 4
int available_bays = -1;
int last_available_bays = -1;

bool value1 = false;
bool value2 = false;
bool value3 = false;
bool value4 = false;

void handleCoapResponse(CoapPacket &packet, IPAddress ip, int port) {
  String payload;
  for (int i = 0; i < packet.payloadlen; i++) {
    payload += (char)packet.payload[i];
  }
  Serial.println("Response from server:");
  Serial.println(payload);

  // Parse the response into variables
  int startIndex = 0;
  int endIndex = 0;

  // Parse value1
  startIndex = payload.indexOf("value1=") + 7;
  endIndex = payload.indexOf(",", startIndex);
  value1 = (payload.substring(startIndex, endIndex) == "True");

  // Parse value2
  startIndex = payload.indexOf("value2=") + 7;
  endIndex = payload.indexOf(",", startIndex);
  value2 = (payload.substring(startIndex, endIndex) == "True");

  // Parse value3
  startIndex = payload.indexOf("value3=") + 7;
  endIndex = payload.indexOf(",", startIndex);
  value3 = (payload.substring(startIndex, endIndex) == "True");

  // Parse value4
  startIndex = payload.indexOf("value4=") + 7;
  value4 = (payload.substring(startIndex) == "True");

  Serial.println("Parsed Values:");
  Serial.print("value1: ");
  Serial.println(value1);
  Serial.print("value2: ");
  Serial.println(value2);
  Serial.print("value3: ");
  Serial.println(value3);
  Serial.print("value4: ");
  Serial.println(value4);

  available_bays = TOTAL_BAYS;
  if (value1) {
    available_bays--;
  }
  if (value2) {
    available_bays--;
  }
  if (value3) {
    available_bays--;
  }
  if (value4) {
    available_bays--;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing");

  Wire.begin(33, 32);

  lcdTimer.start();

  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Warte auf WLAN");

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

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WLAN verbunden");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  delay(2000);

  // Resolve the IP address of the server using DNS
  if (WiFi.hostByName(coap_server_str, ip)) {
    Serial.print("Resolved IP for ");
    Serial.print(coap_server_str);
    Serial.print(": ");
    Serial.println(ip);
  } else {
    Serial.println("DNS resolution failed!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DNS Fehler");
    delay(3000);
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Server gefunden");
  lcd.setCursor(0, 1);
  lcd.print(ip.toString());
  delay(1000);

  // Connect CoAP client to a server
  coap.response(handleCoapResponse);

  coap.start();

  coapTimer.start();
#endif
}

void loop() {
  if (last_available_bays != available_bays) {
    last_available_bays = available_bays;
    lcdTimer.start();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Parken Gleis 13:");
    lcd.setCursor(16-4-2, 1);
    lcd.print(available_bays);
    lcd.setCursor(16-4, 1);
    lcd.print("Frei");
  }

#ifndef TESTING
  if (coapTimer.hasElapsed()) {
    coapTimer.start();
    Serial.println("Getting CoAP data");
    coap.get(ip, coap_port, "boolean");
  }
  coap.loop();
#endif
}