#include <Arduino.h>
#include <WiFi.h>

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