#include <WiFi.h>
#include <PubSubClient.h>

//WiFi setup
const char* ssid = "";
const char* password = "";

//Raspberry Pi MQTT broker IP address
const char* mqtt_server = "192.168.1.165";

WiFiClient espClient;
PubSubClient client(espClient);
char msg[100];
int value = 0;
const char* device_name = "My_ESP32";

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi()
{
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  //int attempts = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    //attempts++;
    //if(attempts > 3)
    //{
      //ESP.restart();
    //}
    Serial.print(".");
  }
 Serial.println("");
 Serial.println("WiFi connected");
 Serial.println("IP Address: ");
 Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length)
{
  Serial.print("Message arriveed on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  if(String(topic) == "My_Topic")
  {
    Serial.print("Changing output to: ");
    if (messageTemp == "message")
    {
      client.publish("My_Topic", "Message received at ESP32");
      Serial.println("on");
    }
    else if(messageTemp == "off")
    {
      Serial.println("off");
    }
  }
}

void reconnect()
{
  //loop until connected
  while(!client.connected())
  {
    Serial.print("Attempting connection to MQTT server!");
    //Attempt to connect
    if(client.connect(device_name))
    {
      Serial.println("Connected");
      client.subscribe("My_Topic");
    }
    else
    {
      Serial.print("failed, rc = ");
      Serial.print(client.state());
      Serial.println("Try again in 5 seconds");
      delay(5000);
    } 
  }
}

void loop()
{
  if(!client.connected())
  {
    reconnect();
}
  
}
