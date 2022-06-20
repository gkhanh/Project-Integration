//#include <common.h>
#include "time.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <esp8266-google-home-notifier.h>
#include "version.h"
#include <TinyGPSPlus.h> // library for gps
#include <SoftwareSerial.h> // makes a new serial
#include <Metro.h> // this makes it possible to set timer flag

#include <SD.h>
#include <sd_defines.h>
#include <sd_diskio.h>

#include <FS.h>
#include <FSImpl.h>
#include <vfs_api.h>

#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
MAX30105 particleSensor;

#include <TimeLib.h>
#define WIFI_TIMEDOUT_MS 30000

//ESP _ S1106 OLED screen includes
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
//defines the I2C pins to which the display is connected
#define OLED_SDA 21
#define OLED_SCL 22

#define SD_CS 5

#define battPin 32

#define pinButton1 12
#define pinbutton2 14
//software serial initialisation
#define RXD2 16 // on ttgo use pin 15
#define TXD2 17
#define GPSBaud 9600
// The serial connection to the GPS device
SoftwareSerial ss(RXD2, TXD2);

// tinygpsplus object
TinyGPSPlus gps;

//Firebase
#include <FirebaseFS.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
// Insert Firebase project API Key
#define API_KEY "AIzaSyBHCug9AODPo_8G-FaNHZJY3hXCzwsKbKA"
// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://projectintegration-c0d16-default-rtdb.europe-west1.firebasedatabase.app/"
//Define Firebase Data object
//Define Firebase Data object
//#include "addons/SDHelper.h"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;
String databasePath = "/Patient1";
// Parent Node (to be updated in every loop)
String parentPath;
// Database child nodes
String FormatDate = "/Time";
String gpsLng = "/gpsLong";
String gpsLat = "/gpsLat";
String heartbeat = "/heartbeat";
int count = 0;
bool signupOK = false;
String formattedDate;

unsigned long sendDataPrevMillis = 0;
String dataMessage;

int sampleCount = 0;

#define MAX_BRIGHTNESS 255

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
//Arduino Uno doesn't have enough SRAM to store 100 samples of IR led data and red led data in 32-bit format
//To solve this problem, 16-bit MSB of the sampled data will be truncated. Samples become 16-bit data.
uint16_t irBuffer[100]; //infrared LED sensor data
uint16_t redBuffer[100];  //red LED sensor data
#else
uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data
#endif

int32_t bufferLength; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t prevHeartRate; // holds previous valid heart rate
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

bool switchScreen = false;
bool screen1 = true;
bool screen2 = false;

long lng;
long lat;

//sets pin 21 and 22 for display
Adafruit_SH1106 display(21, 22);

// current time variables
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

char *mqttServer = "broker.hivemq.com";
int mqttPort = 1883;

// google home
const char GoogleHomeName[] = "Master bedroom speaker";  //Must match the name on Google Home AP
const char *host = "GoogleSay";
String ttstext = "google home connection established";

GoogleHomeNotifier ghn;

// wifi credentials
const char *SSID = "Gi_TTGO";
const char *PWD = "obbf5496";

String result;
const char * c;

bool taskCompleted = false;

// time based interrupt
//Metro sendData5 = Metro(300000) // sets the metro timer for 5 min
Metro sendData1 = Metro(60000);  // sets the metro timer for 1 min  testing purposes
Metro sendData20s = Metro(20000); // sets the metro timer for 20 sec  testing purposes

//void rtdbDownloadCallback(RTDB_DownloadStatusInfo info)
//{
//  if (info.status == fb_esp_rtdb_download_status_init)
//  {
//    Serial.printf("Downloading file %s (%d) to %s\n", info.remotePath.c_str(), info.size, info.localFileName.c_str());
//  }
//  else if (info.status == fb_esp_rtdb_download_status_download)
//  {
//    Serial.printf("Downloaded %d%s\n", (int)info.progress, "%");
//  }
//  else if (info.status == fb_esp_rtdb_download_status_complete)
//  {
//    Serial.println("Download completed\n");
//  }
//  else if (info.status == fb_esp_rtdb_download_status_error)
//  {
//    Serial.printf("Download failed, %s\n", info.errorMsg.c_str());
//  }
//}
//
//// The Firebase upload callback function
//void rtdbUploadCallback(RTDB_UploadStatusInfo info)
//{
//  if (info.status == fb_esp_rtdb_upload_status_init)
//  {
//    Serial.printf("Uploading file %s (%d) to %s\n", info.localFileName.c_str(), info.size, info.remotePath.c_str());
//  }
//  else if (info.status == fb_esp_rtdb_upload_status_upload)
//  {
//    Serial.printf("Uploaded %d%s\n", (int)info.progress, "%");
//  }
//  else if (info.status == fb_esp_rtdb_upload_status_complete)
//  {
//    Serial.println("Upload completed\n");
//  }
//  else if (info.status == fb_esp_rtdb_upload_status_error)
//  {
//    Serial.printf("Upload failed, %s\n", info.errorMsg.c_str());
//  }
//}

