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
String System = "GI";
String Object = "GEBBIA";
String User_name = "Gio";
int soglia_refresh = 10;
int MY_number_sensors = 0;
int MY_number_device = 0;
struct MY_sensor {
  String User;
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
struct MY_sensor Sensors[3];

// variables of INTERNET CONNECTION
//===========================
const char* ssid = "FASTWEB-D6A39D";
const char* password = "AEF84N31YA";

// variables of MQTT CONNECTION
//===========================
const char* clientID = "10023";
String S_topic_sensor = User_name + "/" + System + "/" + Object + "/#";
const char* topic = S_topic_sensor.c_str();
//String S_topic_sub_catalog = "catalog/" + User_name + "/" + System + "/" + Object + "/name/field/value";
//const char* topic_sub_catalog = S_topic_sub_catalog.c_str();

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

// variables of time_count
//===========================
unsigned long now = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10); // wait for serial port to connect. Needed for native USB port only
  }
  ELETTRO_VALVOLA();
  FOTOCELLULA();
  //
  WiFi.begin(ssid, password);
  WIFI_CONNECT();
  MQTT_client.setServer(catalog_ip, 1883);
  MQTT_client.setCallback(callback);
  MQTT_client.connect(clientID);
  MQTT_client.subscribe(topic);
  my_status = "SPENTO";
  Serial.println("DEVICE INITILIZED!");
}

