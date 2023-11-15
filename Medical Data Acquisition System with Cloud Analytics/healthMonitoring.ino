#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include "MAX30100.h"
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#define REPORTING_PERIOD_MS     1000
const char* ssid = "Kirtik";   // your network SSID (name) 
const char* password = "aaaaaaaa";   // your network password

WiFiClient  client;
MAX30100 maxim;
unsigned long myChannelNumber = 1;
const char * myWriteAPIKey = "H21GRYKEUWCSO421";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 15000;
#define DHTPIN 2

#define DHTTYPE    DHT11     


PulseOximeter pox;

uint32_t tsLastReport = 0;

DHT_Unified dht(DHTPIN, DHTTYPE);

float temperatureC;
float humidity;
float pulse;
float spo2;
float motion;

void onBeatDetected()
{
    Serial.println("");
}
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);

    Serial.print("Initializing pulse oximeter..");
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }

   pox.setOnBeatDetectedCallback(onBeatDetected);
     dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
   
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);
  sensor_t sensor;
 if(WiFi.status() != WL_CONNECTED){
  Serial.print("Attempting to connect");
  while(WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid, password); 
    delay(5000);     
  } 
  Serial.println("\nConnected.");
 }

 }


void loop() {
  // put your main code here, to run repeatedly:
  
  pox.update();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        Serial.print("Heart rate:");
        
        pulse = pox.getHeartRate();
        Serial.print(pulse);
        Serial.print("bpm / SpO2:");
        spo2 = pox.getSpO2();
        Serial.print(spo2);
        Serial.println("%");

        tsLastReport = millis();
        
    }


  if ((millis() - lastTime) > timerDelay) {
    
  // Get temperature event and print its value.
  sensors_event_t event;
    // Asynchronously dump heart rate and oxidation levels to the serial
    // For both, a value of 0 means "invalid"
    
      dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    temperatureC = event.temperature;
    Serial.println(F("°C"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    humidity= event.relative_humidity;
    Serial.println(F("%"));
  }
       

      lastTime = millis();
      ThingSpeak.setField(1, pulse);
      ThingSpeak.setField(2, spo2);
      ThingSpeak.setField(3, temperatureC);
      ThingSpeak.setField(4, humidity);
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    maxim.resetFifo();
    if(x == 200){
      Serial.println(" Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    
  
    }


}

