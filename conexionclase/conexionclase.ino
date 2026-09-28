#include <WiFi.h> //Wifi library
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

const char *ssid = "eduroam";
//const char *password = "password";
#define EAP_ANONYMOUS_IDENTITY "20220719anonymous@urjc.es" // leave as it is
#define EAP_IDENTITY "n.casado.2022@alumnos.urjc.es"    // Use your URJC email
#define EAP_PASSWORD "***"            // User your URJC password
#define EAP_USERNAME "n.casado.2022@alumnos.urjc.es"    // Use your URJC email


#define MQTT_SERVER "teachinghub.eif.urjc.es"
uint16_t MQTT_PORT = 21883;
#define MQTT_USERNAME "tanquesito"
#define MQTT_KEY "5"

#define RXD2 33
#define TXD2 4

#define TOPIC "/SETR/2024/5/"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_KEY);

const char *topic = "/SETR/2024/5/";
const char *id_equipo = "5";
long start_time, end_time, ping_time;
long distance = 000;
bool end_lap = false;
String dist = "";

void connectToWiFi() {
  Serial.print(F("Connecting to network: "));
  Serial.println(ssid);
  WiFi.disconnect(true); 

  WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD); 

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
    Serial2.print("G");
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
  Serial2.print("GO");
}

void sendStartLapMessage() {
  String payload = "{";
  payload += "\"team_name\": \"tanquesito\",";
  payload += "\"id\": \"" + String(id_equipo) + "\",";
  payload += "\"action\": \"START_LAP\"}";

  mqtt.publish(TOPIC, payload.c_str());
}

void sendEndLapMessage() {
  end_time = millis() - start_time;

  String payload = "{";
  payload += "\"team_name\": \"tanquesito\",";
  payload += "\"id\": \"" + String(id_equipo) + "\",";
  payload += "\"action\": \"END_LAP\",";
  payload += "\"time\": " + String(end_time); 
  payload += "}";

  mqtt.publish(TOPIC, payload.c_str());
}

void sendObstacleDetectedMessage() {
  String payload = "{";
  payload += "\"team_name\": \"tanquesito\",";
  payload += "\"id\": \"" + String(id_equipo) + "\",";
  payload += "\"action\": \"OBSTACLE_DETECTED\",";
  payload += "\"distance\": " + String(distance); 
  payload += "}";

  mqtt.publish(TOPIC, payload.c_str());
}

void sendLostLineMessage() {
  String payload = "{";
  payload += "\"team_name\": \"tanquesito\",";
  payload += "\"id\": \"" + String(id_equipo) + "\",";
  payload += "\"action\": \"LINE_LOST\"}";

  mqtt.publish(TOPIC, payload.c_str());
}

void sendFoundLine() {
  String payload = "{";
  payload += "\"team_name\": \"tanquesito\",";
  payload += "\"id\": \"" + String(id_equipo) + "\",";
  payload += "\"action\": \"LINE_FOUND\"}";

  mqtt.publish(TOPIC, payload.c_str());
}

void sendPingMessage() {
  end_time = millis() - start_time;

  String payload = "{";
  payload += "\"team_name\": \"tanquesito\",";
  payload += "\"id\": \"" + String(id_equipo) + "\",";
  payload += "\"action\": \"PING\",";
  payload += "\"time\": " + String(end_time); 
  payload += "}";

  mqtt.publish(TOPIC, payload.c_str());
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

  String buffer = "";
  char read;

  // cerrarlo en bucle tipo tanquesito (while (buffer != "GO")) para forzarlo a palabra
  // ver si no se repiten letras y hace palabra entera
  // cerrarlo con simbolo clave tipo } 
  if (Serial2.available() > 0) {
    read = Serial2.read();
    Serial.print(read);
    if (read != '\n' && read != '\r') { // Ignora caracteres de nueva línea
      buffer += read; // Añade el carácter al buffer
    }
    Serial.println(buffer);
  }

  // El ping distinto? Con thread para cada 4s ejecutarlo??
  if (buffer == "S") {
    start_time = millis();
    ping_time = millis();
    sendStartLapMessage();
  } else if (buffer == "E") {
    end_lap = true;
    sendEndLapMessage();
  } else if (buffer == "O") {
    dist = Serial2.readStringUntil('-');
    distance = dist.toInt();
    sendObstacleDetectedMessage();
  } else if (buffer == "L") {
    sendLostLineMessage();
  } else if (buffer == "F") {
    sendFoundLine();
  }

  if (((millis() - ping_time) >= 4000) && (!end_lap)) {
    sendPingMessage();
    ping_time = millis();
  }
}