void loop() {
  now = millis();
  WIFI_CONNECT();
  MQTT_client.loop();

  MQTT_RECONNECT();
  if (modality == "automatico") {
    if (my_status != "ERROR") {
      check_MultiSensors_status();
      CHECK_mySTATUS();
    }
  }
  else {
    my_status = manual_status;
  }
  ELETTRO_VALVOLA();
  FOTOCELLULA();
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
        MQTT_client.subscribe(topic);
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

void callback(char* topic, byte *payload, unsigned int length) {
  //read the topic
  //Serial.println(String(topic));
  int i = 0;
  String msg_info[8] = {};
  message_split(const_cast<char*>(topic), &msg_info[0]);
  String mittente = msg_info[3];
  String D_name = msg_info[2];
  Serial.println(mittente);

  //_________________________________
  if (mittente == "sensor") {
    String msgIn;
    for (int j = 0; j < length; j++) {
      msgIn = msgIn + char(payload[j]);
    }
    message_split(const_cast<char*>(msgIn.c_str()), &msg_info[0]);
    if (D_name == Object) {
      if (msg_info[0] != "valvola") {
        String S_name = msg_info[0];
        S_name = S_name + "_" + D_name;
        String S_status = msg_info[1];
        int S_refresh = msg_info[2].toInt();
        multiSensor_manager(S_name, D_name, S_status, S_refresh);
      }
    }
  }

  //_________________________________
  if (mittente == "catalog") {
    if (D_name == Object) {
      Serial.println("Catalog message received");
      String msgIn;
      for (int j = 0; j < length; j++) {
        msgIn = msgIn + char(payload[j]);
      }
      message_split(const_cast<char*>(msgIn.c_str()), &msg_info[0]);
      for (byte j = 0; j < MY_number_sensors; j++) {
        if (msg_info[0] == Sensors[j].Name) {
          Serial.println("Sensor: " + msg_info[0]);
          for (byte f = 0; f < 5; f++) {
            if (msg_info[1] == Sensors[j].fields[f]) {
              Serial.println("field: " + msg_info[1]);
              Serial.println("value: " + msg_info[2]);
              Sensors[j].values[f] = msg_info[2];
              Sensors[j].flag = true;
              break;
            }
          }
          break;
        }
      }
    }

  }
}

void message_split(char* msgCopy, String *msg_info) {
  unsigned int i = 0;
  char* token;
  while ((token = strtok_r(msgCopy, "/", &msgCopy)) != NULL) {
    msg_info[i] = String(token);
    i = i + 1;
  }
}

void multiSensor_manager(String S_name, String D_name, String S_status, int S_refresh) {
  bool flag_sensor = false;
  if (number_sensors > 0) {
    for (byte i = 0; i < number_sensors; i ++) {
      if (MultiSensor[i].Name == S_name) {
        flag_sensor = true;
        MultiSensor[i].timestamp = now;
        MultiSensor[i].refresh = S_refresh;
        Serial.println("refresh sensor " + S_name);
        if (S_status !=  MultiSensor[i].Status) {
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
    MultiSensor[number_sensors].Status = "init";
    MultiSensor[number_sensors].Object = D_name;
    MultiSensor[number_sensors].timestamp = now;
    MultiSensor[number_sensors].refresh = S_refresh;
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


void ELETTRO_VALVOLA() {
  String S_name = "valvola";
  int pin_SWITCH = 5;
  int idx_modality = 2;
  int idx_status = 1;
  int DB_flag = 1;
  if (my_status == "SETUP") {
    pinMode(pin_SWITCH, OUTPUT);
    String S_status = "SPENTO";
    String topic = User_name + "/" + System + "/" + Object + "/sensor/Status/-refresh/-status/-modality/db";
    String payload = S_name + "/" +  S_status + "/" + String(soglia_refresh) + "/" + S_status + "/" + modality + "/" + String(DB_flag);
    Sensors[MY_number_sensors] = {User_name, System, Object, S_name, S_status, payload, topic, {"-refresh", "-status", "-modality"}, {String(soglia_refresh), S_status, "automatico"}, now, now, true};
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
          payload = S_name + "/" +  S_status + "/" + Sensors[i].values[0] + "/" + S_status + "/" + modality + "/" + String(DB_flag);
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
          payload = S_name + "/" +  S_status + "/" + Sensors[i].values[0] + "/" + S_status + "/" + modality + "/" + String(DB_flag);
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
          MQTT_PUBLISH(i);
        }
        break;
      }
    }
  }
}

void FOTOCELLULA() {
  const int pin_FOTOCELLULA = A0;
  int pin_LED = 12;
  String S_name = "fotocellula";
  int idx_soglia_lumen = 1;
  int DB_flag = 2;
  if (my_status == "SETUP") {
    pinMode(pin_FOTOCELLULA, INPUT);
    pinMode(pin_LED, OUTPUT);
    String S_status = "SPENTO";
    int soglia_lumen = 300;
    String topic = User_name + "/" + System + "/" + Object + "/sensor/Status/-refresh/luminosty/-soglia/db";
    String payload = S_name + "/" +  S_status + "/" + String(soglia_refresh) + "/" + String(0) + "/" + String(soglia_lumen) + "/" + String(DB_flag);
    Sensors[MY_number_sensors] = {User_name, System, Object, S_name, S_status, payload, topic, {"-refresh", "-soglia"}, {String(soglia_refresh), "300"}, now, now, true};
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
          String payload = S_name + "/" +  S_status + "/" + Sensors[i].values[0] + "/" + String(lumen) + "/" + Sensors[i].values[idx_soglia_lumen] + "/" + String(DB_flag);
          Sensors[i].payload = payload;
          MQTT_PUBLISH(i);
        }
        break;
      }
    }

  }
}

void mySENSOR_REFRESH() {
  int idx_soglia_refresh = 1;
  for (byte j = 0; j < MY_number_sensors; j += 1) {
    unsigned long Ti_sensor = now - Sensors[j].timestamp;
    if (Ti_sensor > Sensors[j].values[0].toInt() * 1000) {
      Sensors[j].flag = true;
    }
  }
}


void MQTT_PUBLISH(byte i) {
  MQTT_client.publish(const_cast<char*>(Sensors[i].topic.c_str()), const_cast<char*>(Sensors[i].payload.c_str()));
  Serial.println("send sensor messange " + Sensors[i].payload);
}
