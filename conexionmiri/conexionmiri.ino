#include <WiFi.h> //Wifi library
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

const char *ssid = "miriwifi";
const char *password = "password";

#define MQTT_SERVER "teachinghub.eif.urjc.es"
uint16_t MQTT_PORT = 21883;
#define MQTT_USERNAME "tanquesito"
#define MQTT_KEY "5"

#define RXD2 33
#define TXD2 4

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_KEY);

const char *topic = "/SETR/2024/5/";
const char *id_equipo = "5";

void connectToWiFi() {
  Serial.print(F("Connecting to network: "));
  Serial.println(ssid);
  WiFi.disconnect(true); 

  WiFi.begin(ssid, password); 

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println("");
  Serial.println(F("WiFi is connected!"));
  Serial.println(F("IP address set: "));
  Serial.println(WiFi.localIP()); //print LAN IP
}

// Should be called in the loop function and it will take care if connecting.
void connectToMQTT() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

void setup() {
  Serial.begin(9600);
  // Arduino
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  connectToWiFi();
  
}

void loop() {
  // Ensure the connection to the MQTT server is alive
  if (!mqtt.connected()) {
    connectToMQTT();
  }
}
