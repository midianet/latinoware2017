#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <DHT.h>

#define dhtType  DHT11
#define pinDHT    16
#define pinRed    15
#define pinGreen  13
#define pinBlue   14
#define pinYellow 12

const char* MYSSID = "ESP8266AP";
const char* MYPASS = "0000"; 

Adafruit_SSD1306 display(0);
ESP8266WebServer server(80);

/**** DTH 11 *********************************************************************/
DHT dht(pinDHT, dhtType);
static char celsiusTemp[7];
static char humidityTemp[7];
/********************************************************************************/

void flagAll(int flag){
  digitalWrite(pinRed   ,flag);
  digitalWrite(pinGreen ,flag);
  digitalWrite(pinYellow,flag);
  digitalWrite(pinBlue  ,flag);
}

void readTemp(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    strcpy(celsiusTemp,"Failed");
    strcpy(humidityTemp, "Failed");   
  }else{
    float hic = dht.computeHeatIndex(t, h, false);       
    dtostrf(hic, 6, 2, celsiusTemp);             
    dtostrf(h, 6, 2, humidityTemp);    
  }
}

void pageNotFound(){
  String page = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>ESP8266 Erro (404).</title><style type='text/css'>";
  page+= "*{font-family:'Arial';background-color:#000000;margin:0;padding:0;text-align:center;text-shadow:4px 4px 9px #f9f7f7;}</style>";
  page+="</head><body><div style='float:left;padding:10px;font-size: 40px;'><p style='color: yellow;'>Latinoware 2017</p></div>";
  page+="<div style='position:fixed;left:50%;margin-left:-150px;color:#ff0534;'><h1 style=font-size:180px;'>404</h1>";
  page+="<p style='margin-top:-24px;font-size:64px;'>Not Found</p></div></body></html>";  
  server.send(404, "html", page);
}

void verifyPins(){
  int flag;
  if(server.arg("all").length() != 0){
    flag = server.arg("all") == "on";
    digitalWrite(pinBlue  ,flag);
    digitalWrite(pinYellow,flag);
    digitalWrite(pinGreen ,flag);
    digitalWrite(pinRed   ,flag);
  }
  if(server.arg("red").length() != 0){
    flag = server.arg("red") == "on";
    digitalWrite(pinRed,flag);
  }
  if(server.arg("green").length() != 0){
    flag = server.arg("green") == "on";
    digitalWrite(pinGreen,flag);
  }
  if(server.arg("blue").length() != 0){
    flag = server.arg("blue") == "on";
    digitalWrite(pinBlue,flag);
  }
  if(server.arg("yellow").length() != 0){
    flag = server.arg("yellow") == "on";
    digitalWrite(pinYellow,flag);
  }
}

void pageState(){
  String page = "{'red':";
  page += digitalRead(pinRed)    == HIGH ? "'on'" : "'off'";
  page += ",'green':";
  page += digitalRead(pinGreen)  == HIGH ? "'on'" : "'off'";
  page += ",'yellow':";
  page += digitalRead(pinYellow) == HIGH ? "'on'" : "'off'";  
  page += ",'blue':";
  page += digitalRead(pinBlue)   == HIGH ? "'on'" : "'off'"; 
  page += ",'temp':";
  page += celsiusTemp;
  page += ",'humid':";
  page += humidityTemp;
  page += "}";
    server.send(200, "application/json", page); 
}

void pageHome(){
  verifyPins();
  readTemp();
  String page = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>ESP8266 Home.</title><style type='text/css'>";
  page+= "<style type='text/css'>*{font-family: 'Arial';margin:0;padding:0;text-align:center;}</style></head><body style='background-color: #000000;'>";
  page+= "<div style='position:fixed;left:50%;margin-top:50px;margin-left:-150px;color:#ff0534;'><div style='text-align:center'><p>Temperatura: ";
  page+= celsiusTemp;
  page+= " &#8451;</p><br><p>Humidade: ";
  page+= humidityTemp;
  page+= "</p></div><div><a href='\?all=on'><button style='width:100px;height:40px;background-color:#ffffff;'>All On</button></a>";
  page+= "<a href='\?all=off'><button style='width:100px;height:40px;background-color:#cac6c6'>All Off</button></a></div><div>";
  if(digitalRead(pinRed) == HIGH){
    page+= "<a href='\?red=off'><button style='width:200px;height:40px;background-color:#fb0505;'>Red Off</button></a>"; 
  }else{
    page+= "<a href='\?red=on'><button style='width:200px;height:40px;background-color:#cac6c6'>Red On</button></a>";    
  }
  page+= "</div><div>";
  if(digitalRead(pinGreen) == HIGH){
    page+= "<a href='\?green=off'><button style='width:200px;height:40px;background-color:#01ff09;'>Green Off</button></a>";
  }else{
    page+= "<a href='\?green=on'><button style='width:200px;height:40px;background-color:#cac6c6'>Green On</button></a>";
  }
  page+= "</div><div>";
  if(digitalRead(pinYellow) == HIGH){
    page+="<a href='\?yellow=off'><button style='width:200px;height:40px;background-color:#fbff00;'>Yellow Off</button></a>";
  }else{
    page+="<a href='\?yellow=on'><button style='width:200px;height:40px;background-color:#cac6c6'>Yellow On</button></a>";    
  }
  page+= "</div><div>";
  if(digitalRead(pinBlue) == HIGH){
    page+="<a href='\?blue=off'><button style='width:200px;height:40px;background-color:#0781ff;'>Blue Off</button></a>";
  }else{
    page+="<a href='\?blue=on'><button style='width:200px;height:40px;background-color:#cac6c6'>Blue On</button></a>";    
  }
  page+= "</div><div></body></html>";
  server.send(200, "html", page); 
}

void setup(){
  Serial.begin(9600);
  pinMode(pinDHT,   INPUT);
  pinMode(pinBlue  ,OUTPUT);
  pinMode(pinYellow,OUTPUT);
  pinMode(pinGreen ,OUTPUT);
  pinMode(pinRed   ,OUTPUT);
  flagAll(0);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Check Connection");
  display.display();
  delay(300);
  display.setCursor(0,0);
  display.println("Check Connection.");
  display.display();
  delay(300);
  display.setCursor(0,0);
  display.println("Check Connection..");
  display.display();
  delay(300);
  display.setCursor(0,0); 
  display.println("Check Connection...");
  display.display();
  delay(300);
  WiFiManager wifiManager;
  //wifiManager.resetSettings(); //Test
  if(!wifiManager.autoConnect(MYSSID,MYPASS)){
    display.clearDisplay(); 
    display.setCursor(0,0);
    display.println("Wifi Config");
    display.setCursor(0,10);
    display.println("SSID:");
    display.setCursor(32,10);
    display.println(MYSSID);
    display.setCursor(0,20);
    display.println("Password:");
    display.setCursor(55,20);
    display.println(MYPASS);
    display.display();
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  display.clearDisplay(); 
  display.setCursor(0,0);
  display.println("Connected");
  display.setCursor(0,10);
  display.println(WiFi.localIP());
  server.on("/",pageHome);
  server.on("/state",pageState);
  server.onNotFound(pageNotFound);
  server.begin();
  display.setCursor(0,20);
  display.println("Server Started");
  display.display();
}

void loop() {
  server.handleClient();
}
