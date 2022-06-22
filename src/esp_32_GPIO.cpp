#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdio.h>

#include <SPIFFS.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DHT.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>

using namespace std;

//mosquitto_pub -h <host> -t <"topic"> -m <"message">
//mosquitto_sub -h <host> -t <"topic"> 
//mosquitto_pub -h mqtt.darkflow.com.ar -t giuli/data -m 

#define humiditySensor_PIN 23 // GPIO pins from the ESP-32
#define tempSensor1_PIN 32
#define DHTTYPE DHT21 //Define the type of DHT sensor

//Constructor for the sensors, the wifi and the MQTT Object
OneWire oneWire(tempSensor1_PIN); 
DallasTemperature DS18B20(&oneWire);
DHT humiditySensor(humiditySensor_PIN, DHTTYPE);
WiFiClient espClient; 
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int lenght);
void JSONIZER(String data0, float data1, bool doEnd);
void listAllFiles();
void reconnect();
void setup_wifi();
void loadData();

const char *path = "/config.json";

const char *ssid_;
const char *wifiPassword;
const char *host;
const char *root_topic_subscribe;
const char *root_topic_publish;
const char *userName;
const char *password;
int port;

int sensorsCount = 0;
float tempC;
char message[100];
String stringifiedJSON = "{";

class terminalMessages{
  private:
    String msg1 = "Buscando sensores de temperatura...";
    String msg2;
    String msg3;
  public:
    String msg1Ret(){
      return msg1;
    }
};

terminalMessages msg1;

void setup() {
  //File system and Serial configuration
  Serial.begin(115200);
  //loadData();
  Serial.println(ssid_);
  Serial.println(password);
  Serial.println(host);
  //Temp configuration
  DS18B20.begin();
  Serial.print(msg1.msg1Ret());
  sensorsCount = DS18B20.getDeviceCount();
  Serial.print(sensorsCount, DEC);
  Serial.print(" dispositivos");
  Serial.println(""); 
  //Humidity Sensor
  humiditySensor.begin();
  //WIFI
  Serial.println("&&&&&&&&&&&&&&&&&&&&&&&&&");
  setup_wifi();
  //MQTT
  client.setServer(host, port);
  client.setCallback(callback); //Callback 
}

//function> loop: None -> void
void loop() {
  //Temp loop
  DS18B20.requestTemperatures();
  for(int n = 0; n < sensorsCount; n++){
    String sensorNumber = "Sensor" + String(n);
    if(n < sensorsCount-1){
      JSONIZER(sensorNumber, DS18B20.getTempCByIndex(n), false);
    }else{
      JSONIZER(sensorNumber, DS18B20.getTempCByIndex(n), false);
    }
  }
  //Humidity data
  float ambientHumidity = humiditySensor.readTemperature();
  JSONIZER("Humidity", ambientHumidity, true);
  //MQTT
  if(!client.connected()){
    reconnect();
  }
  if(client.connected()){
    Serial.println(stringifiedJSON);
    stringifiedJSON.toCharArray(message, 100);
    client.publish(root_topic_publish, message);
    delay(1);
  }
  client.loop();
  // {"temp0":x,"temp1:x,"temp2":x} 
  stringifiedJSON = "{";
}
//function> setup_wifi: None -> int
void setup_wifi(){

  //WiFi connection
  Serial.print("Conectando a ");
  Serial.print(String(ssid_));
  Serial.println(" ");

  WiFi.begin("chop", "741963258");

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("IP local: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int lenght){
  String incomingMessage = "";
  Serial.print("desde > ");
  Serial.print(topic);
  Serial.println("");
  for(int n = 0; n < lenght; n++){
    incomingMessage += (char)payload[n];
  }
  incomingMessage.trim();
  Serial.println(" >>" + incomingMessage);
}

void reconnect(){
  while(!client.connected()){
    String deviceId = "DarkFlow_";
    deviceId += String(random(0xffff), HEX);
    String message = "Intentando conectar a: " + String(host) + ", Con ID: " + String(deviceId); 
    if(client.connect(deviceId.c_str())){
      Serial.println("Conexión Exitosa");
      if(client.subscribe(root_topic_subscribe)){
        Serial.println("Subscripción exitosa");
      }else{
        Serial.println("subscripción Fallida...");
      }
    }else{
      Serial.println("Falló la conexión / Error >");
      Serial.print(client.state());
      Serial.println("Intentando nuevamente en 10 Segundos");
      delay(5000);
    }
  }
}

void JSONIZER(String data0, float data1, bool doEnd){
  if(doEnd == false){
    String stringStream = "\"" + data0 + "\" : \"" + data1 + "\", ";   
    stringifiedJSON += stringStream;
  } 
  if(doEnd == true){
    String stringStream = "\"" + data0 + "\" : \"" + data1 + "\"} ";
    stringifiedJSON += stringStream;
  }
}

void listAllFiles(){
  if(!SPIFFS.begin(true)){
    Serial.println("Error initializing");
    while(true){
      Serial.print("..");
    }
  }
  // List any available file in the flash file system
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while(file){
    Serial.print("File >> ");
    Serial.println(file.name());
    file = root.openNextFile();
  }
  root.close();
  file.close();
}

void loadData(){
  File file = SPIFFS.open(path);

  StaticJsonDocument<1024> document;
  DeserializationError error = deserializeJson(document, path);

  Serial.print("Loading config...");

  ssid_ = document["ssid"];
  wifiPassword = document["wifiPassword"];
  host = document["host"];
  root_topic_subscribe = document["root_topic_subscribe"];
  root_topic_publish = document["root_topic_publish"];
  userName = document["userName"];
  password = document["password"];
  port = stoi(document["port"].as<string>());

  file.close();
}