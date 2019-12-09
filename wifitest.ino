#include "WiFi.h"
#include "M5StickC.h"
#include <IOXhop_FirebaseESP32.h>



#include <NTPClient.h>
#include <WiFiUdp.h>


#include <Adafruit_BMP280.h>
#include "SHT20.h"
#include "yunBoard.h"
#include <math.h>
#include "display.h"





const String deviceNumber = "001";
const String deviceName = "M5stickC " + deviceNumber;


 
const char* ssid = "ENGR_IOT";
const char* password =  "tse@iot2018";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

//#define SSID_AP_NAME "BEWIOT"
//#define SSID_AP_PASS "123456789"

#define FIREBASE_HOST "iotcourse-72928.firebaseio.com"
#define FIREBASE_KEY "5VQwd5r4iVpSlXl3sF1pr3OwafzKLMWxHBKU4M9I"

SHT20 sht20;
Adafruit_BMP280 bmp;

uint32_t update_time = 0;
float tmp, hum;
float pressure;
uint16_t light;
extern uint8_t  lightR;
extern uint8_t  lightG;


 
void setup() {
 
  Serial.begin(115200);

  
//WiFi.mode(WIFI_AP_STA);
//  delay(100);
//  WiFi.softAP(SSID_AP_NAME, SSID_AP_PASS);
//  Serial.print("AP IP address: ");
//  Serial.println(WiFi.softAPIP());
//  Serial.println();


 
  WiFi.begin(ssid, password);
 Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());


// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(25200);
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_KEY);
  
// Firebase.setString("/name","temp" );

 
int8_t i,j;
  M5.begin();
  Wire.begin(0, 26, 100000);
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextSize(2);
  
  // RGB888
  // led_set(uint8_t 1, 0x080808);
  
  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
  }

    /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_1000); /* Standby time. */
  
  // put your setup code here, to run once:
  display_light4();

//
//Firebase.setString(deviceNumber + "/name", deviceName);

}

 uint8_t light_flag=0;
 
void loop() {
  


 if(millis() > update_time) {
    update_time = millis() + 1000;
    tmp = sht20.read_temperature();
    hum = sht20.read_humidity();
    light = light_get();
    pressure = bmp.readPressure();}
    M5.Lcd.setCursor(3, 3);
    M5.Lcd.setTextColor(TFT_BLUE, TFT_BLACK);
    M5.Lcd.print("YUN");
    M5.Lcd.setCursor(0, 25);
    M5.Lcd.setTextColor(TFT_RED, TFT_BLACK);
    M5.Lcd.printf("%.2fC\r\n", tmp);
    M5.Lcd.setCursor(0, 25+20);
    M5.Lcd.setTextColor(TFT_RED, TFT_BLACK);
    M5.Lcd.printf("%d", int(hum));
    M5.Lcd.print("%\r\n");
    M5.Lcd.setCursor(0, 25+20+20);
    M5.Lcd.setTextColor(TFT_RED, TFT_BLACK);
    M5.Lcd.printf("%d lux\r\n", int(light));
    //M5.Lcd.printf("%d Pa\r\n", int(pressure));

    M5.update();

  if(M5.BtnA.wasPressed()) {
    esp_restart();  }


while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  
  Serial.println(formattedDate);

  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  Serial.print("DATE: ");
  Serial.println(dayStamp);
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.print("HOUR: ");
  Serial.println(timeStamp);
  delay(1000);

  int Light_Intensity = light;
  float temperature = tmp;
  int Air_Pressure = pressure;


  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& valueObject = jsonBuffer.createObject();
 
  valueObject["temperature"] = temperature;
  valueObject["Light_Intensity"] = Light_Intensity;
  valueObject["Air_Pressure"] = Air_Pressure;
  valueObject["Timestamps"] = formattedDate;
  Firebase.push(deviceNumber + "/value", valueObject);
  delay(10000);
  }


  
