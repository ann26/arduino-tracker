
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

//Define pins
#define FONA_RX 2
#define FONA_TX 3
#define FONA_KEY 4
#define FONA_RST 5
#define FONA_PS 6

int keyTime = 2000;

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX,FONA_RX);
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
void setup() {
  pinMode(FONA_KEY,OUTPUT);
  pinMode(FONA_PS, INPUT);
  digitalWrite(FONA_KEY,HIGH);
  turnOnFona();
  Serial.begin(115200);
  Serial.println("Fona is starting up");
  fonaSS.begin(4800);

  if (! fona.begin(fonaSS)) {           // can also try fona.begin(Serial1) 
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  Serial.println(F("FONA is OK"));
  char imei[15] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("SIM card IMEI: "); Serial.println(imei);
  }
}

void fonaReset(){
  digitalWrite(FONA_RST,LOW);
  unsigned long KeyPress = millis();
  while(KeyPress + 100 >=millis()){}
  digitalWrite(FONA_RST,HIGH);
  Serial.println("Reset Fona");
  pinMode(FONA_KEY,OUTPUT);
  pinMode(FONA_PS, INPUT);
  digitalWrite(FONA_KEY,HIGH);
  fonaSS.begin(4800);
  if (! fona.begin(fonaSS)) {           // can also try fona.begin(Serial1) 
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  Serial.println(F("FONA is OK"));
  char imei[15] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("SIM card IMEI: "); Serial.println(imei);
  }
}

void turnOnFona(){
  if(!digitalRead(FONA_PS)){
    Serial.println("Device is turning on");
    digitalWrite(FONA_KEY,LOW);
    unsigned long KeyPress = millis();
    while(KeyPress +keyTime >= millis()){}
    digitalWrite(FONA_KEY,HIGH);
    Serial.println("FONA Powered UP");  
  }
  else{
    Serial.println("Fona is already on.");
  }
}

void turnOffFona(){
  if(!digitalRead(FONA_PS)){
    Serial.println("Device is turning off");
    digitalWrite(FONA_KEY,LOW);
    unsigned long KeyPress = millis();
    while(KeyPress +keyTime >= millis()){}
    digitalWrite(FONA_KEY,HIGH);
    Serial.println("FONA Powered Down");  
  }
        else{
          Serial.println("Fona is already off.");
        }
}

void turnOnGPRS(){
  if (!fona.enableGPRS(true)) {Serial.println(F("Failed to turn on"));} 
}

void turnOffGPRS(){
  if (!fona.enableGPRS(false)){Serial.println(F("Failed to turn off"));}
}

void turnOnGPS(){
if (!fona.enableGPS(true)){Serial.println(F("Failed to turn on"));}
}

void turnOffGPS(){
  if (!fona.enableGPS(false)){Serial.println(F("Failed to turn off"));}
}

void readLocation(){
char gpsdata[80];
char * fields = "mode,longitude,latitude,altitude,utctime(yyyymmddHHMMSS),ttff,satellites,speed,course";
       fona.getGPS(0, gpsdata, 80);
       char * pch;
char * mode;
char * longitude;
char * latitude;
char * altitude;
char * utctime;
char * ttff;
char * satellites;
char * speedoftracker;
char * course;
          pch = strtok (gpsdata,",");
          mode = pch;
          pch = strtok (NULL, ",");
          longitude = pch;  
          pch = strtok (NULL, ",");
          latitude = pch;  
          pch = strtok (NULL, ",");
          altitude = pch; 
          pch = strtok (NULL, ",");
          utctime = pch; 
          pch = strtok (NULL, ",");
          ttff = pch; 
          pch = strtok (NULL, ",");
          satellites = pch; 
          pch = strtok (NULL, ",");
          speedoftracker = pch; 
          pch = strtok (NULL, ",");
          course = pch; 
          pch = strtok (NULL, ",");
      
          Serial.print("Mode: ");
          Serial.println(mode);
          Serial.print("Longtitude: ");
          Serial.println(longitude);
          Serial.print("Latitude: ");
          Serial.println(latitude);
          Serial.print("Altitude: ");
          Serial.println(altitude);
          Serial.print("Utctime: ");
          Serial.println(utctime);
          Serial.print("Ttff: ");
          Serial.println(ttff);
          Serial.print("Satellites :");
          Serial.println(satellites);
          Serial.print("Speed :");
          Serial.println(speedoftracker);
          Serial.print("Course :");
          Serial.println(course);
}

void flushSerial() {
    while (Serial.available()) 
    Serial.read();
}

void loop() {
//  Serial.println("Entering loop");
  /*turnOffFona();
  delay(1000);
  turnOnFona();
  delay(1000);
  fonaReset();
  delay(2000);*/
  turnOnGPS();
  delay(2000);
  readLocation();
  
  

}
