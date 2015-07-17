#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"
#include "ArduinoJson.h"
//Define pins
#define FONA_RX 2
#define FONA_TX 3
#define FONA_KEY 4
#define FONA_RST 5
#define FONA_PS 6

int keyTime = 2000;
int counter = 0; //turns off fona if 60 or greater
unsigned long ATtimeOut = 10000;
String response;

char newdata [80];
char olddata [80];
char temp [80];


SoftwareSerial fonaSS = SoftwareSerial(FONA_TX,FONA_RX);
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

//sets up the arduino device
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
    Serial.print("SIM card IMEI: "); 
    Serial.println(imei);
  }
  delay(2000);
  turnOnGPS();
  turnOnGPRS();
  delay(2000);
  strcpy(newdata,readLocation());
  strcpy(temp,newdata);
}

//Hard reset and redefining of pins
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
  delay(2000);
  turnOnGPS();
  delay(2000);
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
    turnOffGPRS();
    turnOffGPS();
    delay(2000);
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

char * readLocation(){
    char gpsdata[80];
    char gpsdata_copy[80];
    fona.getGPS(0, gpsdata, 80);
    strcpy(gpsdata_copy, gpsdata);
    Serial.println(gpsdata_copy);
    return gpsdata_copy;
}
char * readGSMLocation(){ //Requires gprs to work
uint16_t returncode;
char replybuffer [256];   
       if (!fona.getGSMLoc(&returncode, replybuffer, 250))
         Serial.println(F("Failed!"));
       if (returncode == 0) {
         Serial.println(replybuffer);
       } else {
         Serial.print(F("Fail code #")); Serial.println(returncode);
       }
       return replybuffer;
       
}
//Determines if device has been in the same location for a while
boolean sameLocation(char newdata[], char olddata[]){
    boolean check = false;
    char my_newdata[80];
    strcpy(my_newdata,newdata);
    char my_olddata[80]; 
    strcpy(my_olddata,olddata);
  char * pch = strtok (my_newdata,",");
  pch = strtok (NULL, ",");
  char * newlong = pch;  
  pch = strtok (NULL, ",");
  char * newlat = pch;  
  pch = strtok (NULL, ",");
  char * newalt = pch;

  char * oldbreak = strtok (my_olddata,",");
   oldbreak = strtok (NULL, ",");
  char * oldlong = oldbreak;  
   oldbreak = strtok (NULL, ",");
  char * oldlat = oldbreak;  
   oldbreak = strtok (NULL, ",");
  char * oldalt = oldbreak; 
  
  
  //*Insert code for checking movement
  double oldlatnum = atof(oldlat);
  double newlatnum = atof(newlat);
  double delta = 0.0005;  
  if(abs(oldlatnum-newlatnum)<=delta ){check=true;}
  return check;
}

void flushSerial() {
    while (Serial.available()) 
    Serial.read();
}

void httprequest(char *httpurl){
  uint16_t statuscode;
  int16_t length;
  char url[80];
  strcpy(url,"<Enter URL here>");
  char data[80];
  //Serial.println(url);
  //Serial.println(data);
  strcpy(data,httpurl);
       if (!fona.HTTP_POST_start(url, F("text/plain"), (uint8_t *) data, strlen(data), &statuscode, (uint16_t *)&length)) {
         Serial.println("Failed!");
       }
       Serial.println(length);
       fona.HTTP_POST_end();
}

char * parseJson(){
    char send_data [80];
    strcpy(send_data,newdata);
    char * devide = strtok(send_data,",");
    char * mode = devide;
    devide = strtok(NULL,",");
    char * latitude = devide;
    devide = strtok(NULL,",");
    char * longitude = devide;
    devide = strtok(NULL,",");
    char * altitude = devide;
    devide = strtok(NULL,",");
    char * date = devide;
    devide = strtok(NULL,",");
    char * numofsatellites = devide;
    devide = strtok(NULL,",");
    char * speeding = devide;
    devide = strtok(NULL,",");
    char * course = devide;
    
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    
    root["mode"]= mode;
    root["latitude"]=latitude;
    root["longitude"]=longitude;
    root["altitude"]=altitude;
    root["date"]=date;
    root["numofsatellites"]=numofsatellites;
    root["speed"]=speeding;
    root["course"]=course;
    root.printTo(Serial);
    char buffer [256];
    root.printTo(buffer,sizeof(buffer));
    char * json =buffer;
    return json;
}

void loop() {
    strcpy(olddata,temp);
    strcpy(newdata,readLocation());
    strcpy (temp,newdata);
      if(!sameLocation(newdata, olddata)){
        counter++;
        Serial.println(counter);
        if(counter >= 60){
          turnOffGPS();
          turnOffFona();
          delay(60000);
          turnOnFona();
          delay(1000);
          fonaReset();
        }
      }
      else{
        counter = 0;
        char * json = parseJson();
        httprequest(json);
        //Send data to server
      }
  delay(1000); //Checks location every second
  
}
