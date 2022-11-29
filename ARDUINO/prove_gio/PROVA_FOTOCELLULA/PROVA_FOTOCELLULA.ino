#include <SPI.h>
#include <Ethernet.h>

int pin_FOTOCELLULA = A0;
int pin_LED=2;

void setup() {
  Serial.begin(9600);
  pinMode(pin_FOTOCELLULA, INPUT);
  pinMode(pin_LED, OUTPUT);
}

void loop() {


  delay(1000);
}

void FOTOCELLULA() {
  int lumen = analogRead(pin_FOTOCELLULA);
  if (lumen > 300) {
    digitalWrite(pin_LED, HIGH);
  }
  else {
    digitalWrite(pin_LED, LOW);
  }
}
