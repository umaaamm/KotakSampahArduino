
#include <ESP8266WiFi.h>          
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          
#include <FirebaseArduino.h>
#include "HX711.h"
#include <EEPROM.h>
#include "simpaneeprom.h"
#define FIREBASE_HOST "kotak-sampah.firebaseio.com"
#define FIREBASE_AUTH "bRyccbh12HrHp3BQe2xrUPyE6E13pk1eAEX125cQ"

HX711 cell(D2,D3);
int tombolreset = D1;
long timertmbl = 0;
long tahan = 1000;
boolean tombolstatus = false;
boolean statustahan = false;
long bacasensor = 0;
int rata1=8464083;
int rata2=-7989;
int berattemp=70;
double hasil,hasilkilo;
int n = 0;
unsigned int alamat = 0;
unsigned int na = 0;
double  hasil2;

const int PIN_LED = 2; 

void setup() {

  pinMode(tombolreset, INPUT);
pinMode(LED_BUILTIN, OUTPUT);
 EEPROM.begin(512);
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
  Serial.println("\n Starting");
  unsigned long startedAt = millis();
  WiFi.printDiag(Serial); 
  Serial.println("Opening configuration portal");
  digitalWrite(PIN_LED, LOW); 
  
  WiFiManager wifiManager;  
 
  if (WiFi.SSID()!="") wifiManager.setConfigPortalTimeout(60); //If no access point name has been previously entered disable timeout.
  
  
  if (!wifiManager.startConfigPortal("Tong Sampah Pintar","IoTpintar")) {
    
     Serial.println("Not connected to WiFi but continuing anyway.");
  } else {
     
     Serial.println("connected...yeey :)");
     }
 digitalWrite(PIN_LED, HIGH); 
    
  Serial.print("After waiting ");
  int connRes = WiFi.waitForConnectResult();
  float waited = (millis()- startedAt);
  Serial.print(waited/1000);
  Serial.print(" secs in setup() connection result is ");
  Serial.println(connRes);

  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");        
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  Serial.println(WiFi.localIP()); 
  digitalWrite(LED_BUILTIN, HIGH);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
   double val;
   int addr = (1*4)+alamat;
   na = baca( addr, val);
  hasil2 = val;
 delay(100); 
 Serial.print("hasil2 dari eeprom= ");
Serial.println(hasil2);
delay(1000);
}


void hitungBerat(){
        bacasensor = cell.read();
        hasil= ( bacasensor  - rata1) / rata2 * berattemp;
        hasil = hasil / 1000;
        hasilkilo = (hasil-hasil2);
  }
  
void kalibrasi(){
 bacasensor= cell.read();
  hasil2= (bacasensor - 8464083)/ -7989 * 70;
  hasil2 = hasil2 / 1000;
  Serial.print("hasil 2= ");
  Serial.println(hasil2);
  na = simpan( (1*4)+alamat,hasil2);
  Serial.print("hasil 2 ke 2= ");
  Serial.println(hasil2);
  delay(1000);

  }
void tombolkalibrasi(){
  if (digitalRead(tombolreset) == HIGH) {

    if (tombolstatus == false) {

      tombolstatus = true;
      timertmbl = millis();

    }

    if ((millis() - timertmbl > tahan) && (statustahan == false)) {

      statustahan = true;
      kalibrasi();
      digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("kalibrasi berhasil!!!!");

    }

  } else {

    if (tombolstatus == true) {

      if (statustahan == true) {

        statustahan = false;

      } else {

        Serial.println("tahan 2 detik untuk kalibrasi");

      }

      tombolstatus = false;

    }

  }
  
  }
  void cekcont(){
    while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");        
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
    digitalWrite(LED_BUILTIN, HIGH);
  }
    }
  void kirim(){
     hitungBerat();
     // set value
  Firebase.setFloat("sensor/berat", hasilkilo); //variablenya ganti data dari sensro
  // handle error
  if (Firebase.failed()) {
      //Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return;
  }
  tombolkalibrasi();
  delay(500);
   // append a new value to /logs
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  JsonObject& tempTime = root.createNestedObject("timestamp");
   tempTime[".sv"] = "timestamp";
  tombolkalibrasi();
  root["berat"] = hasilkilo; //ini isi data dari sensornya
  root["id_kotak_sampah"] = "K_01"; //ini id alatnya 
  String name = Firebase.push("logs",root);
  // handle error
  if (Firebase.failed()) {
      Serial.print("pushing /logs failed:");
      Serial.println(Firebase.error());  
      return;
  }
  Serial.print("pushed: /logs/");
  Serial.println(name);
  tombolkalibrasi();
  delay(1000);
    
    
    }
    

void loop() {
done:
  if(Serial.available()){
        if((Serial.read()) == 'K')
        {
         kalibrasi();
        }
      goto done;
    }
        cekcont();
        tombolkalibrasi();
        hitungBerat();
        kirim();
        Serial.println(hasilkilo);
}
