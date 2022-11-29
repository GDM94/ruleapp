#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <timer.h>
//====== TRANSMITTER ===== //
// versione: definitiva
// scheda: Arduino Mega

IPAddress catalog_ip(192, 168, 1, 96);
int broker_port = 1883;
WiFiClient espClient;
PubSubClient MQTT_client(espClient);
auto timer = timer_create_default(); // create a timer with default settings

// variables of SETTING PIN SENSORS
//===========================
int pinLED_red = 5; //D1
int pin_SENSOR = 4; //D2

// variables of INTERNET CONNECTION
//===========================
const char* ssid = "FASTWEB-D6A39D";
const char* password = "AEF84N31YA";

// variables of MQTT CONNECTION
//===========================
const char* my_name = "casotto";
String msgIn;
const char* topic = "/Catalog/Riserva_Idrica";
int MQTT_message = 0;
String my_status = "";
int DEVICE_STATE = 2;

// variables of time_count
//===========================
unsigned long lastMsg = 0;
unsigned long now = 0;
unsigned long T_error = 0;
int Soglia = 10 * 1000;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(pinLED_red, OUTPUT);
  pinMode(pin_SENSOR, INPUT);
  digitalWrite(pin_SENSOR, LOW);
  //
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  delay(1000);
  MQTT_client.setServer(catalog_ip, 1883);
  Serial.println("Device Initialized!");
}

void loop() {
  now = millis();
  T_error = now - lastMsg;
  toggle_led();
  MQTT_client.loop();
  if (MQTT_client.connected()) {
    int SENSOR_STATE = digitalRead(pin_SENSOR);
    if (SENSOR_STATE == 1) {
      my_status = "ACCESO";
      digitalWrite(pinLED_red, HIGH);
    }

    if (SENSOR_STATE == 0) {
      my_status = "SPENTO";
      digitalWrite(pinLED_red, LOW);
    }

    if (T_error >= Soglia) {
      Serial.println("REFRESHING");
      MQTT_message = 1;
    }
    if (DEVICE_STATE != SENSOR_STATE) {
      DEVICE_STATE = SENSOR_STATE;
      MQTT_message = 1;
    }
    if (MQTT_message == 1) {
      MQTT_PUBLISH();
    }
  }
  else {
    Serial.println("DEVICE OFFLINE, trying to establish connession...");
    MQTT_client.connect(my_name);
    if (MQTT_client.connected()) {
      Serial.println("CONNECTED!");
    }
  }
}

String SETTING_FILE() {
  // create REGISTRATION JSON FILE
  // action: node = send registration request
  //         refresh = send refresh request
  // ======================================================================
  String postMessage = "";
  const size_t capacity = JSON_OBJECT_SIZE(3) + 70;
  StaticJsonDocument<capacity> doc;
  doc["Name"] = my_name;
  doc["Status"] = my_status;
  doc["type"] = "galleggiante";
  // Serialize JSON document
  serializeJson(doc, postMessage);
  return postMessage;
}


void MQTT_PUBLISH() {
  String payload = SETTING_FILE();
  Serial.println(payload);
  if (MQTT_client.publish(topic, (char*) payload.c_str())) {
    MQTT_message = 0;
    lastMsg = now;
    Serial.println("mqtt messange sent!");
  }

}


bool time_count() {
  T_error += 1;
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
