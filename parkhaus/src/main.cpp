#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <Ultrasonic.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

#include "button/button.h"
#include "config.h"
#include "timer.h"

// IP Adresse des CoAP servers
IPAddress ip;

// Initialisiere Ultraschallsensoren
Ultrasonic u1(US_1_TRIG, US_1_ECHO);
Ultrasonic u2(US_2_TRIG, US_2_ECHO);
Ultrasonic u3(US_3_TRIG, US_3_ECHO);
Ultrasonic u4(US_4_TRIG, US_4_ECHO);

// Initialisiere WS2812b LED Streifen
Adafruit_NeoPixel strip(4, LED_PIN, NEO_GRB + NEO_KHZ800);

// Initialisiere Servo
Servo myservo;

// Timer für die Öffnungzeit der Schranke und Richtungsanzeige
Timer barrierTimer(2000);

// Timer für die Abfrage der Ultraschallsensoren
Timer checkTimer(100);

// Timer für die COAP Aktualisierung
Timer coapTimer(1000);

bool coapActive = true;

// UDP Object und coap für die CoAP communication
WiFiUDP udp;
Coap coap(udp);

// Button für die Öffnung der Schranke
Button button(BTN_PIN, 1000);

bool p1 = false;
bool p2 = false;
bool p3 = false;
bool p4 = false;

// Funktion zur Abfrage der Ultraschallsensoren
void checkBays() {
  // minimaler Abstand zum als besetzt markieren
  unsigned int besetzt_abstand = 8;

  // Falls der Sensor blockiert wird, ist der Gemessene wert maximal.
  // Da dies aber ein unlogischer Zustand ist, wird die Bucht als Besetzt
  // markiert. Der maximale Abstand zwischen Sensor und Boden ist 13cm.
  unsigned int max_abstand = 13;

  // Lese die Distanz der Sensoren
  // Warte zwischen den Messungen, da sonst die Sensoren sich gegenseitig stören
  delay(50);
  unsigned int u1d = u1.read(CM);
  delay(50);
  unsigned int u2d = u2.read(CM);
  delay(50);
  unsigned int u3d = u3.read(CM);
  delay(50);
  unsigned int u4d = u4.read(CM);

  // Überprüfe die gemessenen Abstände
  p1 = p2 = p3 = p4 = false;
  if (u1d <= besetzt_abstand || u1d >= max_abstand) p1 = true;
  if (u2d <= besetzt_abstand || u2d >= max_abstand) p2 = true;
  if (u3d <= besetzt_abstand || u3d >= max_abstand) p3 = true;
  if (u4d <= besetzt_abstand || u4d >= max_abstand) p4 = true;
}

// Funktionen zur Aktualisierung der Parkbucht Led Anzeigen
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

// Funktion zur Anzeige der Richtungspfeile
// 0b00 -> Kein Pfeil
// 0b01 -> Pfeil nach rechts
// 0b10 -> Pfeil nach links
// 0b11 -> Kreuz (Besetzt)
void displayArrow(int dir) {
  digitalWrite(AL_PIN, dir & 0b01);
  digitalWrite(AR_PIN, dir & 0b10);
}

// Funktionen zur Steuerung der Schranke
void openGate() { myservo.write(80); }
void closeGate() { myservo.write(170); }

void updateCoap() {
  if (coapTimer.hasElapsed()) {
    coapTimer.start();

    // Construct payload as a JSON-like string,
    JsonDocument doc;
    // Add the key-value pairs
    doc["p1"] = p1;
    doc["p2"] = p2;
    doc["p3"] = p3;
    doc["p4"] = p4;
    String payload;
    serializeJson(doc, payload);

    // Send data to the CoAP server
    coap.put(ip, coap_port, "boolean", payload.c_str(), payload.length());
  }
  if (!coap.loop()) {
    Serial.println("coap error");  // Keep the CoAP client running
  }
}

