#include <Arduino.h>
#include <WiFi.h>
<<<<<<< HEAD
#include <HTTPClient.h>

const char* ssid = "Dein_SSID";           // WLAN-SSID
const char* password = "Dein_Passwort";   // WLAN-Passwort
const String serverUrl = "http://dein-server/api/updateStatus";  // URL der API-Schnittstelle

// Ultraschallsensoren Pins (jeweils für 4 Parkplätze)
const int trigPins[] = {5, 18, 19, 21};   // Trig-Pins für Sensoren
const int echoPins[] = {23, 22, 2, 4};    // Echo-Pins für Sensoren
const int numSensors = 4;                 // Anzahl der Sensoren
const long schwellwert = 50;              // Schwellwert in cm für "besetzt"

long messEntfernung(int trigPin, int echoPin) {
    // Trigger-Puls senden
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Dauer des Echos messen
    long dauer = pulseIn(echoPin, HIGH);

    // Entfernung berechnen (Schallgeschwindigkeit = 34300 cm/s)
    long entfernung = dauer * 0.034 / 2;
    return entfernung;
}

// Funktion zum Senden des Parkplatzstatus an den Server
void sendStatusToServer(int platzId, bool isOccupied) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/json");

        // JSON-Daten mit Parkplatz-Status
        String postData = "{\"parkplatz\": " + String(platzId) + ", \"status\": \"" + String(isOccupied ? "besetzt" : "frei") + "\"}";

        int httpResponseCode = http.POST(postData);

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println(httpResponseCode);
            Serial.println(response);
        } else {
            Serial.println("Fehler beim Senden des POST-Requests");
        }
        http.end();
    } else {
        Serial.println("Keine WLAN-Verbindung");
    }
}

void setup() {
    Serial.begin(115200);

    // WLAN-Verbindung herstellen
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Verbindung zu WiFi...");
    }
    Serial.println("Mit WiFi verbunden");

    // Ultraschallsensor-Pins initialisieren
    for (int i = 0; i < numSensors; i++) {
        pinMode(trigPins[i], OUTPUT);
        pinMode(echoPins[i], INPUT);
    }
}

void loop() {
    for (int i = 0; i < numSensors; i++) {
        // Entfernung messen
        long entfernung = messEntfernung(trigPins[i], echoPins[i]);

        // Überprüfen, ob der Parkplatz besetzt oder frei ist
        bool besetzt = (entfernung < schwellwert);
        Serial.print("Parkplatz ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(besetzt ? "besetzt" : "frei");

        // Status an den Server senden
        sendStatusToServer(i + 1, besetzt);
    }

    // Warten für 1 Sekunde, bevor die nächsten Messungen erfolgen
    delay(1000);
}
=======

#include "Thing.CoAP.h"
#include "Thing.CoAP/Client.h"
#include "Thing.CoAP/ESP/UDPPacketProvider.h"

//[RECOMMENDED] Alternatively, if you are NOT using Arduino IDE you can include
// each file you need as bellow: #include "Thing.CoAP/Client.h" #include
// "Thing.CoAP/ESP/UDPPacketProvider.h"

// Declare our CoAP client and the packet handler
Thing::CoAP::Client coapClient;
Thing::CoAP::ESP::UDPPacketProvider udpProvider;

// Change here your WiFi settings
char* ssid = "YourWiFiSSID";
char* password = "YourWiFiPassword";
const char* serverHostname = "yourserver.com";

void sendMessage(const char* payload) {
  // Make a post
  coapClient.Get("create1", payload, [](Thing::CoAP::Response response) {
    std::vector<uint8_t> recivedpayload = response.GetPayload();
    std::string received(recivedpayload.begin(), recivedpayload.end());
    Serial.println("Server sent the following message:");
    Serial.println(received.c_str());
  });
}

void setup() {
  // Initializing the Serial
  Serial.begin(115200);
  Serial.println("Initializing");

  // Try and wait until a connection to WiFi was made
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println("My IP: ");
  Serial.println(WiFi.localIP());

  // Configure our server to use our packet handler (It will use UDP)
  coapClient.SetPacketProvider(udpProvider);

  // Resolve the IP address of the server using DNS
  IPAddress ip;
  if (WiFi.hostByName(serverHostname, ip)) {
    Serial.print("Resolved IP for ");
    Serial.print(serverHostname);
    Serial.print(": ");
    Serial.println(ip);
  } else {
    Serial.println("DNS resolution failed!");
    return;
  }

  // Connect CoAP client to a server
  coapClient.Start(ip, 5683);

  // Send A Message
  sendMessage("Test Message");
}

void loop() { coapClient.Process(); }
>>>>>>> main
