/**
 *
 * HX711 library for Arduino - example file
 * https://github.com/bogde/HX711
 *
 * MIT License
 * (c) 2018 Bogdan Necula
 *
**/
#include "HX711.h"


// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 13;
const int LOADCELL_SCK_PIN = 4;

float divider = 31.71735f ;


HX711 scale;

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 Demo");

  Serial.println("Initializing the scale");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println(scale.read());			// print a raw reading from the ADC

}

void loop() {
  long rawRead = scale.read();
  Serial.print("Raw read : ");
  Serial.println(rawRead);			// print a raw reading from the ADC
  Serial.print("Converted read : ");
  float convRead = rawRead/divider;
  Serial.println(convRead , 6);
  delay(200);

}