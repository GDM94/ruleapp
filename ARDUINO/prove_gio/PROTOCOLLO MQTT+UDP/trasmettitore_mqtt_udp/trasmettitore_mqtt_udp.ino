#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <EthernetUdp.h>
//====== TRANSMITTER ===== //
// scheda: Arduino Mega

EthernetClient client;
EthernetUDP Udp;
PubSubClient MQTT_client(client);

// address of broker
IPAddress catalog_ip(192, 168, 1, 96);
int broker_port = 1883;
// adress of receiver
IPAddress destIp(192, 168, 1, 212); //Destination ip address Trivella
unsigned int destPort = 5005; //Destination port

// variables of SETTING PIN SENSORS
//===========================
int pinLED_red = 24;
int pinLED_green = 23;
int pin_SENSOR = 22;
String my_status = "";

// variables of INTERNET CONNECTION
//===========================
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF };
byte myIP[] = { 192, 168, 1, 211 };
//byte dns[] = { 192, 168, 1, 1 };
byte gateway[] = { 192, 168, 1, 1 };
byte subnet[] = { 255, 255, 255, 0 };

// variables of MQTT CONNECTION
//===========================
const char* my_name = "gebbia";
const char* topic = "riserva_idrica/galleggiante/gebbia";
const char* topic_sub = "catalog/riserva_idrica/+";
String MQTT_STATE = "";
unsigned long lastMsg_mqtt = 0;
unsigned long T_error_MQTT = 0;
unsigned long T_reconnect_MQTT = 0;
unsigned long lastConnect_mqtt = 0;
int mqtt_connection_state;

// variables of UDP TRASMETTITORE
//===========================
IPAddress ip(192, 168, 1, 211); //Local IP address
unsigned int localPort = 5000; // local port to listen on
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
  pinMode(pinLED_green, OUTPUT);
  pinMode(pinLED_red, OUTPUT);
  pinMode(pin_SENSOR, INPUT);
  digitalWrite(pin_SENSOR, LOW);
  //

  Ethernet.begin(mac);
  delay(1000);
  Udp.begin(localPort);
  delay(1000);
  MQTT_client.setServer(catalog_ip, 1883);
  MQTT_client.setCallback(callback);
  MQTT_client.subscribe(topic_sub, 1);
  Serial.println("Device Initialized!");
}

void loop() {
  now = millis();
  Ethernet.maintain();
  MQTT_client.loop();
  //SENSOR MANAGMENT
  //======================================================
  SENSOR_MANAGMENT();

  //MQTT PUBLISH MANAGEMENT
  //======================================================
  if (my_status == risposta) {
    MQTT_RECONNECT();
    if (MQTT_client.connected()) {
      T_error_MQTT = now - lastMsg_mqtt;
      if (T_error_MQTT >= Soglia) {
        Serial.println("MQTT REFRESHING");
        MQTT_STATE = "REFRESH";
      }
      MQTT_PUBLISH();
    }
  }


  if (modality == "automatico") {
    //UDP MANAGEMENT
    //======================================================
    T_error_UDP = now - lastMsg_udp;
    if (T_error_UDP >= Soglia) {
      risposta = "REFRESH";
    }
    TRANSMITTION_FUNCTION();
  }
  else {
    lastMsg_udp = now;
    risposta = my_status;
  }

} //

void SENSOR_MANAGMENT() {
  int SENSOR_STATE = digitalRead(pin_SENSOR);
  if (SENSOR_STATE == 1) {
    my_status = "ACCESO";
    digitalWrite(pinLED_green, HIGH);
    digitalWrite(pinLED_red, LOW);
  }
  if (SENSOR_STATE == 0) {
    my_status = "SPENTO";
    digitalWrite(pinLED_green, LOW);
    digitalWrite(pinLED_red, HIGH);
  }
}

void MQTT_PUBLISH() {
  if (MQTT_STATE != my_status) {
    String payload = my_status;
    int payload_length = payload.length();
    if (MQTT_client.publish(topic, (char*) payload.c_str())) {
      Serial.println("mqtt messange sent!");
      MQTT_STATE = my_status;
      lastMsg_mqtt = now;
    }
  }
}

void MQTT_RECONNECT() {
  if (!MQTT_client.connected()) {
    T_reconnect_MQTT = now - lastConnect_mqtt;
    if (T_reconnect_MQTT > 5000) {
      Serial.println("mqtt try to reconnect...");
      lastConnect_mqtt = now;
      MQTT_client.connect(my_name);
      if (MQTT_client.state() == 0) {
        Serial.println("MQTT SUBSCRIBER CONNECTED");
        MQTT_client.subscribe(topic_sub);
      }
    }
  }
}

void TRANSMITTION_FUNCTION() {
  if (my_status != risposta) {
    T_send_UDP = now - lastMsg_sent;
    if (T_send_UDP > 500) {
      lastMsg_sent = now;
      Serial.print("SEND: " + my_status + " ... ");
      Udp.beginPacket(destIp, destPort); //Start Packet
      char copy[7];
      my_status.toCharArray(copy, 7);
      Udp.write(copy);
      Udp.endPacket(); //Close Packet
    }
    int packetSize = Udp.parsePacket();
    risposta = "";
    if (packetSize > 5) {
      Udp.read(packetBuffer, 6);
      risposta = String(packetBuffer);
      Serial.println(" RECEIVED: " + risposta);
      lastMsg_udp = now;
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
