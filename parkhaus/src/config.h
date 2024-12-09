#include <Arduino.h>

// #define TESTING

// Konfiguriere Wlan
const char* ssid = "smart";
const char* password = "11111111";

// Konfiguriere CoAP-Server
const char* coap_server_str = "192.168.0.100";
const int coap_port = 5683;

// Konfiguriere Pins der Ultraschallsensoren
#define US_1_TRIG 16
#define US_1_ECHO 22

#define US_2_TRIG 18
#define US_2_ECHO 23

#define US_3_TRIG 5
#define US_3_ECHO 21

#define US_4_TRIG 17
#define US_4_ECHO 19

// Konfiguriere Pins der WS2812b LEDs
#define LED_PIN 15

// Konfiguriere Pins der Richtungsanzeige (LED Matrix)
#define AL_PIN 32
#define AR_PIN 33

// Konfiguriere Pins des Buttons und Servos
#define BTN_PIN 25
#define SERVO_PIN 26
