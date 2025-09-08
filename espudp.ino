#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "HUSSAIN";
const char* password = "123456789";

WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on

char incomingPacket[255];  // buffer for incoming packets

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  
  // Start UDP
  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}

void loop() {
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    // Receive incoming UDP packet
    int len = Udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;  // Null-terminate the string
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);
    
    // Forward the received string to Arduino Uno via Serial
    Serial.println(incomingPacket);
  }
}