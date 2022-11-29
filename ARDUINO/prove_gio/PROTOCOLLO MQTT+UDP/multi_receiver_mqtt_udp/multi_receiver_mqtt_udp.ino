#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <EthernetUdp.h>


IPAddress catalog_ip(192, 168, 1, 96);
int broker_port = 1883;
EthernetClient client;
PubSubClient MQTT_client(client);
EthernetUDP Udp;

// variables of SETTING PIN SENSOR
//===========================
int pin_LUCE = 2;
String my_status = "SPENTO";

// variables of INTERNET CONNECTION
//===========================
byte myIP[] = { 192, 168, 1, 212 };
byte dns[] = { 192, 168, 1, 1 };
byte gateway[] = { 192, 168, 1, 1 };
byte subnet[] = { 255, 255, 255, 0 };
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// variables of UDP RECEIVER
//===========================
char packetBuffer[7]; //buffer to hold incoming packet,
unsigned int localPort = 5005;      // local port to listen on
String postMessage;


// variables of MQTT CONNECTION
//===========================
const char* my_name = "trivella";
const char* topic = "riserva_idrica/interruttore/trivella";
String MQTT_STATE = "";
unsigned long lastMsg_mqtt = 0;
unsigned long T_error_MQTT = 0;
unsigned long lastConnect_mqtt = 0;
unsigned long T_reconnect_MQTT = 0;


// variables of MULTI DEVICE MANAGER
//===================================
int number_devices = 0;
String devices_status[10] = {};
IPAddress devices_name[10] = {};
unsigned long devices_timestamp[10] = {};

// variables of POWER SENSING
//===================================
String power_status = "SPENTO";
int PinGND = A2;  // sensor Virtual Ground
int PinVClamp = A1;    // Sensor SCT-013-000
float P_soglia = 35;
int power_error = 0;
unsigned long lastMeasure_power = 0;
unsigned long T_measure_POWER = 0;
int Soglia_measure = 1 * 1000;

// variables of MANUAL MANAGEMENT
//==============================
const char* topic_sub = "catalog/riserva_idrica/+";
String modality = "automatico";
String manual_status = "";

// variables of time_count
//===========================
int Soglia_refresh = 10 * 1000;
int Soglia_error = 15 * 1000;
unsigned long now = 0;



void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10); // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(pin_LUCE, OUTPUT);       // LED on pin 2
  pinMode(3, OUTPUT);
  //

  Ethernet.begin(mac, myIP, gateway, subnet);
  delay(1000);
  Udp.begin(localPort);
  delay(1000);
  MQTT_client.setServer(catalog_ip, 1883);
  MQTT_client.setCallback(callback);
  Serial.println("DEVICE INITILIZED!");
}

void loop() {
  now = millis();
  Ethernet.maintain();
  MQTT_client.loop();
  //manage PIN sensor states
  //=====================================================
  if (my_status == "ACCESO") {
    digitalWrite(pin_LUCE, HIGH);
  }
  else {
    digitalWrite(pin_LUCE, LOW);
  }

  //POWER SENSING
  //=====================================================
  T_measure_POWER = now - lastMeasure_power;
  if (T_measure_POWER >= Soglia_measure) {
    lastMeasure_power = now;
    POWER_ERROR_MANAGER();
  }

  //MQTT MANAGER
  //=====================================================
  MQTT_RECONNECT();
  if (MQTT_client.connected()) {
    T_error_MQTT = now - lastMsg_mqtt;
    if (T_error_MQTT >= Soglia_refresh) {
      Serial.println("MQTT REFRESHING");
      MQTT_STATE = "REFRESH";
    }
    MQTT_PUBLISH();
  }

  if (modality == "automatico") {
    if (my_status != "ERROR") {
      //MULTI UDP UDP MANAGER
      //=====================================================
      RECEIVER_FUNCTION();
      check_timestamp();
      // check status of all devices connected to trivella and update trivella status
      //======================================================
      my_status = "SPENTO";
      if (number_devices > 0) {
        for (byte i = 0; i <= number_devices - 1; i += 1) {
          if (String("ACCESO") == devices_status[i]) {
            my_status = "ACCESO";
            break;
          }
        }
      }
    }
  }
  else {
    my_status = manual_status;
  }

}

