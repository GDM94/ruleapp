#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

IPAddress catalog_ip(192, 168, 1, 18);
int broker_port = 1883;
WiFiClient client;
PubSubClient MQTT_client(client);


//SETTINGS INFO
//==============================
String my_status = "";
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
const char* ssid = "NETGEAR23";
const char* password = "quaintcoconut226";

// variables of MQTT CONNECTION
//===========================
const char* clientID = "00124";
unsigned long lastConnect_mqtt = 0;
unsigned long T_reconnect_MQTT = 0;

// variables of time_count
//===========================
unsigned long now = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10); // wait for serial port to connect. Needed for native USB port only
  }
  //
  WiFi.begin(ssid, password);
  WIFI_CONNECT();
  MQTT_client.setServer(catalog_ip, 1883);
  MQTT_client.setCallback(callback);
  MQTT_client.connect(clientID);
  Serial.println("DEVICE INITILIZED!");
}

void loop() {
  now = millis();
  WIFI_CONNECT();
  MQTT_client.loop();
  MQTT_RECONNECT();
  FOTOCELLULA();
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
        for (byte j = 0; j < MY_number_sensors; j++) {
          Sensors[j].Status = "reconnect";
        }
      }
      else {
        Serial.println("mqtt try to reconnect...");
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
  String mittente = msg_info[0];
  String S_name = msg_info[1];
  //_________________________________

  for (byte j = 0; j < MY_number_sensors; j++) {
    if (S_name == Sensors[j].Name) {
      Serial.println("Sensor: " + S_name);
      String msgIn;
      for (int j = 0; j < length; j++) {
        msgIn = msgIn + char(payload[j]);
      }
      message_split(const_cast<char*>(msgIn.c_str()), &msg_info[0]);
      if (mittente == "catalog") {
        Serial.println("Catalog message received");
        for (byte f = 0; f < 5; f++) {
          if (msg_info[0] == Sensors[j].fields[f]) {
            Serial.println("field: " + msg_info[0]);
            Serial.println("value: " + msg_info[1]);
            Sensors[j].values[f] = msg_info[1];
            Sensors[j].flag = true;
            break;
          }
        }
      }
      else if (mittente == "syncro") {
        if (msg_info[0] == "desyncro") {
          Serial.println("desyncro message received");
          Sensors[j].User = "";
          Sensors[j].System = "";
          Sensors[j].Object = "";
          Sensors[j].Status = "syncro";
        }
        else {
          Serial.println("syncro message received");
          Sensors[j].User = msg_info[0];
          Sensors[j].System = msg_info[1];
          Sensors[j].Object = msg_info[2];
          Sensors[j].Status = "setup";
        }
      }
      break;
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

void ELETTRO_VALVOLA() {
  String S_name = "RX1-00235";
  int pin_SWITCH = 5;
  int idx_status = 1;
  int flag_syncro = 0;
  for (byte i = 0; i < MY_number_sensors; i++) {
    if (Sensors[i].Name == S_name) {
      flag_syncro = 1;
      if (Sensors[i].Status != "syncro") {
        //RECONNECT--------------------
        if (Sensors[i].Status == "reconnect") {
          String S_topic_syncro = "syncro/" + S_name + "/user/sys/obj";
          String S_topic_catalog = "catalog/" + S_name;
          const char* topic_syncro = S_topic_syncro.c_str();
          const char* topic_catalog = S_topic_catalog.c_str();
          MQTT_client.subscribe(topic_syncro);
          MQTT_client.subscribe(topic_catalog);
        }
        // SETUP---------------------
        if (Sensors[i].Status == "setup") {
          pinMode(pin_SWITCH, OUTPUT);
          Sensors[i].topic = Sensors[i].User + "/" + Sensors[i].System + "/" + Sensors[i].Object + "/" + S_name + "/Status";
        }
        // DUTY-CICLE ----------------------------------
        String S_status = Sensors[i].values[idx_status];
        if (S_status == "ACCESO") {
          digitalWrite(pin_SWITCH, HIGH);
        }
        else {
          digitalWrite(pin_SWITCH, LOW);
        }
        if (S_status == Sensors[i].Status) {
          Sensors[i].const_status = now;
        }
        if (now - Sensors[i].const_status > 500) {
          Sensors[i].flag = true;
        }
        unsigned long Ti_sensor = now - Sensors[i].timestamp;
        if (Ti_sensor > Sensors[i].values[0].toInt() * 1000) {
          Sensors[i].flag = true;
        }
        if (Sensors[i].flag == true) {
          Sensors[i].flag = false;
          Sensors[i].timestamp = now;
          Sensors[i].Status = S_status;
          Sensors[i].payload = S_status;
          MQTT_PUBLISH(i);
        }
      }
      break;
    }
  }
  // SYNCRO-----------------------------------------
  if (flag_syncro == 0) {
    String S_status = "syncro";
    String User_name = "";
    String System = "";
    String Object = "";
    String topic = "";
    String payload = "";
    Sensors[MY_number_sensors] = {User_name, System, Object, S_name, S_status, payload, topic, {"refresh", "status"}, {String(10), "SPENTO"}, now, now, true};
    MY_number_sensors += 1;
    String S_topic_syncro = "syncro/" + S_name + "/user/sys/obj";
    String S_topic_catalog = "catalog/" + S_name;
    const char* topic_syncro = S_topic_syncro.c_str();
    const char* topic_catalog = S_topic_catalog.c_str();
    MQTT_client.subscribe(topic_syncro);
    MQTT_client.subscribe(topic_catalog);
    Serial.println("add device " + S_name + " waiting for syncronization");
  }
}

void FOTOCELLULA() {
  const int pin_SENSOR = A0;
  int pin_LED = 12;
  String S_name = "TR-00F1";
  int idx_soglia_lumen = 1;
  int flag_syncro = 0;
  for (byte i = 0; i < MY_number_sensors; i++) {
    if (Sensors[i].Name == S_name) {
      flag_syncro = 1;
      if (Sensors[i].Status != "syncro") {
        //RECONNECT--------------------
        if (Sensors[i].Status == "reconnect") {
          String S_topic_syncro = "syncro/" + S_name + "/user/sys/obj";
          String S_topic_catalog = "catalog/" + S_name;
          const char* topic_syncro = S_topic_syncro.c_str();
          const char* topic_catalog = S_topic_catalog.c_str();
          MQTT_client.subscribe(topic_syncro);
          MQTT_client.subscribe(topic_catalog);
        }
        // SETUP---------------------
        if (Sensors[i].Status == "setup") {
          Sensors[i].topic = Sensors[i].User + "/" + Sensors[i].System + "/" + Sensors[i].Object + "/" + S_name + "/Status/measure";
          Sensors[i].Status == "SPENTO";
        }
        unsigned long Ti_sensor = now - Sensors[i].timestamp;
        if (Ti_sensor > Sensors[i].values[0].toInt() * 1000) {
          Sensors[i].flag = true;
        }
        if (Sensors[i].flag == true) {
          int soglia = Sensors[i].values[idx_soglia_lumen].toInt();
          String S_status = "SPENTO";
          int lumen = analogRead(pin_SENSOR);
          delay(5);
          if (lumen > soglia) {
            S_status = "ACCESO";
          }
          Sensors[i].flag = false;
          Sensors[i].timestamp = now;
          Sensors[i].Status = S_status;
          Sensors[i].payload = S_status + "/" + String(lumen);
          MQTT_PUBLISH(i);
        }
      }

      break;
    }
  }
  // SYNCRO-----------------------------------------
  if (flag_syncro == 0) {
    String S_status = "syncro";
    String User_name = "";
    String System = "";
    String Object = "";
    String topic = "";
    String payload = "";
    Sensors[MY_number_sensors] = {User_name, System, Object, S_name, S_status, payload, topic, {"refresh", "soglia"}, {String(10), "300"}, now, now, true};
    MY_number_sensors += 1;
    String S_topic_syncro = "syncro/" + S_name + "/user/sys/obj";
    String S_topic_catalog = "catalog/" + S_name;
    const char* topic_syncro = S_topic_syncro.c_str();
    const char* topic_catalog = S_topic_catalog.c_str();
    MQTT_client.subscribe(topic_syncro);
    MQTT_client.subscribe(topic_catalog);
    Serial.println("add device " + S_name + " waiting for syncronization");
  }
}

void SOIL_MOISTURE() {
  const int pin_SENSOR = A0;
  String S_name = "TR-00SM1";
  int idx_soglia = 1;
  int flag_syncro = 0;
  for (byte i = 0; i < MY_number_sensors; i++) {
    if (Sensors[i].Name == S_name) {
      flag_syncro = 1;
      if (Sensors[i].Status != "syncro") {
        //RECONNECT--------------------
        if (Sensors[i].Status == "reconnect") {
          String S_topic_syncro = "syncro/" + S_name + "/user/sys/obj";
          String S_topic_catalog = "catalog/" + S_name;
          const char* topic_syncro = S_topic_syncro.c_str();
          const char* topic_catalog = S_topic_catalog.c_str();
          MQTT_client.subscribe(topic_syncro);
          MQTT_client.subscribe(topic_catalog);
        }
        // SETUP---------------------
        if (Sensors[i].Status == "setup") {
          Sensors[i].topic = Sensors[i].User + "/" + Sensors[i].System + "/" + Sensors[i].Object + "/" + S_name + "/Status/measure";
          Sensors[i].Status == "SPENTO";
        }
        unsigned long Ti_sensor = now - Sensors[i].timestamp;
        if (Ti_sensor > Sensors[i].values[0].toInt() * 1000) {
          Sensors[i].flag = true;
        }
        if (Sensors[i].flag == true) {
          int soglia = Sensors[i].values[idx_soglia].toInt();
          String S_status = "SPENTO";
          int measure = analogRead(pin_SENSOR);
          delay(5);
          if (measure > soglia) {
            S_status = "ACCESO";
          }
          if (S_status == Sensors[i].Status) {
            Sensors[i].const_status = now;
          }
          if (now - Sensors[i].const_status > 2000) {
            Sensors[i].flag = false;
            Sensors[i].timestamp = now;
            Sensors[i].Status = S_status;
            Sensors[i].payload = S_status + "/" + String(measure);
            MQTT_PUBLISH(i);
          }
        }
      }
      break;
    }
  }
  // SYNCRO-----------------------------------------
  if (flag_syncro == 0) {
    String S_status = "syncro";
    String User_name = "";
    String System = "";
    String Object = "";
    String topic = "";
    String payload = "";
    Sensors[MY_number_sensors] = {User_name, System, Object, S_name, S_status, payload, topic, {"refresh", "soglia"}, {String(10), "300"}, now, now, true};
    MY_number_sensors += 1;
    String S_topic_syncro = "syncro/" + S_name + "/user/sys/obj";
    String S_topic_catalog = "catalog/" + S_name;
    const char* topic_syncro = S_topic_syncro.c_str();
    const char* topic_catalog = S_topic_catalog.c_str();
    MQTT_client.subscribe(topic_syncro);
    MQTT_client.subscribe(topic_catalog);
    Serial.println("add device " + S_name + " waiting for syncronization");
  }
}


void WATER_LEVEL() {
  const int trigPin = 16;
  const int echoPin = 5;
  String S_name = "TR-001WL";
  int idx_SogliaOnOff = 0;
  int idx_H = 1;
  int Soglia_measure = 1 * 1000;
  int flag_syncro = 0;
  for (byte i = 0; i < MY_number_sensors; i++) {
    if (Sensors[i].Name == S_name) {
      flag_syncro = 1;
      if (Sensors[i].Status != "syncro") {
        //RECONNECT--------------------
        if (Sensors[i].Status == "reconnect") {
          String S_topic_syncro = "syncro/" + S_name + "/user/sys/obj";
          String S_topic_catalog = "catalog/" + S_name;
          const char* topic_syncro = S_topic_syncro.c_str();
          const char* topic_catalog = S_topic_catalog.c_str();
          MQTT_client.subscribe(topic_syncro);
          MQTT_client.subscribe(topic_catalog);
        }
        // SETUP---------------------
        if (Sensors[i].Status == "setup") {
          Sensors[i].topic = Sensors[i].User + "/" + Sensors[i].System + "/" + Sensors[i].Object + "/" + S_name + "/Status/measure";
          Sensors[i].Status == "SPENTO";
        }
        unsigned long Ti_sensor = now - Sensors[i].timestamp;
        if (Ti_sensor > Sensors[i].values[0].toInt() * 1000) {
          Sensors[i].flag = true;
        }
        if (Sensors[i].flag == true) {
          int soglia_OnOff = Sensors[i].values[idx_SogliaOnOff].toInt();
          int H_max = Sensors[i].values[idx_H].toInt();
          String S_status = "SPENTO";
          //_________________________________
          delay(5);
          digitalWrite(trigPin, LOW);
          delayMicroseconds(1);
          digitalWrite(trigPin, HIGH);
          delayMicroseconds(10);
          digitalWrite(trigPin, LOW);
          long duration = pulseIn(echoPin, HIGH);
          float distance = duration * 0.034 / 2;
          float level_perc = distance / H_max;
          float measure = (1 - level_perc) * 100;
          //___________________________________________
          if (measure > soglia_OnOff) {
            S_status = "SPENTO";
          }
          else {
            S_status = "ACCESO";
          }
          if (S_status == Sensors[i].Status) {
            Sensors[i].const_status = now;
          }
          if (now - Sensors[i].const_status > 3000) {
            Sensors[i].flag = false;
            Sensors[i].timestamp = now;
            Sensors[i].Status = S_status;
            Sensors[i].payload = S_status + "/" + String(measure);
            MQTT_PUBLISH(i);
          }
        }
      }

      break;
    }
  }
  // SYNCRO-----------------------------------------
  if (flag_syncro == 0) {
    String S_status = "syncro";
    String User_name = "";
    String System = "";
    String Object = "";
    String topic = "";
    String payload = "";
    Sensors[MY_number_sensors] = {User_name, System, Object, S_name, S_status, payload, topic, {"refresh", "soglia", "H"}, {String(10), "50", "30"}, now, now, true};
    MY_number_sensors += 1;
    String S_topic_syncro = "syncro/" + S_name + "/user/sys/obj";
    String S_topic_catalog = "catalog/" + S_name;
    const char* topic_syncro = S_topic_syncro.c_str();
    const char* topic_catalog = S_topic_catalog.c_str();
    MQTT_client.subscribe(topic_syncro);
    MQTT_client.subscribe(topic_catalog);
    Serial.println("add device " + S_name + " waiting for syncronization");
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    digitalWrite(trigPin, LOW);
    digitalWrite(echoPin, LOW);
  }
}


void MQTT_PUBLISH(byte i) {
  MQTT_client.publish(const_cast<char*>(Sensors[i].topic.c_str()), const_cast<char*>(Sensors[i].payload.c_str()));
  Serial.println("send sensor messange " + Sensors[i].payload);
}
