#include "PubSubClient.h" // Connect and publish to the MQTT broker

// Code for the ESP8266
#include "ESP8266WiFi.h"  // Enables the ESP8266 to connect to the local network (via WiFi)

#define INPUT_SIZE 100
// WiFi
const char* ssid = "AirPennNet-Device";                 // Your personal network SSID
const char* wifi_password = "penn1740wifi"; // Your personal network password

// MQTT
const char* mqtt_server = "10.103.215.11";  // IP of the MQTT broker
const char* root_topic = "test";
const char* mqtt_username = "test"; // MQTT username
const char* mqtt_password = "test"; // MQTT password
const char* clientID = "nodemcu"; // MQTT client ID

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient); 

// Custom function to connet to the MQTT broker via WiFi
void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}


void setup() {
  Serial.begin(115200);
  while(!Serial);
  connect_MQTT();
}

void loop() {

  if (Serial.available()) {
    char data[INPUT_SIZE+1];
    byte size = Serial.readBytesUntil('&', data, INPUT_SIZE);
    data[size] = 0;
//    Serial.println(data);
    // split string
    char * pch = strtok(data, " ");
    while (pch != 0)
    {
      // Split the data in two values
      char* val = strchr(pch, ':');
      if (val != 0)
      {
          // Actually split the string in 2: replace ':' with 0
          *val = 0;
          ++val;
          char topic[50];
          snprintf(topic, 50, "%s/%s\0", root_topic, pch);
          // PUBLISH to the MQTT Broker (topic = Temperature, defined at the beginning)
          if (client.publish(topic, val)) {
            Serial.print(topic+String(" "));
            Serial.print(val);
            Serial.println(" sent!");
          }
          // Again, client.publish will return a boolean value depending on whether it succeded or not.
          // If the message failed to send, we will try again, as the connection may have broken.
          else {
            Serial.println(" failed to send. Reconnecting to MQTT Broker and trying again");
            client.connect(clientID, mqtt_username, mqtt_password);
            delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
            client.publish(topic, val);
          }
      }
      // Find the next command in input string
      pch = strtok(0, " ");
    }
    Serial.println("");
  }

}
