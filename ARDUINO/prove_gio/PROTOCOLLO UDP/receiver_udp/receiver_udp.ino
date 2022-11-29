#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>
#include <EthernetUdp.h>
#include <timer.h>

IPAddress catalog_ip(192, 168, 1, 96);
int catalog_port = 8084;
EthernetClient client;
EthernetUDP Udp;
auto timer = timer_create_default(); // create a timer with default settings



// variables of SETTING PIN SENSOR
//===========================
int pin_LUCE = 2;

// variables of INTERNET CONNECTION
//===========================
byte myIP[] = { 192, 168, 1, 212 };
byte dns[] = { 192, 168, 1, 1 };
byte gateway[] = { 192, 168, 1, 1 };
byte subnet[] = { 255, 255, 255, 0 };
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };


// variables of SETTING FILE
//===========================
String my_name = "Pompa_casotto";
String postMessage;
String myIP_string;

// variables of UDP RECEIVER
//===========================
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
unsigned int localPort = 5005;      // local port to listen on
String my_status = "SPENTO";

// variables of POST METHOD
//===========================
String POST_status = "start";

// variables of MISURA POTENZA
//===========================
#define misure 1000
#define taratura 510
double sensorValue;
int sensorPin = A1;
int interrompi = 0;
int valore = 0;
//double taratura = 0;
//------misura energia-----------------------

// variables of time_count
//===========================
int T_error = 0;
int T_refres = 0;
int Soglia = 50;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    delay(10); // wait for serial port to connect. Needed for native USB port only
  }
  Ethernet.begin(mac, myIP, gateway, subnet);
  delay(1000);
  Udp.begin(localPort);
  delay(1000);
  pinMode(pin_LUCE, OUTPUT);       // LED on pin 2
  pinMode(3, OUTPUT);
  timer.every(1000, time_count);
}

void loop() {
  timer.tick(); // tick the timer
  if (client.connected() == false) {
    client.connect(catalog_ip, catalog_port);
  }
  RECEIVER_FUNCTION();
  if (my_status == "ACCESO") {
    digitalWrite(pin_LUCE, HIGH);
  }
  if (my_status == "SPENTO") {
    digitalWrite(pin_LUCE, LOW);
  }
  if (T_error >= Soglia + 3) {
    Serial.println("TRASMITTER IS NOT ONLINE. ");
    my_status = "SPENTO";
    Misura_Potenza();
    Serial.println("REFRESH");
    POST_status = "";
    POST_METHOD();
  }
}

void SERVER_CONNECTION() {
  // Function General Description:
  // start the Ethernet connection and get a local IP randomly assigned
  // ===================================================================

  Serial.println("Connecting to SERVER....");
  client.connect(catalog_ip, catalog_port);
  delay(500);
  if (client.connected()) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    IPAddress myIP = Ethernet.localIP();
    myIP_string = String(myIP[0]) + "." + String(myIP[1]) + "." + String(myIP[2]) + "." + String(myIP[3]);
    Serial.println(myIP_string);
  }
  else {
    //client.stop();
    Serial.println("connection failed!");
  }

}




void SETTING_FILE() {
  // create REGISTRATION JSON FILE
  // action: node = send registration request
  //         refresh = send refresh request
  // ======================================================================
  postMessage = "";
  const size_t capacity = JSON_OBJECT_SIZE(7);
  StaticJsonDocument<capacity> doc;
  doc["Name"] = my_name;
  doc["Status"] = my_status;
  //doc["address"] = "";
  //doc["UDPport"] = "";
  doc["type"] = "receiver";
  doc["SensorData"] = valore;
  doc["SensorName"] = "Misuratore_Corrente";
  // Serialize JSON document
  serializeJson(doc, postMessage);
}

void POST_METHOD() {
  // action: node = send registration request
  //         refresh = send refresh request
  // =====================================================================
  if (client.connected()) {
    if (POST_status != my_status) {
      Misura_Potenza();
      SETTING_FILE();
      Serial.print("POSTING to ");
      Serial.println(client.remoteIP());
      Serial.println(postMessage);

      client.println("POST /catalog/register HTTP/1.0");
      client.println("Content-Type: application/json");
      client.println("Content-Length: " + String(postMessage.length()));
      client.println();
      client.print(postMessage);
      client.flush();

      POST_status = my_status;
      T_error = 0;
    }
  }
}

void RECEIVER_FUNCTION() {
  int packetSize = Udp.parsePacket(); // verifica arrivo pacchetto
  if (packetSize) {
    Udp.read(packetBuffer, 6);
    String comando = String(packetBuffer);
    Serial.print ("RECEIVED  " + comando + " ... ");
    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    char copy[7];
    comando.toCharArray(copy, 7);
    Udp.write(copy);
    Udp.endPacket();
    Serial.println("REPLIED: " + comando );
    if (my_status != comando) {
      my_status = comando;
      POST_METHOD();
    }
    T_error = 0;
  }
}

bool time_count() {
  T_refres += 1;
  T_error += 1;
  return true;
}


void Misura_Potenza() {
  int z = 0;
  double maxval = 0;
  double Vmax = 0;
  for ( z = 0; z < misure; z++)
  {
    sensorValue = analogRead(sensorPin);
    if (sensorValue > maxval) maxval = sensorValue;
  }
  // if (digitalRead(2)==LOW){
  Vmax = maxval;
  //                    }
  //  if (digitalRead(2)==HIGH) {valore = ((maxval-taratura)*0.146*220/1.414);}

  valore = (maxval * 0.146 * 220 / 1.414);
  Serial.print ("ValoreMax..:");
  Serial.println (String(Vmax));
  Serial.print ("Valore..:");
  // Serial.print (maxval);
  Serial.println (String(valore));
} // Fine routine Misura
