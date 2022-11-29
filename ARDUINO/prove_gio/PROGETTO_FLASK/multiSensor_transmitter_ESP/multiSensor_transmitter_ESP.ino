#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//====== TRANSMITTER ===== //
// versione: definitiva
// scheda: Arduino Mega

IPAddress catalog_ip(192, 168, 1, 96);
int broker_port = 1883;
WiFiClient client;
PubSubClient MQTT_client(client);

//SETTINGS INFO
//==============================
String my_status = "SETUP";
String System = "GI";
String Object = "GEBBIA";
int MY_number_sensors = 0;
int MY_number_device = 0;
struct MY_sensor {
  String System;
  String Object;
  String Name;
  String Status;
  String payload;
  String topic;
  String fields[5];
  String values[5];
  long timestamp;
  long const_status;
  bool flag;
};
struct MY_device {
  String System;
  String Object;
  String Name;
  String sensors[5];
  int sensors_number;
  String payload;
  String topic;
  String fields[5];
  String values[5];
  long timestamp;
  bool flag;
};
struct MY_sensor Sensors[3];
struct MY_device Device[2];

// variables of INTERNET CONNECTION
//===========================
const char* ssid = "FASTWEB-D6A39D";
const char* password = "AEF84N31YA";

// variables of MQTT CONNECTION
//===========================
const char* clientID = "3";
const char* topic_sub_catalog = "catalog/System/Object/name/field/value";
unsigned long T_reconnect_MQTT = 0;
unsigned long lastConnect_mqtt = 0;

// variables of SISTEM MANAGEMENT
//==============================
void(* Riavvio)(void) = 0;

// variables of time_count
//===========================
unsigned long now = 0;



void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  FOTOCELLULA();
  WATER_LEVEL();
  //

  WiFi.begin(ssid, password);
  WIFI_CONNECT();
  MQTT_client.setServer(catalog_ip, 1883);
  MQTT_client.connect(clientID);
  MQTT_client.subscribe(topic_sub_catalog);
  MQTT_client.setCallback(callback);
  my_status = "SPENTO";
  Serial.println("Device Initialized!");
}

void loop() {
  now = millis();
  WIFI_CONNECT();
  MQTT_client.loop();
  MQTT_RECONNECT();
  FOTOCELLULA();
  WATER_LEVEL();
  MY_DEVICE_MANAGER();
  mySENSOR_REFRESH();
  CHECK_mySTATUS();
}


void WIFI_CONNECT() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("wifi connecting");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
    Serial.println("CONNECTED!");
  }
}

void MQTT_RECONNECT() {
  if (!MQTT_client.connected()) {
    T_reconnect_MQTT = now - lastConnect_mqtt;
    if (T_reconnect_MQTT > 5000) {
      lastConnect_mqtt = now;
      MQTT_client.connect(clientID);
      if (MQTT_client.state() == 0) {
        Serial.println("MQTT CONNECTED");
        MQTT_client.subscribe(topic_sub_catalog);
      }
      else {
        Serial.println("mqtt try to reconnect...");
      }
    }
  }
}

