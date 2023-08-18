// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPLeqMU7c0X"
#define BLYNK_TEMPLATE_NAME "AR using ESP32"
#define BLYNK_AUTH_TOKEN "uVoQYaUvojsrBPta4c-GrS41eyaReQJX"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "ACS712.h"
#include <DHT.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "vivoY21T";
char pass[] = "Sakthi@123";

#define DHTPIN 33          // Mention the digital pin where you connected 
#define DHTTYPE DHT11     // DHT 11  
DHT dht(DHTPIN, DHTTYPE);

BlynkTimer timer;

#define relay1_pin 26
#define relay2_pin 27

int relay1_state = 0;
int relay2_state = 0;

//Change the virtual pins according the rooms
#define button1_vpin    V1
#define button2_vpin    V2
#define temp_vpin    V5
#define power_vpin    V6

// This function is called every time the device is connected to the Blynk.Cloud
// Request the latest state from the server
BLYNK_CONNECTED() {
  Blynk.syncVirtual(button1_vpin);
  Blynk.syncVirtual(button2_vpin);
}

// This function is called every time the Virtual Pin state change
//i.e when web push switch from Blynk App or Web Dashboard
BLYNK_WRITE(button1_vpin) {
  relay1_state = param.asInt();
  digitalWrite(relay1_pin, relay1_state);
}

BLYNK_WRITE(button2_vpin) {
  relay2_state = param.asInt();
  digitalWrite(relay2_pin, relay2_state);
}

ACS712  ACS(34, 5.0, 4095, 185);

void setup()
{
  // Debug console
  Serial.begin(115200);

  while (!Serial);
  Serial.println(FILE);
  Serial.print("ACS712_LIB_VERSION: ");
  Serial.println(ACS712_LIB_VERSION);

  ACS.autoMidPoint();
  Serial.print("MidPoint: ");
  Serial.print(ACS.getMidPoint());
  Serial.print(". Noise mV: ");
  Serial.println(ACS.getNoisemV());

  pinMode(relay1_pin, OUTPUT);
  pinMode(relay2_pin, OUTPUT);
  //--------------------------------------------------------------------
  //During Starting all Relays should TURN OFF
  digitalWrite(relay1_pin, HIGH);
  digitalWrite(relay2_pin, HIGH);
  //--------------------------------------------------------------------
  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  Blynk.run();
  timer.run();
  float mA;
  for(int i=0;i<20;i++){
    mA = ACS.mA_AC() - 300;
    if (mA<100){
      mA=0;
    }
    else{
      mA+=mA;
    }
  }
  mA/=20;
  mA/=100;
  float P = 220 * mA;
  Blynk.virtualWrite(power_vpin, P);
  Serial.print("Power Consumed: ");
  Serial.print(P);
  Serial.println(" Watts");
  delay(100);

  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  if (t<0){
    t=t*-1;
  }

  Serial.println(t);
  Blynk.virtualWrite(temp_vpin, t);
    Serial.print("Temperature : ");
    Serial.print(t);

    if(t>=40){
      digitalWrite(relay2_pin, 0);
    }
}