#include <Arduino.h>

#define TESTING

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

const char* coap_server_str = "jonahwille.de";

const int coap_port = 5683;

// echo 23, 22, 21, 19
// trig 18, 5, 17, 16

#define US_1_TRIG 16
#define US_1_ECHO 22

#define US_2_TRIG 18
#define US_2_ECHO 23

#define US_3_TRIG 5
#define US_3_ECHO 21

#define US_4_TRIG 17
#define US_4_ECHO 19

#define LED_PIN 15

#define AL_PIN 32
#define AR_PIN 33

#define BTN_PIN 25
#define SERVO_PIN 26
