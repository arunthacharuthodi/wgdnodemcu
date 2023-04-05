#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>

#define USER_SERIAL Serial
SocketIOclient socketIO;
 const int PWM_PIN = D1;
 int DUTY_RATIO = 127;
const char* ssid     = "MIA3";        
const char* password = "arun@root1";
void setup() {
  USER_SERIAL.begin(115200);
//pwm generation
  pinMode(PWM_PIN, OUTPUT);
  analogWriteFreq(10000); 
  analogWrite(PWM_PIN, DUTY_RATIO);


  //network connection
  WiFi.begin(ssid, password);            
  USER_SERIAL.print("Connecting to ");
  USER_SERIAL.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  USER_SERIAL.println('\n');
  USER_SERIAL.println("Connection established!");  
  USER_SERIAL.print("IP address:\t");
  USER_SERIAL.println(WiFi.localIP());    
  socketIO.begin("192.168.155.36", 3000,"/socket.io/?EIO=4");
  // event handler
  socketIO.onEvent(socketIOEvent);
}
void loop() { 
	socketIO.loop();
  }

void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case sIOtype_DISCONNECT:
            USER_SERIAL.printf("[IOc] Disconnected!\n");
            break;
        case sIOtype_CONNECT:
            USER_SERIAL.printf("[IOc] Connected to url: %s\n", payload);

            // join default namespace (no auto join in Socket.IO V3)
            socketIO.send(sIOtype_CONNECT, "/");
            break;
        case sIOtype_EVENT:
            USER_SERIAL.printf("[IOc] get event: %s\n", payload);


            // Cast the payload to a char array
            char* payloadStr = (char*) payload;

            // Find the position of the "message" field in the payload
            char* messagePos = strstr(payloadStr, "\"message\"");

            if (messagePos != NULL) {
              // Skip past the "message" field name and the colon
              char* valuePos = messagePos + strlen("\"message\":");

              // Find the end of the value (which could be a number or a string)
              char* endPos = valuePos;
              while (*endPos != ',' && *endPos != '}' && *endPos != '\0') {
                endPos++;
              }

              // Extract the value from the payload as a string
              char valueStr[32];
              strncpy(valueStr, valuePos, endPos - valuePos);
              valueStr[endPos - valuePos] = '\0';

              // Convert the value to an integer
              valueStr[strlen(valueStr)-1] = '\0'; 
              // valueStr[strlen(valueStr)-1] = '\0'; 
              // valueStr[0] = '\0'; 
              char *result = valueStr+1;

              int value = atoi(result);
              USER_SERIAL.println(result);

              // Print the value
              printf("The value of 'message' is: %d\n", value);
              DUTY_RATIO = value;
              analogWrite(PWM_PIN, DUTY_RATIO);
              printf("The value of 'duty ratio' is changed to: %d\n", DUTY_RATIO);

            }
            else {
              printf("No 'message' field found in payload.\n");
            }
            break;

    }
}