void logSDCard() {
  dataMessage = String(count) + "," + String(random(0, 120)) + "," + String(random(35, 42)) + "," +
                String(formattedDate) + "\r\n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/data.txt", dataMessage.c_str());
}

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

float getPercentage() {
  float batteryLevel = map(analogRead(32), 50.0f, 4095.0f, 0, 100);
  Serial.println(batteryLevel);
  return batteryLevel;

}

void changeScreen() {
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  //set the text size, color, cursor position and displayed text
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(25, 25);
  //  if () // if the time is 6-12 am goodmorning, else if the time is 12-6pm good afternoon else good evening.
  display.println(" good day");
  display.display();
}

void changeScreen(String text) {
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  //set the text size, color, cursor position and displayed text
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(25, 25);
  //  if () // if the time is 6-12 am goodmorning, else if the time is 12-6pm good afternoon else good evening.
  display.println(text);
  display.display();
}

void changeScreen1() {
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  //set the text size, color, cursor position and displayed text
  display.setTextSize(1.0);
  display.setTextColor(WHITE);
  display.setCursor(85, 15);
  float temp = getPercentage();
  display.print(temp);
  display.println("%");
  display.setTextSize(2.0);
  display.setTextColor(WHITE);
  display.setCursor(35, 30);
  display.println(localTime());
  display.display();
}

void changeScreen2() {
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  //set the text size, color, cursor position and displayed text
  display.setTextSize(1.0);
  display.setTextColor(WHITE);
  display.setCursor(0, 15);
  display.println(localTime());
  display.setTextSize(1.0);
  display.setTextColor(WHITE);
  display.setCursor(85, 15);
  float temp = getPercentage();
  display.print(temp);
  display.println("%");
  display.setTextSize(2.0);
  display.setTextColor(WHITE);
  display.setCursor(40, 30);
  display.println(heartRate);
  display.display();
}

// button interrupt
void IRAM_ATTR Ext_INT1_ISR() { // esp32 interrupt
  switchScreen = true;
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

bool wifiConnected = false;
void connectToWiFi() {
  Serial.print("Connecting to ");
  WiFi.begin(SSID, PWD);
  Serial.println(SSID);
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEDOUT_MS) {
    Serial.print(".");
    delay(500);
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Failed.\n");
  }
  else {
    Serial.print("Connected.");
    wifiConnected = true;
  }
}

void connectToGH() {
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

String localTime() {
  String temp;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return temp = "Failed to obtain time";
  }
  char timeHour[6];
  strftime(timeHour, 6, "%H:%M", &timeinfo);
  temp = String(timeHour);
  return temp;
}

String localTimeS() {
  String temp;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return temp = "Failed to obtain time";
  }
  char timeHour[9];
  strftime(timeHour, 9, "%H:%M:%S", &timeinfo);
  temp = String(timeHour);
  return temp;
}

void transmitInfo() {
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    lat = gps.location.lat();
    lng = gps.location.lng();
    Serial.print(lat + lng);

  }
  else
  {
    Serial.print(F("INVALID"));
    lat = 0;
    lng = 0;
  }
}

void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);
  pinMode(pinButton1, INPUT);
  attachInterrupt(pinButton1, Ext_INT1_ISR, FALLING); // change pin to button pin used in project
  connectToWiFi();
  setupMQTT();
  connectToGH();
  if (ghn.notify(ttstext.c_str()) != true) {
    Serial.println(ghn.getLastError());
    return;
  }
  ttstext = "";

