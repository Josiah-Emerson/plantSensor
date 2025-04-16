#include <M5Stack.h>

// Blynk defines
#define BLYNK_TEMPLATE_ID "TMPL2BuAi9s4J"
#define BLYNK_TEMPLATE_NAME "PlantSensor"
#define BLYNK_AUTH_TOKEN "Jp7ohzf2SWILKKpR_AeezyCTjx-36xwc"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp32_SSL.h>

char ssid[] = "APU-BYOD";
char pass[] = "chaffy-intertwining-NyAC";

BlynkTimer timer;

// End of blynk stuff



#define SENSOR_PIN 35
#define PUMP_PIN 17


int rawMoistureADC; // analog input for moisture
// The value was mostly around 1660-1680 when first bought
// Test week or two later to see what it should be at
const int moistureThreshold = 1800; // NOTE: lower means more moisture
int8_t batteryLevel;
bool botanistMode;

// function declarations
void takeMeasurements();
void sendMeasurements();
void pump();
void botanistModeSetup();



// * * * * * SETUP * * * * * 

void setup(){
  delay(1000);
  M5.begin();
  M5.Power.begin();
  M5.Lcd.setTextSize(2);
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println();
  Serial.println("*  *  *  CONNECTED TO WIFI *  *  *");
  delay(2500);

  
  //  *  *  *  *  * M A I N  F U N C I O N A L I T Y  *  *  *  *  *
  botanistModeSetup();
  takeMeasurements();
  sendMeasurements();
  pump();
  

  // *  *  *  *  *  P U T  T O  S L E E P  *  *  *  *  *
  Serial.println("Going to sleep...");
  M5.Lcd.println("Going to sleep...");
  delay(2500);
  uint64_t sleepTime = 3600e6; // in micro seconds
  M5.Power.setWakeupButton(BUTTON_A_PIN);
  M5.Power.deepSleep(sleepTime);
}



void loop() {
 // Nothing in loop because of deep sleep
}





void takeMeasurements(){
  // *  *  *  *  *  M O I S T U R E  &  P U M P  *  *  *  *  *
  pinMode(SENSOR_PIN, INPUT);
  pinMode(PUMP_PIN, OUTPUT);
  
  rawMoistureADC = analogRead(SENSOR_PIN);
  Serial.print("Moisture: ");
  Serial.println(rawMoistureADC);

  // *  *  *  *  *  B A T T E R Y *  *  *  *  *
  batteryLevel = M5.Power.getBatteryLevel();
  Serial.print("Battery: ");
  Serial.println(batteryLevel);
}

void sendMeasurements(){
  // send moisture
  Blynk.virtualWrite(0, rawMoistureADC);

  // send battery level
  Blynk.virtualWrite(2, batteryLevel);

  M5.Lcd.println("* * * Data sent to Blynk * * *");
  Serial.println("* * * Data sent to Blynk * * *");
}

void pump(){
  if(rawMoistureADC > moistureThreshold){ 
    // higher value = drier
    Serial.println("Soil too dry");
    M5.Lcd.println("Soil too dry");
    if(botanistMode){
      M5.Lcd.println("In BotanistMode | No Action Taken");
      Serial.println("In BotanistMode | No Action Taken");
      return;
    }
    
    Serial.println("* * * 5 SEC PUMP * * *");
    M5.Lcd.println("* * * 5 SEC PUMP * * *");
    digitalWrite(PUMP_PIN, true);
    delay(5000);
    digitalWrite(PUMP_PIN, false);
  }else{
    Serial.println("Soil is fine");
    M5.Lcd.println("Soil is fine");
  }

}
void botanistModeSetup(){
  Blynk.syncVirtual(V1);
  Serial.print("Botanist mode: ");
  Serial.println(botanistMode);
}

BLYNK_WRITE(V1){
  if(param.asInt() ==1){
    // Botanist Mode ON
    botanistMode = true;
  }else{
    // Botanist Mode OFF
    botanistMode = false;
  }}

  BLYNK_CONNECTED(){
    Blynk.syncAll();
  }


