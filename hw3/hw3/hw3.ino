/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#include "Ultrasonic.h"
#define BLYNK_TEMPLATE_ID "TMPL6GyiHCD6k"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "YPmayRqUbYncFzgt7ZSq7TyEn9NysnRC"


/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include "DHT20.h"


DHT20 DHT;
#include <rpcWiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleWioTerminal.h>
Ultrasonic ultrasonic(0);

// Your WiFi credentials.
// Set password to "" for open networks.
// notice, DO NOT usd iphone as the hotspot!
char ssid[] = "iPhone de Miggy";
char pass[] = "c#esgodd";


BlynkTimer timer;
uint8_t count = 0;

BLYNK_WRITE(V6) //buzzer
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if (pinValue == 1){
     tone(WIO_BUZZER, random(500,1500), 500);
     Serial.println("1");

  } 
  // process received value
}
BLYNK_WRITE(V8) //buzzer
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
analogWrite(8, pinValue);


  
  // process received value
}
// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent() {
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  if (millis() - DHT.lastRead() >= 1000) {
    //  READ DATA
    uint32_t start = micros();
    int status = DHT.read();
    uint32_t stop = micros();


    if ((count % 10) == 0) {
      count = 0;
      Serial.println();
      Serial.println("Type\tHumidity (%)\tTemp (°C)\tTime (µs)\tStatus");
    }
    count++;


    Serial.print("DHT20 \t");
    //  DISPLAY DATA, sensor has only one decimal.
    Serial.print(DHT.getHumidity(), 1);
    Serial.print("\t\t");
    Serial.print(DHT.getTemperature(), 1);
    Serial.print("\t\t");
    Serial.print(stop - start);
    Serial.print("\t\t");
    Blynk.virtualWrite(V4, DHT.getTemperature());
    Blynk.virtualWrite(V5, DHT.getHumidity());
    switch (status) {
      case DHT20_OK:
        Serial.print("OK");
        break;
      case DHT20_ERROR_CHECKSUM:
        Serial.print("Checksum error");
        break;
      case DHT20_ERROR_CONNECT:
        Serial.print("Connect error");
        break;
      case DHT20_MISSING_BYTES:
        Serial.print("Missing bytes");
        break;
      case DHT20_ERROR_BYTES_ALL_ZERO:
        Serial.print("All bytes read zero");
        break;
      case DHT20_ERROR_READ_TIMEOUT:
        Serial.print("Read time out");
        break;
      case DHT20_ERROR_LASTREAD:
        Serial.print("Error read too fast");
        break;
      default:
        Serial.print("Unknown error");
        break;
    }
    Serial.print("\n");
  }
  Serial.println("updated");
  delay(2000);
}


void setup() {
  pinMode(WIO_BUZZER, OUTPUT);
int LED = 8;
  pinMode(LED, OUTPUT);
  
    
  // Debug console
  Serial.begin(115200);
  Wire.begin();
  DHT.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("connected");
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);


  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
  //  ESP32 default pins 21 22
  delay(1000);
}


void loop() {
   long RangeInCentimeters;
   RangeInCentimeters = ultrasonic.read(CM);
   if (RangeInCentimeters<5){
    analogWrite(8, 255);
   } 
  Blynk.run();
  timer.run();  // Initiates BlynkTimer
  //Serial.println("...");
}
