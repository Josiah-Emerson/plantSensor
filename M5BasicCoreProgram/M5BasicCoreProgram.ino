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

// must be in microseconds... 1 second = 1e6 us
#define DEEP_SLEEP_DURATION 5e6

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



// * * * * * SETUP * * * * * 

void setup() {
  delay(1000);
  M5.begin();
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);


  // *  *  *  *  *  S L E E P  S E T U P  *  *  *  *  *  *
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  if(wakeup_reason == ESP_SLEEP_WAKEUP_EXT0){
    Serial.println("Woke up by button");
    M5.Lcd.println("Woke up by button");
  }else if(wakeup_reason == ESP_SLEEP_WAKEUP_TIMER){
    Serial.println("Woke up by timer");
    M5.Lcd.println("Woke up by timer");
  }
  
  //  *  *  *  *  * M A I N  F U N C I O N A L I T Y  *  *  *  *  *
  takeMeasurements();
  sendMeasurements();
  pump();
  //botanistModeSetup();
  

  // *  *  *  *  *  P U T  T O  S L E E P  *  *  *  *  *
  Serial.println("Going to sleep...");
  M5.Lcd.println("Going to sleep...");
  delay(2500);
  esp_sleep_enable_timer_wakeup(DEEP_SLEEP_DURATION);
  esp_sleep_enable_ext0_wakeup((gpio_num_t) BUTTON_A_PIN, 0);
  esp_deep_sleep_start();
}


// * * * * * LOOP * * * * * 

void loop() {
  M5.update();
  Blynk.run(); // Blynk should run constantly, but timer only in if for sending measurements
  //TODO - Rename timer to something better

  
  if(M5.BtnA.wasPressed()){
    Serial.println("Button A pressed");
  }
  
  if(M5.BtnB.wasPressed()){
    Serial.println("Button pressed!");

    rawMoistureADC = analogRead(SENSOR_PIN);
    Serial.print("Current reading: ");
    Serial.println(rawMoistureADC);
    delay(25);

    Serial.println("* * * * * SENDING DATA * * * * * ");
    
    timer.run();
    
    Serial.println("* * * * * DONE | DELAYING * * * * * ");
    delay(5000);
  }

  Serial.print("Botanist Mode: ");
  Serial.println(botanistMode);

  rawMoistureADC = analogRead(SENSOR_PIN);
 

  if(rawMoistureADC > moistureThreshold){ 
    // Too dry (Remember higher val is drier from experiments)
    Serial.println("Soil is too dry");   
    if(!botanistMode){
      Serial.println("Pumping for 5 sec... ");
      digitalWrite(PUMP_PIN, true);
      delay(5000);
      digitalWrite(PUMP_PIN, false);
    }else{
      Serial.println("Not pumping (in botanist mode)");
    }
  }

  Serial.println("* * * * * * * * * * * * * * * * * * * * * * * * * *");
  delay(2500);
}





void takeMeasurements(){
  // *  *  *  *  *  M O I S T U R E  &  P U M P  *  *  *  *  *
  pinMode(SENSOR_PIN, INPUT);
  pinMode(PUMP_PIN, OUTPUT);
  
  rawMoistureADC = analogRead(SENSOR_PIN);
  Serial.print("Moisture: ");
  Serial.println(rawMoistureADC);

  // *  *  *  *  *  B A T T E R Y *  *  *  *  *
  M5.Power.begin();
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
  botanistMode = false;
  Blynk.virtualWrite(1, 0);
  // BLYNK_READ (similar to BLYNK_WRITE)
}

BLYNK_WRITE(V1){
  if(param.asInt() ==1){
    // Botanist Mode ON
    botanistMode = true;
  }else{
    // Botanist Mode OFF
    botanistMode = false;
  }

}


