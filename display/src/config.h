#include <Arduino.h>

// Falls TESTING definiert ist, wird CoAP nicht verwendet, Wifi nicht verbunden
// und DEMO Daten auf dem LCD ausgegeben
// #define TESTING

// Konfiguriere WLAN Zugangsdaten
const char* ssid = "smart";
const char* password = "11111111";

// Konfiguriere CoAP Server
const char* coap_server_str = "192.168.0.100";
const int coap_port = 5683;