void CHECK_mySTATUS() {
  my_status = "ACCESO";
  if (MY_number_sensors > 0) {
    for (byte i = 0; i < MY_number_sensors; i ++) {
      if (Sensors[i].Status == "SPENTO") {
        my_status = "SPENTO";
        break;
      }
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
  //read the topic
  int i = 0;
  char* token;
  token = strtok(topic, "/");
  String mittente = String(token);

  if (mittente == "catalog") {
    Serial.println("Catalog message received");
    String msg_info[8] = {};
    message_split(payload, length, &msg_info[0]);
    if (msg_info[1] == System) {
      if (msg_info[2] == Object) {
        if (msg_info[0] == "sensor") {
          for (byte j = 0; j < MY_number_sensors; j++) {
            if (msg_info[3] == Sensors[j].Name) {
              Serial.println("Sensor: " + msg_info[3]);
              for (byte f = 0; f < 5; f++) {
                if (msg_info[4] == Sensors[j].fields[f]) {
                  Serial.println("field: " + msg_info[4]);
                  Serial.println("value: " + msg_info[5]);
                  Sensors[j].values[f] = msg_info[5];
                  Sensors[i].flag = true;
                  break;
                }
              }
              break;
            }
          }
        }
        else if (msg_info[0] == "device") {
          for (byte d = 0; d < MY_number_device; d++) {
            if (msg_info[1] == Device[d].System) {
              if (msg_info[2] == Device[d].Object) {
                if (msg_info[3] == Device[d].Name) {
                  Serial.println("Device: " + msg_info[3]);
                  for (byte f = 0; f < 5; f++) {
                    if (msg_info[4] == Device[d].fields[f]) {
                      Serial.println("field: " + msg_info[4]);
                      Serial.println("value: " + msg_info[5]);
                      Device[d].values[f] = msg_info[5];
                      Device[d].flag = true;
                    }
                  }
                  break;
                }
              }
            }
          }
        }

      }
    }
  }
}

void message_split(byte *payload, unsigned int length, String *msg_info) {
  String msgIn;
  for (int j = 0; j < length; j++) {
    msgIn = msgIn + char(payload[j]);
  }
  //Serial.println(msgIn);
  unsigned int i = 0;
  char *msgCopy = const_cast<char*>(msgIn.c_str());
  char* token;
  while ((token = strtok_r(msgCopy, "/", &msgCopy)) != NULL) {
    msg_info[i] = String(token);
    i = i + 1;
  }
}

void FOTOCELLULA() {
  const int pin_FOTOCELLULA = A0;
  int pin_LED = 12;
  String S_name = "fotocellula";
  int Soglia_measure = 1 * 1000;
  int idx_soglia_lumen = 0;
  if (my_status == "SETUP") {
    pinMode(pin_FOTOCELLULA, INPUT);
    pinMode(pin_LED, OUTPUT);
    String S_status = "SPENTO";
    int soglia_lumen = 300;
    String payload = S_name + "/" + System + "/" + Object + "/" + S_status + "/" + String(0) + "/" + String(soglia_lumen);
    String topic = "sensor/System/Object/Status/luminosty/-soglia";
    Sensors[MY_number_sensors] = {System, Object, S_name, S_status, payload, topic, {"-soglia"}, {"300"}, now, now};
    MY_device_INIT(MY_number_sensors);
    MY_number_sensors += 1;
  }
  else {
    for (byte i = 0; i < MY_number_sensors; i++) {
      if (Sensors[i].Name == S_name) {
        int soglia_lumen = Sensors[i].values[idx_soglia_lumen].toInt();
        String S_status = "SPENTO";
        int lumen = analogRead(pin_FOTOCELLULA);
        delay(5);
        if (lumen > soglia_lumen) {
          digitalWrite(pin_LED, HIGH);
          S_status = "ACCESO";
        }
        else {
          digitalWrite(pin_LED, LOW);
        }
        if (S_status == Sensors[i].Status) {
          Sensors[i].const_status = now;
        }
        if (now - Sensors[i].const_status > 3000) {
          Sensors[i].flag = true;
        }
        if (Sensors[i].flag == true) {
          Sensors[i].flag = false;
          Sensors[i].timestamp = now;
          Sensors[i].Status = S_status;
          String payload = S_name + "/" + System + "/" + Object + "/" + S_status + "/" + String(lumen) + "/" + Sensors[i].values[idx_soglia_lumen];
          Sensors[i].payload = payload;
          MQTT_PUBLISH("sensor", i);
        }
        break;
      }
    }

  }
}

void WATER_LEVEL() {
  String S_name = "water_level";
  const int trigPin = 16;
  const int echoPin = 5;
  int idx_H = 0;
  int idx_SogliaOnOff = 1;
  int Soglia_measure = 1 * 1000;
  if (my_status == "SETUP") {
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    digitalWrite(trigPin, LOW);
    digitalWrite(echoPin, LOW);
    String S_status = "SPENTO";
    String payload =  S_name + "/" + System + "/" + Object + "/" +  S_status + "/" + 0 + "/" + 0 + "/" + 0;
    String topic = "sensor/System/Object/Status/water level(%)/-H max(cm)/-soglia(%)";
    Sensors[MY_number_sensors] = {System, Object,S_name, S_status, payload, topic, {"-H max(cm)", "-soglia(%)", "stability"}, {"30", "50", "0"}, now, 0};
    Serial.println("aggiungo device " + S_name);
    MY_device_INIT(MY_number_sensors);
    MY_number_sensors += 1;
    Serial.println(MY_number_sensors);
  }
  else {
    for (byte i = 0; i < MY_number_sensors; i++) {
      if (Sensors[i].Name == S_name) {
        unsigned long T_measure = now - Sensors[i].const_status;
        if (T_measure >= Soglia_measure) {
          Sensors[i].const_status = now;
          int H_max = Sensors[i].values[idx_H].toInt();
          int soglia_OnOff = Sensors[i].values[idx_SogliaOnOff].toInt();
          int measure_stability = Sensors[i].values[3].toInt();
          String S_status = "SPENTO";
          //__________________________________
          digitalWrite(trigPin, LOW);
          delayMicroseconds(1);
          digitalWrite(trigPin, HIGH);
          delayMicroseconds(10);
          digitalWrite(trigPin, LOW);
          long duration = pulseIn(echoPin, HIGH);
          float distance = duration * 0.034 / 2;
          float level_perc = distance / H_max;
          float water_level = (1 - level_perc) * 100;
          //__________________________________
          if (water_level > soglia_OnOff) {
            S_status = "SPENTO";
          }
          else {
            S_status = "ACCESO";
          }
          if (S_status == Sensors[i].Status) {
            measure_stability = 0;
          }
          else {
            measure_stability += 1;
          }
          if (measure_stability > 3) {
            Sensors[i].flag = true;
          }
          Sensors[i].values[3] = String(measure_stability);
          if (Sensors[i].flag == true) {
            Sensors[i].flag = false;
            Sensors[i].timestamp = now;
            Sensors[i].Status = S_status;
            String payload =  S_name + "/" + System + "/" + Object + "/" +  S_status + "/" + String(water_level) + "/" + Sensors[i].values[idx_H] + "/" + Sensors[i].values[idx_SogliaOnOff];
            Sensors[i].payload = payload;
            MQTT_PUBLISH("sensor", i);
          }
        }
        break;
      }
    }

  }
}

void MY_DEVICE_MANAGER() {
  int idx_soglia_refresh = 1;
  int idx_riavvio = 0;
  for (byte i = 0; i < MY_number_device; i++) {
    String riavvio = Device[i].values[idx_riavvio];
    if (riavvio == "true") {
      riavvio = "false";
      Riavvio();
    }
    if (Device[i].flag == true) {
      Device[i].flag = false;
      Device[i].timestamp = now;
      int Soglia = Device[i].values[idx_soglia_refresh].toInt();
      String payload = String(clientID) + "/" + Device[i].System + "/" + Device[i].Object + "/" + riavvio + "/" + String(Soglia);
      for (byte j = 0; j < Device[i].sensors_number; j++) {
        payload += "/" + Device[i].sensors[j];
      }
      Device[i].payload = payload;
      MQTT_PUBLISH("device", i);

    }
  }
}

void MY_device_INIT(int sensor_idx) {
  bool flag_object = false;
  int Soglia_init=10;
  if (MY_number_device > 0) {
    for (byte i = 0; i < MY_number_device; i++) {
      if (Sensors[sensor_idx].System == Device[i].System) {
        if (Sensors[sensor_idx].Object == Device[i].Object) {
          Device[i].sensors[Device[i].sensors_number] = Sensors[sensor_idx].Name;
          Device[i].sensors_number += 1;
          flag_object = true;
          break;
        }
      }
    }
  }
  if (!flag_object) {
    String riavvio = "false";
    String topic = "device/System/Object/-riavvio/-refresh";
    String payload = "";
    Device[MY_number_device] = {Sensors[sensor_idx].System, Sensors[sensor_idx].Object, String(clientID), {" "}, 0, payload, topic, {"-riavvio", "-refresh"}, {"false", String(Soglia_init)}, now, true};
    Device[MY_number_device].sensors[Device[MY_number_device].sensors_number] = Sensors[sensor_idx].Name;
    Device[MY_number_device].sensors_number+=1;
    MY_number_device += 1;
  }
}


void mySENSOR_REFRESH() {
  int idx_soglia_refresh = 1;
  for (byte i = 0; i < MY_number_device; i++) {
    int Soglia = Device[i].values[idx_soglia_refresh].toInt();
    unsigned long Ti_device = now - Device[i].timestamp;
    if (Ti_device > Soglia * 1000) {
      Device[i].flag = true;
    }
    for (byte j = 0; j < MY_number_sensors; j += 1) {
      if (Sensors[j].System == Device[i].System) {
        if (Sensors[j].Object == Device[i].Object) {
          unsigned long Ti_sensor = now - Sensors[j].timestamp;
          if (Ti_sensor > Soglia * 1000) {
            Sensors[j].flag = true;
          }
        }
      }
    }
  }
}

void MQTT_PUBLISH(String type, byte i) {
  if (type == "sensor") {
    MQTT_client.publish(const_cast<char*>(Sensors[i].topic.c_str()), const_cast<char*>(Sensors[i].payload.c_str()));
    Serial.println("send sensor messange " + Sensors[i].payload);
  }
  else if (type == "device") {
    MQTT_client.publish(const_cast<char*>(Device[i].topic.c_str()), const_cast<char*>(Device[i].payload.c_str()));
    Serial.println("send device messange " + Device[i].payload);
  }
}
