#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

IPAddress catalog_ip(192, 168, 1, 96);
int broker_port = 1883;
EthernetClient client;
PubSubClient MQTT_client(client);


//SETTINGS INFO
//==============================
String my_status = "SETUP";
String System = "GI";
String Object = "TRIVELLA";
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
struct MY_sensor Sensors[3] = {};

// variables of INTERNET CONNECTION
//===========================
byte mac[] = { 0x90, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// variables of MQTT CONNECTION
//===========================
const char* clientID = "5005";
String S_topic_sensor = User_name + "/" + System + "/#";
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
struct multi_sensor MultiSensor[6] = {};
struct object Objects[3] = {};


// variables of SWITCH MANAGEMENT
//==============================
String modality = "automatico";
String manual_status = "";

// variables of SISTEM MANAGEMENT
//==============================
void(* Riavvio)(void) = 0;
int power_error = 0;
int error_corto = 0;
// variables of time_count
//===========================
unsigned long now = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10); // wait for serial port to connect. Needed for native USB port only
  }
  SWITCH_MANAGER();
  POWER_ERROR_MANAGER();
  //
  Ethernet.begin(mac);
  delay(1000);
  MQTT_client.setServer(catalog_ip, 1883);
  MQTT_client.setCallback(callback);
  MQTT_client.connect(clientID);
  MQTT_client.subscribe(topic);
  my_status = "SPENTO";
  Serial.println("DEVICE INITILIZED!");
}

void loop() {
  now = millis();
  Ethernet.maintain();
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
  SWITCH_MANAGER();
  POWER_ERROR_MANAGER();
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

void callback(char* topic, byte *payload, unsigned int length) {
  //read the topic
  //Serial.println(String(topic));
  int i = 0;
  String msg_info[8] = {};
  message_split(topic, &msg_info[0]);
  String mittente = msg_info[3];
  String D_name = msg_info[2];

  //_________________________________
  if (mittente == "sensor") {
    if (D_name != Object) {
      String msgIn;
      for (int j = 0; j < length; j++) {
        msgIn = msgIn + char(payload[j]);
      }
      char* msgCopy = msgIn.c_str();
      message_split(msgCop, &msg_info[0]);
      String S_name = msg_info[0];
      S_name = S_name + "_" + D_name;
      String S_status = msg_info[1];
      int S_refresh = msg_info[2].toInt();
      multiSensor_manager(S_name, D_name, S_status, S_refresh);
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
      char* msgCopy = msgIn.c_str();
      message_split(msgCop, &msg_info[0]);
      for (byte j = 0; j < MY_number_sensors; j++) {
        if (msg_info[0] == Sensors[j].Name) {
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
  //Serial.println(msgIn);
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
          }
        }
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
    if (Ti_error > (MultiSensor[j].refresh * 1000) + 5000) {
      MultiSensor[j].timestamp = now;
      Serial.println("Sensor " + MultiSensor[j].Name + " not online");
      if (MultiSensor[j].Status != "SPENTO") {
        MultiSensor[j].Status = "SPENTO";
        multiObject_manager(MultiSensor[j].Object);
      }
    }
  }
}


void SWITCH_MANAGER() {
  String S_name = "interruttore";
  int pin_SWITCH = 2;
  int idx_status = 1;
  int idx_modality = 2;
  int DB_flag = 0;
  if (my_status == "SETUP") {
    pinMode(pin_SWITCH, OUTPUT);
    String S_status = "SPENTO";
    String topic = User_name + "/" + System + "/" + Object + "/sensor/Status/-refresh/-status/-modality/db";
    String payload = S_name + "/" +  S_status + "/" + String(soglia_refresh) + "/" + S_status + "/" + modality + "/" + String(DB_flag);
    Sensors[MY_number_sensors] = {User_name, System, Object, S_name, S_status, payload, topic, {"-refresh", "-status", "-modality"}, {String(soglia_refresh), S_status, "automatico"}, now, now};
    MY_number_sensors += 1;
    Serial.println("aggiungo device " + S_name);
  }
  else {
    for (byte i = 0; i < MY_number_sensors; i++) {
      if (Sensors[i].Name == S_name) {
        String S_status = "SPENTO";
        String payload = "";
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
        if (Sensors[i].Status == S_status) {
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

void POWER_ERROR_MANAGER() {
  String S_name = "amperometro";
  int idx_soglia_ON_OFF = 1;
  int idx_soglia_Corto = 2;
  int idx_power_error = 3;
  int T_measure = 1 * 1000;
  int DB_flag = 1;
  if (my_status == "SETUP") {
    String S_status = "SPENTO";
    String topic = User_name + "/" + System + "/" + Object + "/sensor/Status/-refresh/power(W)/-soglia(W)/-corto(W)/db";
    String payload =  S_name + "/" +  S_status + "/" + String(soglia_refresh) + "/" + 0 + "/" + 0 + "/" + 0 + "/" + String(DB_flag);
    String fields[5] = {"-refresh", "-soglia(W)", "-corto(W)", "power_error"};
    String values[5] = {String(soglia_refresh), "35", "100", String(0)};
    Sensors[MY_number_sensors] = {User_name, System, Object, S_name, S_status, payload, topic, {"-refresh", "-soglia(W)", "-corto(W)", "power_error"}, {String(soglia_refresh), "35", "100", String(0)}, now, now, true};
    Serial.println("aggiungo device " + S_name);
    MY_number_sensors += 1;
  }
  else {
    for (byte i = 0; i < MY_number_sensors; i++) {
      if (Sensors[i].Name == S_name) {
        unsigned long T_measure_POWER = now - Sensors[i].const_status;
        if (T_measure_POWER >= T_measure || Sensors[i].flag == true) {
          int soglia_OnOff = Sensors[i].values[idx_soglia_ON_OFF].toInt();
          int soglia_Corto = Sensors[i].values[idx_soglia_Corto].toInt();
          power_error = Sensors[i].values[idx_power_error].toInt();
          if (Sensors[i].Status != my_status || Sensors[i].flag == true) {
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
            if (S_status != my_status) {
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
            my_status = "ERROR";
            S_status = "ERROR";
          }
          if (power_error > 3) {
            S_status = "ERROR";
          }
          Sensors[i].values[idx_power_error] = String(power_error);
          if (S_status != Sensors[i].Status) {
            Sensors[i].flag = true;
          }
          if (Sensors[i].flag == true) {
            Sensors[i].flag = false;
            Sensors[i].timestamp = now;
            Sensors[i].Status = S_status;
            String payload = S_name + "/" +  S_status + "/" + String(Sensors[i].values[0]) + "/" + String(Power) + "/" + Sensors[i].values[idx_soglia_ON_OFF] + "/" + Sensors[i].values[idx_soglia_Corto] + "/" + String(DB_flag);
            Sensors[i].payload = payload;
            MQTT_PUBLISH(i);
          }
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
