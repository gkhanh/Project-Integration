#include <common.h>
#include <FirebaseFS.h>
#include <Firebase_ESP_Client.h>
#include <Utils.h>
#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
//Network credentials
#define WIFI_SSID "o==[]:::::::::2.4G::::::::>"
#define WIFI_PASSWORD "Mates@2020"
// Insert Firebase project API Key
#define API_KEY "AIzaSyBHCug9AODPo_8G-FaNHZJY3hXCzwsKbKA"
// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://projectintegration-c0d16-default-rtdb.europe-west1.firebasedatabase.app/" 
//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
int timestamp;
FirebaseJson json;
//get timestamp
const char* ntpServer = "pool.ntp.org";
String databasePath;
// Parent Node (to be updated in every loop)
String parentPath;
// Database child nodes
String temperature = "/temperature";
String heartbeat = "/heartbeat";
String timePath = "/timestamp";
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void setup(){
  Serial.begin(115200);
  configTime(0, 0, ntpServer);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  /* Assign the api key (required) */
  config.api_key = API_KEY;
  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  databasePath = "/Patient1";
}

void loop(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 30000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    timestamp = getTime();
    Serial.print ("time: ");
    Serial.println (timestamp);
    parentPath= databasePath + "/" + String(count++);
    json.set(temperature.c_str(), String(random(35,42)));
    json.set(heartbeat.c_str(), String(random(1,120)));
    json.set(timePath, String(timestamp));  
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }
}
