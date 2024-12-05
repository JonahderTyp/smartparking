
#include <Arduino.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <coap-simple.h>

#include "config.h"
#include "timer.h"

// IP address of the CoAP server
IPAddress ip;

// Timer für die Aktualisierung der Daten
Timer coapTimer(3000);

// UDP object for CoAP communication
WiFiUDP udp;
// CoAP object
Coap coap(udp);

// LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variablen für die Anzahl der freien Parkplätze und check ob Verfügbarkeit
// sich geändert hat
int available_bays = -1;
int last_available_bays = -1;

// Callback function for CoAP Antwort
void handleCoapResponse(CoapPacket &packet, IPAddress ip, int port) {
  // Konvertieren der payload in einen String
  String payload;
  for (int i = 0; i < packet.payloadlen; i++) {
    payload += (char)packet.payload[i];
  }

  // Debug output
  Serial.println("Response from server:");
  Serial.println(payload);

  // Parse the response into variables
  int startIndex = 0;
  int endIndex = 0;

  // Parse the JSON string
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload);

  // falls ein Fehler auftritt oder die JSON nicht die erwarteten Felder
  // enthält, breche ab
  if (error || !(doc["p1"].is<bool>() && doc["p2"].is<bool>() &&
                 doc["p3"].is<bool>() && doc["p4"].is<bool>())) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // Lese die Daten aus und dekrementiere die verfügbaren Parkplätze

  bool p1 = doc["p1"];
  bool p2 = doc["p2"];
  bool p3 = doc["p3"];
  bool p4 = doc["p4"];

  available_bays = 0;
  if (!p1) {
    available_bays++;
  }
  if (!p2) {
    available_bays++;
  }
  if (!p3) {
    available_bays++;
  }
  if (!p4) {
    available_bays++;
  }
}

void setup() {
  // Beginne Serielle Kommunikation
  Serial.begin(115200);
  Serial.println("Initializing");

  // Initialisiere das LCD an den pins 33 (SDA) und 32 (SCL)
  Wire.begin(33, 32);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Warte auf WLAN");

  // Falls TESTING definiert ist, wird der folgende Code nicht ausgeführt
  // und stattdessen werden Demo Daten auf dem LCD ausgegeben
#ifndef TESTING

  // Verbinde mit WLAN
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

  delay(2000);  // warte 2 Sekunden um LCD zu lesen

  // Bekomme die Server IP via DNS
  // falls die Server IP bereits eine IP ist, wird diese verwendet
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

  // Verbinde die Callback Funktion
  coap.response(handleCoapResponse);

  // Initalisiere CoAP
  coap.start();

  // Starte den CoAP Timer
  coapTimer.start();
#endif
}

void loop() {
  // falls sich die Anzahl der verfügbaren Parkplätze geändert hat,
  // aktualisiere das LCD
  if (last_available_bays != available_bays) {
    last_available_bays = available_bays;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Parken Gleis 13:");
    lcd.setCursor(16 - 4 - 2, 1);
    lcd.print(available_bays);
    lcd.setCursor(16 - 4, 1);
    lcd.print("Frei");
  }

#ifndef TESTING

  // Aktualisiere die CoAP Daten
  if (coapTimer.hasElapsed()) {
    coapTimer.start();
    coap.get(ip, coap_port, "boolean");
  }

  // Wird benötigt damit die coap-bib die Antwort verarbeiten kann
  coap.loop();
#endif
}