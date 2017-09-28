#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>         //https://github.com/esp8266/Arduino
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <DHT.h>
#include <UniversalTelegramBot.h>
#include <Thread.h>
#include <ThreadController.h>

#define dhtType  DHT11
#define pinDHT    16
#define pinRed    15
#define pinGreen  13
#define pinBlue   14
#define pinYellow 12
#define token "236111334:AAHOtZ30wKF4B1UtvaW3E5Vu6HAxFC-QIV4"

const char* CSSID = "ESP8266AP";
const char* CPASS = "0000"; 

long lastTimeScan;  // Ultima vez que buscou mensagem

Adafruit_SSD1306 display(0);
WiFiClientSecure client;
UniversalTelegramBot *bot;
ThreadController threadControl = ThreadController();
Thread threadTelegram          = Thread();



/**** DTH 11 *********************************************************************/
//DHT dht(pinDHT, dhtType);
//static char celsiusTemp[7];
//static char humidityTemp[7];
/********************************************************************************/

void flagAll(int flag){
  digitalWrite(pinRed   ,flag);
  digitalWrite(pinGreen ,flag);
  digitalWrite(pinYellow,flag);
  digitalWrite(pinBlue  ,flag);
}

void readMessages(){
  int messages = bot->getUpdates(bot->last_message_received + 1);  
  Serial.println(messages);
  //while(numNewMessages) {
  for (int i= 0; i < messages; i++) {
      String chat_id = String(bot->messages[i].chat_id);
      String text = bot->messages[i].text;
      String data = bot->messages[i].data;
      //Serial.println(text);
      Serial.print("data");
      Serial.println(data);
      //if(data) text = data;
      String from_name = bot->messages[i].from_name;   
      String message; 
      if (from_name == "") from_name = "Desconhecido";

      if(data == "pinRed"){
        digitalWrite(pinRed, !digitalRead(pinRed));
      }
      
      if (text == "pinRed") {
        digitalWrite(pinRed, !digitalRead(pinRed));
        message = "Led está ligado";
      }else if(text == "/pinRedOff") {
        digitalWrite(pinRed, LOW);
        message = "Led está desligado";
      }else if(text == "/start") {
        message = from_name + ", bem vindo.\n";
      }else if(text == "/menu"){
        //String keyboardJson = "[[\{ \"text\" : \"Luz Verde\", \"url\" : \"https://www.youtube.com\" \} ]]";
        String keyboardJson = "[[\{ \"text\" : \"Luz Verde\", \"callback_data\" : \"pinRed\" \} ]]";
        //String keyboardJson = "[[\{ \"text\" : \"Verde\", \"callback_data\" : \"/pinRedOn\" \} ]]";
        //"inline_keyboard": [[{"text": "A","callback_data": "A1"},{"text": "B","callback_data": "C1"}]]
        bot->sendMessageWithInlineKeyboard(chat_id, "Escolha qual lampada quer acender", "", keyboardJson);
          //String keyboardJson = "[[\"/pinRedOn\", \"/pinRedOff\"],[\"/status\"]]";
         //bot->sendMessageWithReplyKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson, true);
      }
      //bot->sendMessage(chat_id, message, "");
  }
  //numNewMessages = bot->getUpdates(bot->last_message_received + 1);
}

void handleNewMessages(int numNewMessages) {
  
}

void readTemp(){
//  float h = dht.readHumidity();
//  float t = dht.readTemperature();
//  if (isnan(h) || isnan(t)) {
//    strcpy(celsiusTemp,"Failed");
//    strcpy(humidityTemp, "Failed");   
//  }else{
//    float hic = dht.computeHeatIndex(t, h, false);       
//    dtostrf(hic, 6, 2, celsiusTemp);             
//    dtostrf(h, 6, 2, humidityTemp);    
//  }
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
  if(!wifiManager.autoConnect(CSSID,CPASS)){
      Serial.println("Wifi Config");
      Serial.print("SSID");
      Serial.println(CSSID);
      Serial.println("Password");
      Serial.println(CPASS);
    display.clearDisplay(); 
    display.setCursor(0,0);
    display.println("Wifi Config");
    display.setCursor(0,10);
    display.println("SSID:");
    display.setCursor(32,10);
    display.println(CSSID);
    display.setCursor(0,20);
    display.println("Password:");
    display.setCursor(55,20);
    display.println(CPASS);
    display.display();
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  Serial.println("Conected");
  Serial.println(WiFi.localIP());
  bot = new UniversalTelegramBot(token, client);
  display.clearDisplay(); 
  display.setCursor(0,0);
  display.println("Connected");
  display.setCursor(0,10);
  display.println(WiFi.localIP());
  //server.on("/",pageHome);
  //server.on("/state",pageState);
  //server.onNotFound(pageNotFound);
  //server.begin();
  display.setCursor(0,20);
  display.println("Server Started");
  display.display();
  threadControl.add(&threadTelegram);
  threadTelegram.onRun(readMessages);
  threadTelegram.setInterval(200);
  //lastTimeScan = millis();
}

void loop() {
  threadControl.run();
//  if (millis() > lastTimeScan + messageInterval)  {
//    Serial.println("Checking messages - ");
//    int numNewMessages = bot->getUpdates(bot->last_message_received + 1);  
//    Serial.println(numNewMessages);
//    while(numNewMessages) {
//      // Serial.println("got response");
//      handleNewMessages(numNewMessages);
//      numNewMessages = bot->getUpdates(bot->last_message_received + 1);
//    }
//    lastTimeScan = millis();
//  }
//  yield();
//  delay(10);
}
