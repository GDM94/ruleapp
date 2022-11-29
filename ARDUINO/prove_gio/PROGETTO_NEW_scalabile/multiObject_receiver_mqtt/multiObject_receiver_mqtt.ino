#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

IPAddress catalog_ip(192, 168, 1, 96);
int broker_port = 1883;
EthernetClient client;
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
  String fields[7];
  String values[7];
  long timestamp;
  long const_status;
  bool flag;
};
struct MY_sensor Sensors[3] = {};

// variables of INTERNET CONNECTION
//===========================
byte mac[] = { 0x90, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// variables of MQTT CONNECTION
//===========================
String client_unique_ID = "CL0001";
const char* clientID = client_unique_ID.c_str();
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

  Ethernet.begin(mac);
  delay(1000);
  MQTT_client.setServer(catalog_ip, 1883);
  MQTT_client.setCallback(callback);
  MQTT_client.connect(clientID);
  Serial.println("DEVICE INITIALIZED");
}

void loop() {
  now = millis();
  Ethernet.maintain();
  MQTT_client.loop();
  MQTT_RECONNECT();
  SWITCH_MANAGER();
  mySENSOR_REFRESH();
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
  String msg_info[10] = {};
  int i = 0;
  message_split(topic, &msg_info[0]);
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
      char* msgCopy = msgIn.c_str();
      message_split(msgCopy, &msg_info[0]);
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
  //Serial.println(msgIn);
  unsigned int i = 0;
  char* token;
  while ((token = strtok_r(msgCopy, "/", &msgCopy)) != NULL) {
    msg_info[i] = String(token);
    i = i + 1;
  }
}

void SWITCH_MANAGER() {
  String S_name = "RX2-A0001";
  int pin_SWITCH = 2;
  int idx_status = 1;
  int idx_modality = 2;
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
        // SETUP-----------------------
        if (Sensors[i].Status == "setup") {
          pinMode(pin_SWITCH, OUTPUT);
          Sensors[i].topic = Sensors[i].User + "/" + Sensors[i].System + "/" + Sensors[i].Object + "/" + S_name + "/Status";
          Sensors[i].Status = "SPENTO";
          Serial.println("setup sensor " + S_name + " with topic " +  Sensors[i].topic);
        }
        // DUTY CICLE --------------------
        String S_status = Sensors[i].values[idx_status];
        my_status = S_status;
        if (S_status == "ACCESO") {
          digitalWrite(pin_SWITCH, HIGH);
        }
        else {
          digitalWrite(pin_SWITCH, LOW);
        }
        if (Sensors[i].Status == S_status) {
          Sensors[i].const_status = now;
        }
        if (now - Sensors[i].const_status > 500) {
          Sensors[i].flag = true;
          Sensors[i].Status = S_status;
        }
        POWER_ERROR_MANAGER(i);
        if (Sensors[i].flag == true) {
          Sensors[i].flag = false;
          Sensors[i].timestamp = now;
          MQTT_PUBLISH(i);
        }
      }
      break;
    }
  }
  // SYNCRO --------------------
  if (flag_syncro == 0) {
    String User_name = "";
    String System = "";
    String Object = "";
    String S_status = "syncro";
    String payload = "";
    String topic_S = "";
    Sensors[MY_number_sensors] = {User_name, System, Object, S_name, S_status, payload, topic_S, {"refresh", "status", "soglia", "power_error", "error_corto"}, {String(10), "SPENTO", "35", String(0), String(0)}, now, now, true};
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

void POWER_ERROR_MANAGER(byte i) {
  int idx_soglia_ON_OFF = 2;
  int idx_power_error = 3;
  int idx_error_corto = 4;
  int T_measure = 1 * 1000;
  // DUTY CICLE--------------------------------------
  unsigned long T_measure_POWER = now - Sensors[i].const_status;
  int soglia_OnOff = Sensors[i].values[idx_soglia_ON_OFF].toInt();
  int soglia_Corto = soglia_OnOff + (soglia_OnOff/2);
  int power_error = Sensors[i].values[idx_power_error].toInt();
  int error_corto = Sensors[i].values[idx_error_corto].toInt();
  if (Sensors[i].flag == true) {
    delay(300);
  }
  String S_status = "SPENTO";
  int Power = POWER_SENSING();
  if (Power < soglia_Corto) {
    error_corto = 0;
    if (Power > soglia_OnOff) {
      S_status = "ACCESO";
    }
    else {
      S_status = "SPENTO";
      Power = 0;
    }
    if (S_status != Sensors[i].Status) {
      power_error += 1;
    }
    else {
      power_error = 0;
    }
  }
  else {
    error_corto += 1;
  }
  if (error_corto > 3) {
    S_status = "ERROR";
  }
  if (power_error > 3) {
    S_status = "SPENTO";
  }
  Sensors[i].values[idx_power_error] = String(power_error);
  Sensors[i].values[idx_error_corto] = String(error_corto);
  Sensors[i].Status = S_status;
  Sensors[i].payload = S_status;
}

int POWER_SENSING() {
  int PinGND = A2;  // sensor Virtual Ground
  int PinVClamp = A1;    // Sensor SCT-013-000
  const unsigned int numReadings = 200; //samples to calculate Vrms.
  int readingsVClamp[numReadings];    // samples of the sensor SCT-013-000
  float I = 0;
  float SumSqVClamp = 0;
  float total = 0;
  for (unsigned int i = 0; i < numReadings; i++) {
    readingsVClamp[i] = analogRead(PinVClamp) - analogRead(PinGND);
    delay(1); //
  }
  delay(5);
  //Calculate Vrms
  for (unsigned int i = 0; i < numReadings; i++) {
    SumSqVClamp = SumSqVClamp + sq((float)readingsVClamp[i]);
  }
  total = sqrt(SumSqVClamp / numReadings);
  I = total * float(0.14648);
  float Power_float = total * float(31.187);
  int Power = (int) Power_float;
  return Power;
  // Rburden=62 ohms, LBS= 0,004882 V (5/1024)
  // Transformer of 1800 laps (SCT-013-000).
  // 5*220*1800/(62*1024)= 31.187
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
