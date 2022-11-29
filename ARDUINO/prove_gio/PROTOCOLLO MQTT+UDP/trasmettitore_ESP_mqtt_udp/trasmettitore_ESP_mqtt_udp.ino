#include <SPI.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
//====== TRANSMITTER ===== //
// scheda: Arduino Mega

WiFiClient client;
WiFiUDP Udp;
PubSubClient MQTT_client(client);

// address of broker
IPAddress catalog_ip(192, 168, 1, 96);
int broker_port = 1883;
// adress of receiver
IPAddress destIp(192, 168, 1, 212); //Destination ip address Trivella
unsigned int destPort = 5005; //Destination port
unsigned int localPort = 5000;

// variables of SETTING PIN SENSORS
//===========================
int pinLED_red = 5; //D1
int pin_SENSOR = 4; //D2
String my_status = "";

// variables of INTERNET CONNECTION
//===========================
const char* ssid = "FASTWEB-D6A39D";
const char* password = "AEF84N31YA";

// variables of MQTT CONNECTION
//===========================
const char* my_name = "casotto";
const char* topic = "riserva_idrica/galleggiante/casotto";
const char* topic_sub = "catalog/riserva_idrica/+";
String MQTT_STATE = "";
unsigned long lastMsg_mqtt = 0;
unsigned long T_error_MQTT = 0;

// variables of UDP TRASMETTITORE
//===========================
char packetBuffer[7]; //buffer to hold incoming packet,
String risposta = "";
unsigned long T_send_UDP = 0;
unsigned long lastMsg_sent = 0;
unsigned long T_error_UDP = 0;
unsigned long lastMsg_udp = 0;

// variables of MANUAL MANAGEMENT
//==============================
String modality = "automatico";

// variables of TIME MANAGEMENT
//===========================
unsigned long now = 0;
int Soglia = 10 * 1000;

//=========================================================================================
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(pinLED_red, OUTPUT);
  pinMode(pin_SENSOR, INPUT);
  //
  Serial.print("trying to connect to wifi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WiFi.persistent(true);
  WiFi.setAutoConnect (true);
  WiFi.setAutoReconnect(true);
  Serial.println("WIFI CONNECTED!");
  delay(1000);
  Udp.begin(localPort);
  delay(1000);
  MQTT_client.setServer(catalog_ip, 1883);
  MQTT_client.setCallback(callback);
  Serial.println("Device Initialized!");
}

void loop() {
  now = millis();
  MQTT_client.loop();
  if (!MQTT_client.connected()) {
    MQTT_client.connect(my_name);
    if (MQTT_client.connected()) {
      Serial.println("MQTT SUBSCRIBER CONNECTED");
      MQTT_client.subscribe(topic_sub, 1);
    }
  }
  //SENSOR MANAGMENT
  //======================================================
  int SENSOR_STATE = digitalRead(pin_SENSOR);
  if (SENSOR_STATE == 1) {
    my_status = "ACCESO";
    digitalWrite(pinLED_red, HIGH);
  }
  if (SENSOR_STATE == 0) {
    my_status = "SPENTO";
    digitalWrite(pinLED_red, LOW);
  }

  //MQTT PUBLISH MANAGEMENT
  //======================================================
  T_error_MQTT = now - lastMsg_mqtt;
  if (T_error_MQTT >= Soglia) {
    Serial.println("MQTT REFRESHING");
    MQTT_STATE = "REFRESH";
  }
  MQTT_PUBLISH();

  if (modality == "automatico") {
    //UDP MANAGEMENT
    //======================================================
    T_error_UDP = now - lastMsg_udp;
    if (T_error_UDP >= Soglia) {
      Serial.println("REFRESHING");
      risposta = "REFRESH";
    }
    TRANSMITTION_FUNCTION();
    RECEIVER_FUNCTION();
  }
  else {
    lastMsg_udp = now;
  }

}

void MQTT_PUBLISH() {
  if (MQTT_STATE != my_status) {
    if (MQTT_client.connected()) {
      String payload = my_status;
      int payload_length = payload.length();
      if (MQTT_client.publish(topic, (char*) payload.c_str())) {
        MQTT_STATE = my_status;
        lastMsg_mqtt = now;
        Serial.println("mqtt messange sent!");
      }
    }
    else {
      Serial.println("Trying to establish MQTT connession...");
      MQTT_client.connect(my_name);
      if (MQTT_client.connected()) {
        Serial.println("MQTT CONNECTED!");
        MQTT_client.subscribe(topic_sub, 1);
      }
    }
  }
}

void TRANSMITTION_FUNCTION() {
  if (my_status != risposta) {
    T_send_UDP = now - lastMsg_sent;
    if (T_send_UDP > 1000) {
      Serial.print("SEND: " + my_status + " ... ");
      Udp.beginPacket(destIp, destPort); //Start Packet
      char copy[7];
      my_status.toCharArray(copy, 7);
      Udp.write(copy);
      Udp.endPacket(); //Close Packet
    }
  }
}
void RECEIVER_FUNCTION() {
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    risposta = "";
    Udp.read(packetBuffer, 6);
    risposta = String(packetBuffer);
    Serial.println(" RECEIVED: " + risposta);
    lastMsg_udp = now;
  }
}

void callback(char* topic, byte * payload, unsigned int length) {
  char* token;
  char* rest = topic;
  int i = 1;
  while ((token = strtok_r(rest, "/", &rest))) {
    if (i == 3) {
      modality = String(token);
    }
    i += 1;
  }
  Serial.println("modality changed to " + modality);
}