//  SD_Card_Mounting();
//#if defined(USE_SD_FAT_ESP32)
//  // Write demo data to file
//  SdFile file;
//  file.open("/data.txt", O_RDWR | O_CREAT);
//#else
//  File file = DEFAULT_SD_FS.open("/data.txt", "w");
//#endif
//  uint8_t v = 0;
//  for (int i = 0; i < 512; i++)
//  {
//    file.write(v);
//    v++;
//  }
//  file.close();
  // Initialize heartbeat sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings

  // setup current time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  /* Assign the api key (required) */
  config.api_key = API_KEY;
  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  //changes screen to startup screen
  changeScreen();
}

void loop() {
  while (wifiConnected == false) {
    if (sendData1.check()) {
      logSDCard();
      writeFile(SD, "/data.txt", "Reading ID, Time, Heartbeat, Temperature \r\n");
      appendFile(SD, "/data.txt", dataMessage.c_str());
    }
    if (WiFi.status() == WL_CONNECTED)
    {
      break;
    }
  }

  while (ss.available() > 0)
    if (gps.encode(ss.read())) {
      transmitInfo();
    }
  if (!mqttClient.connected())
    reconnect();
  result = "10";
  c = result.c_str();

  mqttClient.loop();

  if (switchScreen) {
    if (screen1) {
      screen1 = false;
      screen2 = true;
      changeScreen2();
      Serial.println("screen 2 is up");
      switchScreen = false;
    } else {
      screen1 = true;
      screen2 = false;
      changeScreen1();
      Serial.println("screen 1 is up");
      switchScreen = false;
    }
  }

  if (particleSensor.available() == false) { //do we have new data?
    Serial.println("im in the heartRate sensor loop");
    particleSensor.check(); //Check the sensor for new data
    redBuffer[sampleCount] = particleSensor.getRed();
    irBuffer[sampleCount] = particleSensor.getIR();
    particleSensor.nextSample(); //We're finished with this sample so move to next sample
    sampleCount++;
  }
  if (sampleCount >= 100) {// after recieving data 100 times (which wont happen seeing 1 or 2 is the best that happens)
    prevHeartRate = heartRate;
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
    if (validHeartRate == 0) {
      heartRate = prevHeartRate;
    }
    Serial.print(F(", HR="));
    Serial.print(heartRate, DEC);
    sampleCount = 0;
  }

  if (sendData1.check()) {

    //update current screen to change time
    if (screen1) {
      changeScreen1();
      Serial.println("im here");
    } else {
      changeScreen2();
    }
        formattedDate = localTimeS();
    
        parentPath= databasePath + "/" + String(count++);
        json.set(gpsLng.c_str(), String(lng));
        json.set(gpsLat.c_str(), String(lat));
        json.set(heartbeat.c_str(), String(heartRate));
        json.set(FormatDate.c_str(), formattedDate);
    
        Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }

//  if (Firebase.ready() && !taskCompleted)
//  {
//    taskCompleted = true;
//    // File name must be in 8.3 DOS format (max. 8 bytes file name and 3 bytes file extension)
//    Serial.println("\nSet file...");
//    if (!Firebase.setFile(fbdo, StorageType::SD, "Patient1", "data.txt", rtdbUploadCallback /* callback function*/))
//      Serial.println(fbdo.errorReason());
//    Serial.println("\nGet file...");
//    if (!Firebase.getFile(fbdo, StorageType::SD, "Patient1", "data.txt", rtdbDownloadCallback /* callback function*/))
//      Serial.println(fbdo.errorReason());
//    if (fbdo.httpCode() == FIREBASE_ERROR_HTTP_CODE_OK)
//    {
//#if defined(USE_SD_FAT_ESP32)
//      // Write demo data to file
//      SdFile file;
//      file.open("/data.txt", O_RDONLY);
//#else
//      File file;
//      file = DEFAULT_SD_FS.open("/data.txt", "r");
//#endif
//      int i = 0;
//      while (file.available())
//      {
//        if (i > 0 && i % 16 == 0)
//          Serial.println();
//        uint8_t v = file.read();
//        if (v < 16)
//          Serial.print("0");
//        Serial.print(v, HEX);
//        Serial.print(" ");
//        i++;
//      }
//      Serial.println();
//      file.close();
//    }
//  }
}

// Hive MQ
//Step 1: Run code
//Step 2: Go to this link and click on connect button
// Add the subscription topics one for each topic the ESP32 uses
//http://www.hivemq.com/demos/websocket-client/


// extra aid: https://www.survivingwithandroid.com/esp32-mqtt-client-publish-and-subscribe/
