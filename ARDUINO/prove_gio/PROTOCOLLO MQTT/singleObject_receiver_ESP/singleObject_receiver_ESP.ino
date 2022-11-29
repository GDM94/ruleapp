#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

IPAddress catalog_ip(192, 168, 1, 96);
int broker_port = 1883;
WiFiClient client;
PubSubClient MQTT_client(client);


//SETTINGS INFO
//==============================
String my_status = "SETUP";
String System = "g.i.";
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
const char* clientID = "1";
const char* topic_sensor = "sensor/System/Object/Status/#";
const char* topic_device = "device/System/Object/-riavvio/-refresh";
const char* topic_sub_catalog = "catalog/System/Object/name/field/value";
unsigned long lastConnect_mqtt = 0;
unsigned long T_reconnect_MQTT = 0;


// variables of MULTI DEVICE MANAGER
//===================================
int number_sensors = 0;
int number_objects = 0;
struct multi_sensor {
  String Name;
  String Object;
  String Status;
  long timestamp;
  int refresh;
};
struct object {
  String Name;
  String Status;
};
struct multi_sensor MultiSensor[6];
struct object Objects[3];

// variables of SWITCH MANAGEMENT
//==============================
String modality = "automatico";
String manual_status = "";

// variables of SISTEM MANAGEMENT
//==============================
void(* Riavvio)(void) = 0;

// variables of time_count
//===========================
//int Soglia = 10;
unsigned long now = 0;



void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10); // wait for serial port to connect. Needed for native USB port only
  }
  GALLEGGIANTE_MANAGER();
  ELETTRO_VALVOLA();
  //

  WiFi.begin(ssid, password);
  WIFI_CONNECT();
  MQTT_client.setServer(catalog_ip, 1883);
  MQTT_client.connect(clientID);
  MQTT_client.subscribe(topic_sensor);
  MQTT_client.subscribe(topic_device);
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
  if (modality == "automatico") {
    CHECK_mySTATUS();
    check_MultiSensors_status();
  }
  else {
    my_status = manual_status;
  }
  ELETTRO_VALVOLA();
  GALLEGGIANTE_MANAGER();
  MY_DEVICE_MANAGER();
  mySENSOR_REFRESH();
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
        MQTT_client.subscribe(topic_sensor);
        MQTT_client.subscribe(topic_device);
        MQTT_client.subscribe(topic_sub_catalog);
      }
      else {
        Serial.println("mqtt try to reconnect...");
      }
    }
  }
}