// Funktion um Wlan zu starten
// gibt 0 zurück wenn erfolgreich
// gibt 1 zurück wenn die Verbindung fehlgeschlagen ist
bool start_wifi() {
  strip.setPixelColor(0, strip.Color(0, 0, 255));
  strip.show();

  // Try and wait until a connection to WiFi was made
  WiFi.begin(ssid, password);

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startTime >= 10000) {
      Serial.println("WiFi connection timed out.");
      WiFi.disconnect();
      strip.setPixelColor(0, strip.Color(255, 0, 0));
      strip.show();
      return false;  // WiFi Connection failed
    }
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  strip.setPixelColor(0, strip.Color(0, 255, 0));
  strip.setPixelColor(1, strip.Color(0, 0, 255));
  strip.show();
  Serial.print("WiFi IP: ");
  Serial.println(WiFi.localIP());

  // Resolve the IP address of the server using DNS
  if (WiFi.hostByName(coap_server_str, ip)) {
    Serial.print("Resolved IP for ");
    Serial.print(coap_server_str);
    Serial.print(": ");
    Serial.println(ip);
  } else {
    strip.setPixelColor(1, strip.Color(255, 0, 0));
    strip.show();
    Serial.println("DNS resolution failed!");
    return false;  // DNS resolution failed
  }

  strip.setPixelColor(1, strip.Color(0, 255, 0));
  strip.setPixelColor(2, strip.Color(0, 0, 255));
  strip.show();

  // Connect CoAP client to a server
  if (!coap.start()) {
    Serial.println("Failed to start CoAP client");
    strip.setPixelColor(2, strip.Color(255, 0, 0));
    strip.show();
    return false;  // CoAP client failed to start
  }
  coap.response([](CoapPacket& packet, IPAddress ip, int port) {
    if (packet.code != 68 || packet.type != 2) {
      Serial.println("Received invalid CoAP response");
      Serial.print("Code: ");
      Serial.println(packet.code);
      Serial.print("Type: ");
      Serial.println(packet.type);
    }
  });
  coapTimer.start();
  strip.setPixelColor(2, strip.Color(0, 255, 0));
  strip.show();
  return true;  // WiFi connection successful
}

void setup() {
  // Initialisiere die serielle Schnittstelle
  Serial.begin(115200);
  delay(1000);
  Serial.println("Initializing");

  // Setzen der Pins als Ausgänge
  pinMode(AL_PIN, OUTPUT);
  pinMode(AR_PIN, OUTPUT);

  // Test der LED Matrix
  displayArrow(0b11);

  // Initalisiere den Servo und schließe die Schranke
  myservo.attach(SERVO_PIN);
  closeGate();

  // Initialisiere der LEDs
  strip.begin();
  strip.setBrightness(50);
  strip.clear();
  strip.show();

  // Starte den Timer für die Abfrage der Sensoren
  checkTimer.start();

  // Starte die WiFi Verbindung
  strip.setPixelColor(3, strip.Color(255, 0, 255));
  strip.show();
  if (!start_wifi()) {
    strip.setPixelColor(3, strip.Color(255, 0, 0));
    strip.show();
    coapActive = false;
  }

  delay(1000);
  strip.clear();
  strip.show();
}

void loop() {
  if (checkTimer.hasElapsed()) {
    checkBays();
    checkTimer.start();
  }
  int left = ((int)!p3) + ((int)!p4);
  int right = ((int)!p1) + ((int)!p2);
  setBayLEDs(p1, p2, p3, p4);

  if (left + right == 0) {
    displayArrow(0b11);
  } else if (!barrierTimer.isRunning()) {
    displayArrow(0b00);
  }

  if (button.isShortPress()) {
    barrierTimer.start();
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

  if (barrierTimer.hasElapsed()) {
    closeGate();
    displayArrow(0b00);
  }

  if (coapActive) {
    updateCoap();
  }

  button.handle();
}