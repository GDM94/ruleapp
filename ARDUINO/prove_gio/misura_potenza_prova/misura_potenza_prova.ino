#include <SPI.h>

const unsigned int numReadings = 100; //samples to calculate Vrms.
int rmsVClamp[numReadings];    // samples of the sensor SCT-013-000
int rmsGND[numReadings];      // samples of the <span id="result_box" class="" lang="en"><span class="hps">virtual</span> <span class="hps alt-edited">ground</span></span>
float SumSqGND = 0;
float SumSqVClamp = 0;
float total = 0;
float Power = 0;
float I = 0;
int PinGND = A2;
int PinVClamp = A1;    // Sensor SCT-013-000
int N_turns = 1800;
int R_burden = 62;
//int PinVirtGND = A1;   // <span id="result_box" class="" lang="en"><span class="hps">Virtual</span> <span class="hps alt-edited">ground</span></span>

// variables of SETTING PIN SENSOR
//===========================
int pin_LUCE = 2;

void setup() {
  Serial.begin(9600);
  // initialize all the readings to 0:
  pinMode(pin_LUCE, OUTPUT);
}

void loop() {
  digitalWrite(pin_LUCE, HIGH);
  /*
  float readingsVClamp = 0;
  for (unsigned int i = 0; i < numReadings; i++) {
    readingsVClamp += sq((float)(analogRead(PinVClamp) - analogRead(PinGND)));
    delay(1);
  }
  float total = sqrt(abs(readingsVClamp / (float)numReadings));
  
  Power = abs(total * float(31.187));                // Rburden=3300 ohms, LBS= 0,004882 V (5/1024)
  // Transformer of 2000 laps (SCT-013-000).
  // 5*220*2000/(3300*1024)= 2/3 (aprox)
  //(R=62; N turns=1800;)-->31.187
  //I = total * float(0.14176);
  
  //Serial.println("Current: "+String(I)+" A");
  */
  
  int Power = power_measure();
  
  Serial.println(Power);
  delay(1500);

}

int power_measure() {
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
    SumSqVClamp += sq((float)readingsVClamp[i]);
  }
  total = sqrt(SumSqVClamp / numReadings);
  I = total * float(0.14648);
  float Power_float = total * float(31.187);
  int Power = (int) Power_float;
  if (Power <= 12){ // error management
    Power = 0;
  }
  return Power;
  // Rburden=62 ohms, LBS= 0,004882 V (5/1024)
  // Transformer of 1800 laps (SCT-013-000).
  // 5*220*1800/(62*1024)= 31.187
}
