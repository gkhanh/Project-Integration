#include <SD.h>
#include <sd_defines.h>
#include <sd_diskio.h>

#include <FS.h>
#include <FSImpl.h>
#include <vfs_api.h>

#include <Firebase.h>
#include <FirebaseESP32.h>
#include <FirebaseFS.h>

#include <Metro.h>
#include <google-tts.h>
#include <esp8266-google-home-notifier.h>

#include <TimeLib.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "version.h"
#define WIFI_TIMEDOUT_MS 30000

#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_SH1106.h>
#define OLED_SDA 21
#define OLED_SCL 22
#define SD_CS 5

#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
// Insert Firebase project API Key
#define API_KEY "AIzaSyBHCug9AODPo_8G-FaNHZJY3hXCzwsKbKA"
// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://projectintegration-c0d16-default-rtdb.europe-west1.firebasedatabase.app/"
//Define Firebase Data object
#include "addons/SDHelper.h"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;

String databasePath = "/Patient1";
// Parent Node (to be updated in every loop)
String parentPath;
String temperature = "/temperature";
String heartbeat = "/heartbeat";
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
String formattedDate;
String FormatDate = "/Time";
String dataMessage;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;

bool switchScreen = false;
bool screen1 = true;
bool screen2 = false;

Adafruit_SH1106 display(21, 22);
// current time variables
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//char *mqttServer = "broker.hivemq.com";
//int mqttPort = 1883;

// google home
//const char GoogleHomeName[] = "Master bedroom speaker";  //Must match the name on Google Home AP
//const char *host = "GoogleSay";
//String ttstext = "google home connection established";

GoogleHomeNotifier ghn;

// wifi credentials
//const char *SSID = "Gi_TTGO";
//const char *PWD = "obbf5496";
const char *SSID = "o==[]:::::::::2.4G::::::::>";
const char *PWD = "Mates@2020";

String result;
const char * c;

bool taskCompleted = false;

// time based interrupt
//Metro sendData5 = Metro(300000) // sets the metro timer for 5 min
Metro sendData1 = Metro(60000);  // sets the metro timer for 1 min  testing purposes
Metro sendData20s = Metro(20000); // sets the metro timer for 20 sec  testing purposes

// The Firebase download callback function
void rtdbDownloadCallback(RTDB_DownloadStatusInfo info)
{
  if (info.status == fb_esp_rtdb_download_status_init)
  {
    Serial.printf("Downloading file %s (%d) to %s\n", info.remotePath.c_str(), info.size, info.localFileName.c_str());
  }
  else if (info.status == fb_esp_rtdb_download_status_download)
  {
    Serial.printf("Downloaded %d%s\n", (int)info.progress, "%");
  }
  else if (info.status == fb_esp_rtdb_download_status_complete)
  {
    Serial.println("Download completed\n");
  }
  else if (info.status == fb_esp_rtdb_download_status_error)
  {
    Serial.printf("Download failed, %s\n", info.errorMsg.c_str());
  }
}

// The Firebase upload callback function
void rtdbUploadCallback(RTDB_UploadStatusInfo info)
{
  if (info.status == fb_esp_rtdb_upload_status_init)
  {
    Serial.printf("Uploading file %s (%d) to %s\n", info.localFileName.c_str(), info.size, info.remotePath.c_str());
  }
  else if (info.status == fb_esp_rtdb_upload_status_upload)
  {
    Serial.printf("Uploaded %d%s\n", (int)info.progress, "%");
  }
  else if (info.status == fb_esp_rtdb_upload_status_complete)
  {
    Serial.println("Upload completed\n");
  }
  else if (info.status == fb_esp_rtdb_upload_status_error)
  {
    Serial.printf("Upload failed, %s\n", info.errorMsg.c_str());
  }
}

/*void changeScreen() {
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
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(25, 25);
  String temp = localTimeS() + "\n   Screen1 lives";
  display.println(temp);
  display.display();
  }

  void changeScreen2() {
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  //set the text size, color, cursor position and displayed text
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(25, 25);
  String temp = localTimeS() + "\n   Screen2 is awful";
  display.println(temp);
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
*/

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
/*
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
*/
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