void CHECK_mySTATUS() {
  my_status = "SPENTO";
  if (number_objects > 0) {
    for (byte d = 0; d < number_objects; d++) {
      if (Objects[d].Status == "ACCESO") {
        my_status = "ACCESO";
        break;
      }
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
  //read the topic
  //Serial.println(String(topic));
  int i = 0;
  char* token;
  token = strtok(topic, "/");
  String mittente = String(token);

  //_________________________________
  if (mittente == "sensor") {
    String msg_info[8] = {};
    message_split(payload, length, &msg_info[0]);
    bool flag = true;
    if (msg_info[0] == "valvola") {
      flag = false;
    }
    if (flag) {
      if (msg_info[1] == System) {
        if (msg_info[2] == Object) {
          String S_name = msg_info[0];
          Serial.println("received message from " + S_name);
          String D_name = msg_info[2];
          S_name = S_name + "_" + D_name;
          String S_status = msg_info[3];
          multiSensor_manager(S_name, D_name, S_status);
        }
      }
    }
  }
  //_________________________________
  if (mittente == "device") {
    String msg_info[8] = {};
    message_split(payload, length, &msg_info[0]);
    if (msg_info[1] == System) {
      if (msg_info[2] == Object) {
        int soglia_refresh = msg_info[4].toInt();
        for (byte i = 5; i < 8; i++) {
          String S_name = msg_info[i] + "_" + msg_info[2];
          for (byte j = 0; j < number_sensors; j++) {
            if (MultiSensor[j].Name == S_name) {
              MultiSensor[j].refresh = soglia_refresh;
              break;
            }
          }
        }
      }
    }
  }
  //_________________________________
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
                  Sensors[j].flag = true;
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

void multiSensor_manager(String S_name, String D_name, String S_status) {
  bool flag_sensor = false;
  if (number_sensors > 0) {
    for (byte i = 0; i < number_sensors; i ++) {
      if (MultiSensor[i].Name == S_name) {
        flag_sensor = true;
        MultiSensor[i].timestamp = now;
        if (S_status !=  MultiSensor[i].Status) {
          Serial.println("refresh sensor " + S_name);
          MultiSensor[i].Status = S_status;
          multiObject_manager(D_name);
        }
        break;
      }
    }
  }
  if (!flag_sensor) {
    Serial.println("aggiungo sensor " + S_name);
    MultiSensor[number_sensors].Name = S_name;
    MultiSensor[number_sensors].Status = S_status;
    MultiSensor[number_sensors].Object = D_name;
    MultiSensor[number_sensors].timestamp = now;
    MultiSensor[number_sensors].refresh = 10;
    number_sensors += 1;
    multiObject_manager(D_name);
  }
}

void multiObject_manager(String D_name) {
  bool flag_object = false;
  if (number_objects > 0) {
    for (byte idx = 0; idx < number_objects; idx++) {
      if (Objects[idx].Name == D_name) {
        flag_object = true;
        Objects[idx].Status = "ACCESO";
        for (byte j = 0; j < number_sensors; j++) {
          if (MultiSensor[j].Object == D_name) {
            if (MultiSensor[j].Status == "SPENTO") {
              Objects[idx].Status = "SPENTO";
              break;
            }
            Serial.println("sensor " + MultiSensor[j].Name + " status: " + MultiSensor[j].Status);
          }
        }
        Serial.println("Object status " + Objects[idx].Status);
        break;
      }
    }
  }
  if (!flag_object) {
    Serial.println("aggiungo device " + D_name);
    Objects[number_objects].Name = D_name;
    Objects[number_objects].Status = "SPENTO";
    number_objects += 1;
  }
}


void check_MultiSensors_status() {
  for (byte j = 0; j < number_sensors; j++) {
    unsigned long Ti_error = now - MultiSensor[j].timestamp;
    if (Ti_error > (MultiSensor[j].refresh + 10) * 1000) {
      if (MultiSensor[j].Status != "SPENTO") {
        MultiSensor[j].Status = "SPENTO";
        Serial.println("Sensor " + MultiSensor[j].Name + " not online");
        multiObject_manager(MultiSensor[j].Object);
      }
    }
  }
}

void ELETTRO_VALVOLA() {
  String S_name = "valvola";
  int pin_SWITCH = 5;
  int idx_modality = 0;
  int idx_status = 1;
  if (my_status == "SETUP") {
    pinMode(pin_SWITCH, OUTPUT);
    String S_status = "SPENTO";
    String payload = S_name + "/" + System + "/" + Object + "/" +  S_status + "/" + modality + "/" + S_status;
    String topic = "sensor/System/Object/Status/-modality/-status";
    Sensors[MY_number_sensors] = {System, Object, S_name, S_status, payload, topic, {"-modality", "-status"}, {"automatico", S_status}, now, now, true};
    MY_device_INIT(MY_number_sensors);
    MY_number_sensors += 1;
    Serial.println("aggiungo device " + S_name);
    Serial.println(MY_number_sensors);
  }
  else {
    for (byte i = 0; i < MY_number_sensors; i++) {
      if (Sensors[i].Name == S_name) {
        String S_status;
        String payload;
        if (modality != Sensors[i].values[idx_modality]) {
          modality = Sensors[i].values[idx_modality];
          my_status = Sensors[i].values[idx_status];
        }
        if (modality == "manuale") {
          S_status = Sensors[i].values[idx_status];
          if (S_status == "ACCESO") {
            my_status = "ACCESO";
            digitalWrite(pin_SWITCH, HIGH);
          }
          else {
            my_status = "SPENTO";
            digitalWrite(pin_SWITCH, LOW);
          }
          payload = S_name + "/" + System + "/" + Object + "/" +  S_status + "/" + modality + "/" + S_status;
        }
        else {
          S_status = "ACCESO";
          if (my_status == "ACCESO") {
            digitalWrite(pin_SWITCH, HIGH);
          }
          else {
            digitalWrite(pin_SWITCH, LOW);
            S_status = "SPENTO";
          }
          payload = S_name + "/" + System + "/" + Object + "/" +  S_status + "/" + modality + "/" + S_status;
        }
        if (S_status == Sensors[i].Status) {
          Sensors[i].const_status = now;
        }
        if (now - Sensors[i].const_status > 500) {
          Sensors[i].flag = true;
        }
        if (Sensors[i].flag == true) {
          Sensors[i].flag = false;
          Sensors[i].timestamp = now;
          Sensors[i].Status = S_status;
          Sensors[i].payload = payload;
          MQTT_PUBLISH("sensor", i);
        }
        break;
      }
    }
  }
}

void GALLEGGIANTE_MANAGER() {
  const int pin_GALLEGGIANTE = 4;
  String S_name = "galleggiante";
  String Object_local="GEBBIA";
  String System_local="g.i.";
  if (my_status == "SETUP") {
    pinMode(pin_GALLEGGIANTE, INPUT);
    String S_status = "SPENTO";
    String payload = S_name + "/" + System_local + "/" + Object_local + "/" +  S_status;
    String topic = "sensor/System/Object/Status/value/soglia";
    Sensors[MY_number_sensors] = {System_local, Object_local, S_name, S_status, payload, topic, {""}, {""}, now, now, true};
    MY_device_INIT(MY_number_sensors);
    MY_number_sensors += 1;
    Serial.println("aggiungo device " + S_name);
    Serial.println(MY_number_sensors);
  }
  else {
    for (byte i = 0; i < MY_number_sensors; i++) {
      if (Sensors[i].Name == S_name) {
        int GALLEGGIANTE_STATE = digitalRead(pin_GALLEGGIANTE);
        String S_status = "SPENTO";
        if (GALLEGGIANTE_STATE == 1) {
          S_status = "ACCESO";
        }
        if (S_status == Sensors[i].Status) {
          Sensors[i].const_status = now;
        }
        if ((now - Sensors[i].const_status) > 500) {
          Sensors[i].flag = true;
        }
        if (Sensors[i].flag == true) {
          Sensors[i].flag = false;
          String payload = S_name + "/" + System_local + "/" + Object_local + "/" +  S_status;
          Sensors[i].timestamp = now;
          Sensors[i].Status = S_status;
          Sensors[i].payload = payload;
          MQTT_PUBLISH("sensor", i);
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
    Device[MY_number_device] = {Sensors[sensor_idx].System, Sensors[sensor_idx].Object, String(clientID), {" "}, 0, payload, topic, {"-riavvio", "-refresh"}, {"false", String(10)}, now, true};
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
