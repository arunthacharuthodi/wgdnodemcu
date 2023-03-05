#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ArduinoJson.h>

#include <WebSocketsClient.h>
#include <SocketIOclient.h>

#include <Hash.h>

SocketIOclient socketIO;
 const int PWM_PIN = D1;
 int DUTY_RATIO = 127;


void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case sIOtype_DISCONNECT:
            Serial.printf("[IOc] Disconnected!\n");
            break;
        case sIOtype_CONNECT:
            Serial.printf("[IOc] Connected to url: %s\n", payload);

            // join default namespace (no auto join in Socket.IO V3)
            socketIO.send(sIOtype_CONNECT, "/nodemcu");
            break;
        case sIOtype_EVENT:
            Serial.printf("[IOc] get event: %s\n", payload);
            break;
        case sIOtype_ACK:
            Serial.printf("[IOc] get ack: %u\n", length);
            hexdump(payload, length);
            break;
        case sIOtype_ERROR:
            Serial.printf("[IOc] get error: %u\n", length);
            hexdump(payload, length);
            break;
        case sIOtype_BINARY_EVENT:
            Serial.printf("[IOc] get binary: %u\n", length);
            hexdump(payload, length);
            break;
        case sIOtype_BINARY_ACK:
            Serial.printf("[IOc] get binary ack: %u\n", length);
            hexdump(payload, length);
            break;
    }
}


const char* ssid     = "MIA3";        
const char* password = "arun@root1";

void setup() {
  Serial.begin(9600);         
//pwm generation
  pinMode(PWM_PIN, OUTPUT);

  // Set the PWM frequency to 10kHz on GPIO5 (D1 pin)
  analogWriteFreq(10000);
  analogWrite(PWM_PIN, DUTY_RATIO);
  //network connection
  WiFi.begin(ssid, password);            
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());    
  
  
       // websocks
   // server address, port and URL
    socketIO.begin("192.168.50.36", 443, "/socket.io/?EIO=4");

    // event handler
    socketIO.onEvent(socketIOEvent);
	// try ever 500 again if connection has failed
	// webSocket.setReconnectInterval(500);






}
void loop() { 
	socketIO.loop();
  }














