#include <Arduino.h>

// Falls TESTING definiert ist, wird CoAP nicht verwendet, Wifi nicht verbunden
// und DEMO Daten auf dem LCD ausgegeben
// #define TESTING

// Konfiguriere WLAN Zugangsdaten
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Konfiguriere CoAP Server
const char* coap_server_str = "172.16.2.107";
const int coap_port = 5683;
