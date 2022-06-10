#include <WiFi.h>
#include <PubSubClient.h>
#include <esp8266-google-home-notifier.h>
#include "version.h"
#include <Metro.h> // this makes it possible to set timer flag

//ESP _ S1106 OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
//defines the I2C pins to which the display is connected
#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SH1106 display(21, 22);

// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 

char *mqttServer = "broker.hivemq.com";
int mqttPort = 1883;

const char GoogleHomeName[] = "Master bedroom speaker";  //Must match the name on Google Home AP
const char *host= "GoogleSay";
String ttstext = "google home connection established";

GoogleHomeNotifier ghn;

const char *SSID = "Gi_TTGO";
const char *PWD = "obbf5496";

//Metro sendData5 = Metro(300000) // sets the metro timer for 5 min
//Metro sendData1 = Metro(60000)  // sets the metro timer for 1 min  testing purposes 
Metro sendData20s = Metro(20000); // sets the metro timer for 20 sec  testing purposes

void connectToWiFi() {
  Serial.print("Connecting to ");
 
  WiFi.begin(SSID, PWD);
  Serial.println(SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("Connected.");
}

void connectToGH(){
  Serial.println("connecting to Google Home...");
  if (ghn.device(GoogleHomeName, "en") != true) {
    Serial.println(ghn.getLastError());
    return;
  }
  Serial.print("found Google Home(");
  Serial.print(ghn.getIPAddress());
  Serial.print(":");
  Serial.print(ghn.getPort());
  Serial.println(")");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Callback - ");
  Serial.print("Message:");
//  ttstext = (char)payload;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    ttstext.concat((char)payload[i]);
  }
  Serial.println();
  if (ghn.notify(ttstext.c_str()) != true) {
    Serial.println(ghn.getLastError());
    return;
  }
  changeScreen(ttstext);
  ttstext = "";
}

void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  // set the callback function
  mqttClient.setCallback(callback);
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  setupMQTT();
  connectToGH();
  if (ghn.notify(ttstext.c_str()) != true) {
    Serial.println(ghn.getLastError());
    return;
  }
  ttstext = "";
  
  changeScreen();
  
//  while(!Serial){
//  Serial.println(".");
//  }
}

void changeScreen(){
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  //set the text size, color, cursor position and displayed text
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(25, 25);
//  if () // if the time is 6-12 am goodmorning, else if the time is 12-6pm good afternoon else good evening.
  display.println("good day");
  display.display();
  }

void changeScreen(String text){
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  //set the text size, color, cursor position and displayed text
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(25, 25);
  // instead of text use a time.
  display.println(text);
  display.display();
  }

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // subscribe to topic
        mqttClient.subscribe("/PJ_int/reciever");
      }
      
  }
}

String result;
const char * c;

void loop() {
//  while(Serial.available()>0){
//    char n = Serial.read();
//    result.concat(n);k
//  }
//  c = result.c_str();
//  Serial.print(c);
//  Serial.println();
  
  if (!mqttClient.connected())
    reconnect();
  result = "10";
  c = result.c_str();
    
  mqttClient.loop();
  if(sendData20s.check()){
    Serial.print(c);
    mqttClient.publish("/PJ_int/gps",c);
    result = "20";
    c = result.c_str();
    Serial.print(c);
    mqttClient.publish("/PJ_int/heartbeat", c);
    Serial.println();
  }
  
}

  // Hive MQ
  //Step 1: Run code
  //Step 2: Go to this link and click on connect button
  // Add the subscription topics one for each topic the ESP32 uses
  //http://www.hivemq.com/demos/websocket-client/


  // extra aid: https://www.survivingwithandroid.com/esp32-mqtt-client-publish-and-subscribe/