void RECEIVER_FUNCTION() {
  int packetSize = Udp.parsePacket(); // verifica arrivo pacchetto
  if (packetSize > 5) {
    Udp.read(packetBuffer, 6);
    String comando = String(packetBuffer);
    Serial.print ("RECEIVED  " + comando + " ... ");
    // send a reply, to the IP address and port that sent us the packet we received
    if (comando == "ACCESO" || comando == "SPENTO") {
      IPAddress D_name = Udp.remoteIP();
      Udp.beginPacket(D_name, Udp.remotePort());
      char copy[7];
      comando.toCharArray(copy, 7);
      Udp.write(copy);
      Udp.endPacket();
      Serial.println("REPLIED: " + comando );
      multiDevice_manager(D_name, comando);
      Serial.println("N devices: " + String(number_devices));
    }
  }
}

void multiDevice_manager(IPAddress D_name, String D_status) {
  bool flag = false;
  String Name_ip = String(D_name[0]) + "." + String(D_name[1]) + "." + String(D_name[2]) + "." + String(D_name[3]);
  if (number_devices > 0) {
    for (int i = 0; i <= number_devices - 1; i += 1) {
      if (devices_name[i] == D_name) {
        Serial.println("refresh device number " + String(i + 1));
        flag = true;
        devices_status[i] = D_status;
        devices_timestamp[i] = now;
        //Serial.print("Name: " + Name_ip);
        //Serial.print(", Status: " + devices_status[i]);
        //Serial.println(", Timestamp: " + String(devices_timestamp[i]));
        break;
      }
    }
  }
  if (!flag) {
    Serial.println("aggiungo device " + Name_ip);
    devices_name[number_devices] = D_name;
    devices_status[number_devices] = D_status;
    devices_timestamp[number_devices] = now;
    //Serial.print("Name: " + Name_ip);
    //Serial.print(", Status: " + devices_status[number_devices]);
    //Serial.println(", Timestamp: " + String(devices_timestamp[number_devices]));
    number_devices += 1;
  }
}

void check_timestamp() {
  if (number_devices > 0) {
    for (int i = 0; i <= number_devices - 1; i += 1) {
      unsigned long Ti_error = now - devices_timestamp[i];
      if (Ti_error > Soglia_error) {
        if (devices_status[i] != "ERROR") {
          devices_status[i] = "ERROR";
          IPAddress D_name = devices_name[i];
          String Name_ip = String(D_name[0]) + "." + String(D_name[1]) + "." + String(D_name[2]) + "." + String(D_name[3]);
          //String Name_ip = D_name.toString().c_str();
          Serial.println("Device N " + String(i + 1) + " NOT ONLINE!");
        }
      }
    }
  }
}


void MQTT_PUBLISH() {
  if (MQTT_STATE != my_status) {
    float Power = POWER_SENSING();
    String payload = my_status + "/" + "power_sensing" + "/" + String(Power);
    int payload_length = payload.length();
    if (MQTT_client.publish(topic, (char*) payload.c_str(), payload_length, true)) {
      MQTT_STATE = my_status;
      lastMsg_mqtt = now;
      Serial.println("mqtt messange sent with topic " + String(topic));
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

float POWER_SENSING() {
  //int N_turns = 1800;
  //int R_burden = 62;
  if (power_status != my_status) {
    power_status = my_status;
    delay(200);
  }
  const unsigned int numReadings = 20; //samples to calculate Vrms.
  int readingsVClamp[numReadings];    // samples of the sensor SCT-013-000
  float Power = 0;
  float I = 0;
  float SumSqVClamp = 0;
  float total = 0;
  for (unsigned int i = 0; i < numReadings; i++) {
    readingsVClamp[i] = analogRead(PinVClamp) - analogRead(PinGND);
    delay(1); //
  }
  //Calculate Vrms
  for (unsigned int i = 0; i < numReadings; i++) {
    SumSqVClamp = SumSqVClamp + sq((float)readingsVClamp[i]);
  }
  total = sqrt(SumSqVClamp / numReadings);
  Power = total * float(32.2265);     // Rburden=62 ohms, LBS= 0,004882 V (5/1024)
  // Transformer of 1800 laps (SCT-013-000).
  // 5*220*1800/(62*1024)= 31.187

  I = total * float(0.14648);
  return Power;
}

void POWER_ERROR_MANAGER() {
  float Power = POWER_SENSING();
  if (my_status == "ACCESO") {
    if (Power < P_soglia) {
      power_error = power_error + 1;
    }
    else {
      power_error = 0;
    }
  }
  else {
    if (Power > P_soglia) {
      power_error = power_error + 1;
    }
    else {
      power_error = 0;
    }
  }
  if (power_error > 3) {
    Serial.println("POWER ERROR DETECTED!!");
    my_status = "ERROR";
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
  String msgIn = "";
  for (int i = 0; i < length; i++) {
    msgIn = msgIn + ((char)payload[i]);
  }
  manual_status = msgIn;
  Serial.println("status " + manual_status);


}
