#include <M5Stack.h>

// pin defines
#define SENSOR_PIN 35
#define PUMP_PIN 17

// global variables
int rawMoistureADC; // analog input for moisture


void setup() {
  // put your setup code here, to run once:
  M5.begin();
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  rawMoistureADC = analogRead(SENSOR_PIN);
  Serial.print("Soil Moisture Level: ");
  Serial.println(rawMoistureADC);
  Serial.println("");
}
