#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>
#include <EthernetUdp.h>
#include <timer.h>
//====== TRANSMITTER ===== //
// versione: definitiva
// scheda: Arduino Mega

IPAddress catalog_ip(192, 168, 1, 96);
int catalog_port = 8084;
EthernetClient client;
EthernetUDP Udp;
auto timer = timer_create_default(); // create a timer with default settings

// variables of SETTING PIN SENSORS
//===========================
int pinLED_red = 24;
int pinLED_green = 23;
int pin_SENSOR = 22;

// variables of INTERNET CONNECTION
//===========================
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF };
byte myIP[] = { 192, 168, 1, 211 };
byte dns[] = { 192, 168, 1, 1 };
byte gateway[] = { 192, 168, 1, 1 };
byte subnet[] = { 255, 255, 255, 0 };
int connection = 0;

// variables of SETTING FILE
//===========================
String my_name = "interruttore_pompa";
String postMessage;

// variables of UDP TRASMETTITORE
//===========================
IPAddress ip(192, 168, 1, 211); //Local IP address
IPAddress destIp(192, 168, 1, 212); //Destination ip address
unsigned int localPort = 5000; // local port to listen on
unsigned int destPort = 5005; //Destination port
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
String my_status = "";
String risposta = "";

// variables of POST METHOD
//===========================
String POST_status = "start";

// variables of time_count
//===========================
int T_error = 0;
int T_refres = 0;
int Soglia = 50;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(pinLED_green, OUTPUT);
  pinMode(pinLED_red, OUTPUT);
  //

  Ethernet.begin(mac, ip);
  delay(1000);
  Udp.begin(localPort);
  delay(1000);
  timer.every(1000, time_count);
}

void loop() {
  timer.tick(); // tick the timer
  toggle_led();
  if (client.connected() == false) {
    client.connect(catalog_ip, catalog_port);
  }
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
  if (T_error >= Soglia) {
    Serial.println("REFRESHING");
    POST_status = "REFRESH"; // triggero il refresh via POST
    risposta = "REFRESH"; // triggero il refresh via UDP
  }
  TRANSMITTION_FUNCTION();
  if (my_status == risposta) {
    POST_METHOD();
  }
}




void SETTING_FILE() {
  // create REGISTRATION JSON FILE
  // action: node = send registration request
  //         refresh = send refresh request
  // ======================================================================
  postMessage = "";
  const size_t capacity = JSON_OBJECT_SIZE(5) + 70;
  StaticJsonDocument<capacity> doc;
  doc["Name"] = my_name;
  doc["Status"] = my_status;
  IPAddress myIP = Ethernet.localIP();
  doc["address"] = String(myIP[0]) + "." + String(myIP[1]) + "." + String(myIP[2]) + "." + String(myIP[3]);
  doc["UDPport"] = localPort;
  doc["type"] = "trasmitter";
  // Serialize JSON document
  serializeJson(doc, postMessage);
}


void POST_METHOD() {
  // action: node = send registration request
  //         refresh = send refresh request
  // =====================================================================
  if (client.connected()) {
    if (POST_status != my_status) {
      SETTING_FILE();
      Serial.print("POSTING to ");
      Serial.print(client.remoteIP());
      Serial.println(postMessage);

      client.print("POST /");
      client.println("catalog/register/ HTTP/1.0");
      client.println("Content-Type: application/json");
      //client.println("Connection:close");
      client.print("Content-Length:");
      client.println(postMessage.length());
      client.println();
      client.print(postMessage);
      client.flush();
      POST_status = my_status;
      T_refres = 0;
    }
  }
}

void TRANSMITTION_FUNCTION() {
  if (my_status != risposta) {
    Serial.print("SEND: " + my_status + " ... ");
    Udp.beginPacket(destIp, destPort); //Start Packet
    char copy[7];
    my_status.toCharArray(copy, 7);
    Udp.write(copy);
    Udp.endPacket(); //Close Packet
    delay(500);
    //===========================
    int packetSize = Udp.parsePacket();
    risposta = "";
    if (packetSize) {
      Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
      risposta = String(packetBuffer);
      Serial.println("RECEIVED: " + risposta);
      T_error = 0;
      T_refres = 0;
    }
  }
}

bool time_count() {
  T_error += 1;
  T_refres += 1;
  return true;
}

void toggle_led() {
  if (T_error >= Soglia + 3) {
    Serial.println("ERROR");
    digitalWrite(pinLED_red, HIGH);
    delay (200);
    digitalWrite(pinLED_red, LOW);
    delay (300);
    digitalWrite(pinLED_red, HIGH);
    delay (200);
    digitalWrite(pinLED_red, LOW);
    delay (200);
    digitalWrite(pinLED_red, HIGH);
    delay (200);
    digitalWrite(pinLED_red, LOW);
    delay (200);
    digitalWrite(pinLED_red, HIGH);
  }
}
