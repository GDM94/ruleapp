#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
//====== TRANSMITTER ===== //
// versione: definitiva
// scheda: Arduino Mega

IPAddress catalog_ip(192, 168, 1, 96);
int broker_port = 1883;
EthernetClient client;
PubSubClient MQTT_client(client);

//SETTINGS INFO
//==============================
int soglia = 300;
String type = "recipiente";
String my_status = "SETUP";
String System = "GESTIONE IDRICA";
String Object = "RECIPIENTE GEBBIA";
String MY_Sensors_name[5] = {};
String MY_Sensor_status[5] = {};
long MY_Sensor_timestamp[5] = {};
String MY_Sensor_payload[5] = {};
String MY_Sensor_topic[5] = {};
int MY_number_sensors = 0;

// variables of INTERNET CONNECTION
//===========================
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF };

// variables of MQTT CONNECTION
//===========================
const char* clientID = "0001";
String msgIn;
String MQTT_status = "";
unsigned long T_reconnect_MQTT = 0;
unsigned long lastConnect_mqtt = 0;

// variables of time_count
//===========================
unsigned long lastMsg = 0;
unsigned long now = 0;
unsigned long T_error = 0;
int Soglia = 10 * 1000;
int Soglia_error = 15 * 1000;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  FOTOCELLULA();
  Ethernet.begin(mac);
  delay(1000);
  MQTT_client.setServer(catalog_ip, 1883);
  MQTT_client.connect(clientID);
  my_status = "SPENTO";
  Serial.println("Device Initialized!");
}

void loop() {
  now = millis();
  MQTT_client.loop();
  Ethernet.maintain();
  MQTT_RECONNECT();
  FOTOCELLULA();
  mySENSOR_REFRESH();
  CHECK_mySTATUS();

}


void CHECK_mySTATUS() {
  my_status = "ACCESO";
  if (MY_number_sensors > 0) {
    for (byte i = 0; i < MY_number_sensors; i ++) {
      if (MY_Sensor_status[i] == "SPENTO") {
        my_status = "SPENTO";
        break;
      }
    }
  }
}

void MQTT_RECONNECT() {
  if (!MQTT_client.connected()) {
    Serial.println("MQTT DISCONNECTED");
    T_reconnect_MQTT = now - lastConnect_mqtt;
    if (T_reconnect_MQTT > 5000) {
      lastConnect_mqtt = now;
      MQTT_client.connect(clientID);
      if (MQTT_client.state() == 0) {
        Serial.println("MQTT CONNECTED");
      }
      else {
        Serial.println("mqtt try to reconnect...");
        //my_status = "SPENTO";
      }
    }
  }
}

void MYSensor_manager(String S_name, String S_status, String payload, String topic) {
  bool flag = false;
  if (MY_number_sensors > 0) {
    for (byte i = 0; i < MY_number_sensors; i += 1) {
      if (MY_Sensors_name[i] == S_name) {
        //Serial.println("refresh sensor " + S_name);
        flag = true;
        if (MY_Sensor_status[i] != S_status) {
          MY_Sensor_timestamp[i] = now;
          MY_Sensor_status[i] = S_status;
          MY_Sensor_payload[i] = payload;
          MY_Sensor_topic[i] = topic;
          MQTT_PUBLISH(S_name);
        }
        break;
      }
    }
  }
  if (!flag) {
    Serial.println("aggiungo device " + S_name);
    MY_Sensors_name[MY_number_sensors] = S_name;
    MY_Sensor_status[MY_number_sensors] = S_status;
    MY_Sensor_timestamp[MY_number_sensors] = now;
    MY_Sensor_payload[MY_number_sensors] = payload;
    MY_Sensor_topic[MY_number_sensors] = topic;
    MQTT_PUBLISH(S_name);
    MY_number_sensors += 1;
  }
}


void FOTOCELLULA() {
  int pin_FOTOCELLULA = A0;
  int pin_LED = 2;
  if (my_status == "SETUP") {
    pinMode(pin_FOTOCELLULA, INPUT);
    pinMode(pin_LED, OUTPUT);
  }
  else {
    String S_name = "fotocellula";
    String S_status = "SPENTO";
    int lumen = analogRead(pin_FOTOCELLULA);
    if (lumen > soglia) {
      digitalWrite(pin_LED, HIGH);
      S_status = "ACCESO";
    }
    else {
      digitalWrite(pin_LED, LOW);
    }
    String topic = "sensor/System/Object/Status/luminosty/-soglia ON-OFF";
    String payload = S_name + "/" + System + "/" + Object + "/" +  S_status+"/"+lumen+"/"+soglia;
    MYSensor_manager(S_name, S_status, payload, topic);
  }
}


/*
  void GALLEGGIANTE_MANAGER() {
  String S_name = "galleggiante";
  int GALLEGGIANTE_STATE = digitalRead(pin_GALLEGGIANTE);
  String S_status = "SPENTO";
  if (GALLEGGIANTE_STATE == 1) {
    S_status = "ACCESO";
  }

  String payload = S_name + "/" + System + "/" + Object + "/" +  S_status;
  MYSensor_manager(S_name, S_status, payload);
  }
*/

void MQTT_PUBLISH(String S_name) {
  if (MQTT_client.connected()) {
    for (byte i = 0; i < MY_number_sensors; i += 1) {
      if (MY_Sensors_name[i] == S_name) {
        String payload = MY_Sensor_payload[i];
        String topic = MY_Sensor_topic[i];
        int payload_length = payload.length();
        MQTT_client.publish(const_cast<char*>(topic.c_str()), const_cast<char*>(payload.c_str()));
        Serial.println("mqtt messange" + payload);
      }
    }
  }
}


void mySENSOR_REFRESH() {
  if (MY_number_sensors > 0) {
    for (byte i = 0; i < MY_number_sensors; i += 1) {
      unsigned long Ti_error = now - MY_Sensor_timestamp[i];
      if (Ti_error > Soglia) {
        MY_Sensor_timestamp[i] = now;
        Serial.println("Sensor " + MY_Sensors_name[i] + " REFRESH");
        MQTT_PUBLISH(MY_Sensors_name[i]);
      }
    }
  }
}