// Write the sensor readings on the SD card
void logSDCard() {
  dataMessage = String(count) + "," + String(random(0,120)) + "," + String(random(35,42)) + "," +
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

void setup() {
  Serial.begin(115200);
  pinMode(32, INPUT);
  //attachInterrupt(32, Ext_INT1_ISR, RISING); // change pin to button pin used in project
  connectToWiFi();
  //setupMQTT();
  //connectToGH();
  /*if (ghn.notify(ttstext.c_str()) != true) {
    Serial.println(ghn.getLastError());
    return;
    }
    ttstext = "";*/
  // Mount SD card.
  SD_Card_Mounting();
#if defined(USE_SD_FAT_ESP32)
  // Write demo data to file
  SdFile file;
  file.open("/data.txt", O_RDWR | O_CREAT);
#else
  File file = DEFAULT_SD_FS.open("/data.txt", "w");
#endif
  uint8_t v = 0;
  for (int i = 0; i < 512; i++)
  {
    file.write(v);
    v++;
  }
  file.close();
  // Initialize sensor
  /*if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
    {
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    }
    Serial.println("Place your index finger on the sensor with steady pressure.");*/


  //particleSensor.setup(); //Configure sensor with default settings
  //particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  //particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

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
  if (DEFAULT_SD_FS.exists("/data.txt"))
    DEFAULT_SD_FS.remove("/data.txt");
  //changeScreen();
}

void loop() {
  formattedDate = localTimeS();
  while (wifiConnected == false) {
    if (sendData1.check()) {
      logSDCard();
      writeFile(SD, "/data.txt", "Reading ID, Time, Heartbeat, Temperature \r\n");
      appendFile(SD, "/data.txt", dataMessage.c_str());
    }
    if(WiFi.status() == WL_CONNECTED)
    {
        break;  
    }
  }
  /*if (!mqttClient.connected())
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
    }*/
  //long irValue = particleSensor.getIR();
  /*if (checkForBeat(irValue) == true)
    {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);
    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable
      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
    }*/
  if (sendData1.check()) {
    //update current screen to change time
    /*if (screen1) {
      changeScreen1();
      } else {
      changeScreen2();
      }*/
    
    parentPath = databasePath + "/" + String(count++);
    json.set(temperature.c_str(), String(random(35, 42)));
    json.set(heartbeat.c_str(), String(/*beatsPerMinute*/random(0, 120)));
    json.set(FormatDate.c_str(), formattedDate);
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }
  if (Firebase.ready() && !taskCompleted)
  {
    taskCompleted = true;
    // File name must be in 8.3 DOS format (max. 8 bytes file name and 3 bytes file extension)
    Serial.println("\nSet file...");
    if (!Firebase.setFile(fbdo, StorageType::SD, "Patient1", "data.txt", rtdbUploadCallback /* callback function*/))
      Serial.println(fbdo.errorReason());
    Serial.println("\nGet file...");
    if (!Firebase.getFile(fbdo, StorageType::SD, "Patient1", "data.txt", rtdbDownloadCallback /* callback function*/))
      Serial.println(fbdo.errorReason());
    if (fbdo.httpCode() == FIREBASE_ERROR_HTTP_CODE_OK)
    {
#if defined(USE_SD_FAT_ESP32)
      // Write demo data to file
      SdFile file;
      file.open("/data.txt", O_RDONLY);
#else
      File file;
      file = DEFAULT_SD_FS.open("/data.txt", "r");
#endif
      int i = 0;
      while (file.available())
      {
        if (i > 0 && i % 16 == 0)
          Serial.println();
        uint8_t v = file.read();
        if (v < 16)
          Serial.print("0");
        Serial.print(v, HEX);
        Serial.print(" ");
        i++;
      }
      Serial.println();
      file.close();
    }
  }
}
