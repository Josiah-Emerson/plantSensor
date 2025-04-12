#include <M5Stack.h>

// Blynk defines
#define BLYNK_TEMPLATE_ID "TMPL2BuAi9s4J"
#define BLYNK_TEMPLATE_NAME "PlantSensor"
#define BLYNK_AUTH_TOKEN "Jp7ohzf2SWILKKpR_AeezyCTjx-36xwc"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp32_SSL.h>

char ssid[] = "Josiah Phone";
char pass[] = "josiahE1";

BlynkTimer timer;


// moisture
#define SENSOR_PIN 35
#define PUMP_PIN 17
int rawMoistureADC; // analog input for moisture

// function declarations
void my_timer();

void setup() {
  M5.begin();
  Serial.begin(9600);
  delay(25);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(5000L, my_timer);

  // setup moisture
  pinMode(SENSOR_PIN, INPUT);
  pinMode(PUMP_PIN, OUTPUT);
}

void loop() {
  rawMoistureADC = analogRead(SENSOR_PIN);
  Blynk.run();
  timer.run();
}

BLYNK_WRITE(V1){
  if(param.asInt() == 1){
    // turn pump on
    digitalWrite(PUMP_PIN, true);
  }else{
    // turn off pump
    digitalWrite(PUMP_PIN, false);
  }
}

void my_timer(){
  Blynk.virtualWrite(0, rawMoistureADC);
  Serial.println("\t* * * sent data to Blynk cloud");
}
