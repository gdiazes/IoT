#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

#include <Espalexa.h>

#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15

const char* ssid = "989826008_Wi-Fi5";
const char* password = "-";

Espalexa espAlexa;

void Funcion_D1(uint8_t brightness);
void Funcion_D2(uint8_t brightness);
void Funcion_D3(uint8_t brightness);
void Funcion_D4(uint8_t brightness);
void Funcion_D5(uint8_t brightness);
void Funcion_D6(uint8_t brightness);
void Funcion_D7(uint8_t brightness);
void Funcion_D8(uint8_t brightness);


void setup() {
  Serial.begin(115200);
  pinMode(D0,OUTPUT);
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
  pinMode(D3,OUTPUT);
  pinMode(D4,OUTPUT);
  pinMode(D5,OUTPUT); 
  pinMode(D6,OUTPUT);
  pinMode(D7,OUTPUT); 
  pinMode(D8,OUTPUT);
  digitalWrite(D0,HIGH);
  digitalWrite(D1,HIGH);
  digitalWrite(D2,HIGH);
  digitalWrite(D3,HIGH);
  digitalWrite(D4,HIGH);
  digitalWrite(D5,HIGH);
  digitalWrite(D6,HIGH);
  digitalWrite(D7,HIGH);
  digitalWrite(D8,HIGH);    
  ConectarWifi();
  espAlexa.addDevice("D1", Funcion_D1);
  espAlexa.addDevice("D2", Funcion_D2);
  espAlexa.addDevice("D3", Funcion_D3);
  espAlexa.addDevice("D4", Funcion_D4);
  espAlexa.addDevice("D5", Funcion_D5);
  espAlexa.addDevice("D6", Funcion_D6);
  espAlexa.addDevice("D7", Funcion_D7);
  espAlexa.addDevice("D8", Funcion_D8);
  espAlexa.begin();
}
///////////////////// VOID LOOP ////////////////////////////////////////////////////
void loop(){
  ConectarWifi();
  espAlexa.loop();
  delay(1);}
   
/////////////////// FUNCION PARA CONECTAR EL WIFI ///////////////////////////////////
void ConectarWifi() {
  if(WiFi.status() != WL_CONNECTED) {
     WiFi.mode(WIFI_STA);
     WiFi.begin(ssid, password);
     Serial.println("");
     Serial.println("Connecting to WiFi");
     while(WiFi.status() != WL_CONNECTED) {
           digitalWrite(D0, HIGH);
           delay(500);
           Serial.print(".");}
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(D0, LOW);}}

/////////////////// FUNCION PARA EL PIN D1 /////////////////////////////
void Funcion_D1(uint8_t brightness){
     if(brightness){digitalWrite(D1, LOW);}
     else{digitalWrite(D1, HIGH);}}
/////////////////// FUNCION PARA EL PIN D2 /////////////////////////////
void Funcion_D2(uint8_t brightness){
     if(brightness){digitalWrite(D2, LOW);}
     else{digitalWrite(D2, HIGH);}}
/////////////////// FUNCION PARA EL PIN D3 /////////////////////////////
void Funcion_D3(uint8_t brightness){
     if(brightness){digitalWrite(D3, LOW);}
     else{digitalWrite(D3, HIGH);}}
/////////////////// FUNCION PARA EL PIN D4 /////////////////////////////
void Funcion_D4(uint8_t brightness){
     if(brightness){digitalWrite(D4, LOW);}
     else{digitalWrite(D4, HIGH);}}
/////////////////// FUNCION PARA EL PIN D5 /////////////////////////////
void Funcion_D5(uint8_t brightness){
     if(brightness){digitalWrite(D5, LOW);}
     else{digitalWrite(D5, HIGH);}}
/////////////////// FUNCION PARA EL PIN D6 /////////////////////////////
void Funcion_D6(uint8_t brightness){
     if(brightness){digitalWrite(D6, LOW);}
     else{digitalWrite(D6, HIGH);}}
/////////////////// FUNCION PARA EL PIN D7 /////////////////////////////
void Funcion_D7(uint8_t brightness){
     if(brightness){digitalWrite(D7, LOW);}
     else{digitalWrite(D7, HIGH);}}
/////////////////// FUNCION PARA EL PIN D8 /////////////////////////////
void Funcion_D8(uint8_t brightness){
     if(brightness){digitalWrite(D8, LOW);}
     else{digitalWrite(D8, HIGH);}}          
