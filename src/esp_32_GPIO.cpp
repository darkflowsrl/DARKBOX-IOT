#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdio.h>

#include <SPIFFS.h>
#include <SD.h>
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

struct Config{
  char ssid[128];
  char wifiPassword[128];
  char host[128];
  char root_topic_subscribe[128];
  char root_topic_publish[128];
  char userName[128];
  char password[128];
  int port;
};

#define humiditySensor_PIN 23 // GPIO pins from the ESP-32
#define tempSensor1_PIN 32
#define DHTTYPE  DHT21 //Define the type of DHT sensor


//Constructor for the sensors, the wifi and the MQTT Object
OneWire oneWire(tempSensor1_PIN); 
DallasTemperature DS18B20(&oneWire);
DHT humiditySensor(humiditySensor_PIN, DHTTYPE);
WiFiClient espClient; 
PubSubClient client(espClient);
Config configuracion;
Config configData;

void callback(char* topic, byte* payload, unsigned int lenght);
void JSONIZER(String data0, float data1, bool doEnd);
void saveConfig(const char *filename, const Config &config);
void printFile(const char *filename, bool sd = false);
void loadConfig(bool sd = false);
void reconnect();
void setup_wifi();

const char *ssid = configuracion.ssid;
const char *wifiPassword = configuracion.wifiPassword;
const char *host = configuracion.host;
const char *root_topic_subscribe = configuracion.root_topic_subscribe;
const char *root_topic_publish = configuracion.root_topic_subscribe;
const char *userName =  configuracion.userName;
const char *password =  configuracion.password;
const int port = configuracion.port;

const char *path = ".\\config.json";
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
  if(!SPIFFS.begin(true)){
    Serial.println("Error initializing");
    while(true){}
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
  Serial.print(ssid);
  Serial.println(" ");

  WiFi.begin(ssid, wifiPassword);

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


void loadConfig(bool fromSD){
  if(fromSD==true){
    File file = SD.open(path);
    StaticJsonDocument<1024> document;

    DeserializationError error = deserializeJson(document, file);
    if(error){
      Serial.println("Error opening the file...");
    }  
    strlcpy(configData.ssid, document["ssid"], sizeof(configData.ssid));
    strlcpy(configData.wifiPassword, document["wifiPassword"], sizeof(configData.wifiPassword));
    strlcpy(configData.host, document["host"], sizeof(configData.host));
    strlcpy(configData.root_topic_publish, document["root_topic_publish"], sizeof(configData.root_topic_publish));
    strlcpy(configData.root_topic_subscribe, document["root_topic_subscribe"], sizeof(configData.root_topic_subscribe));
    strlcpy(configData.userName, document["userName"], sizeof(configData.userName));
    strlcpy(configData.password, document["password"], sizeof(configData.password));
    configData.port = document["port"];
    file.close();
    
  }else if(fromSD == false){
      auto file = ostringstream{};
      ifstream input_file(path);
      if (!input_file.is_open()) {
          std::cerr << "Could not open the file - '"
              << path << "'" << std::endl;
          exit(EXIT_FAILURE);
      }
      StaticJsonDocument<1024> document;

      DeserializationError error = deserializeJson(document, file);
      if(error){
        Serial.println("Error opening the file...");
      }  
      strlcpy(configData.ssid, document["ssid"], sizeof(configData.ssid));
      strlcpy(configData.wifiPassword, document["wifiPassword"], sizeof(configData.wifiPassword));
      strlcpy(configData.host, document["host"], sizeof(configData.host));
      strlcpy(configData.root_topic_publish, document["root_topic_publish"], sizeof(configData.root_topic_publish));
      strlcpy(configData.root_topic_subscribe, document["root_topic_subscribe"], sizeof(configData.root_topic_subscribe));
      strlcpy(configData.userName, document["userName"], sizeof(configData.userName));
      strlcpy(configData.password, document["password"], sizeof(configData.password));
      configData.port = document["port"];
  }
}
void saveConfig(const char *filename, const Config &config){
  SD.remove(filename);

  File file = SD.open(filename, FILE_WRITE);
  if(!file){
    Serial.println(F("Error while trying to create the file :o"));
    return;
  }
  
  StaticJsonDocument<1024> document;
  document["ssid"] = configData.ssid;
  document["wifiPassword"] = configData.wifiPassword;
  document["host"] = configData.host;
  document["root_topic_publish"] = configData.root_topic_publish;
  document["root_topic_subscribe"] = configData.root_topic_subscribe;
  document["userName"] = configData.userName;
  document["password"] = configData.password;
  document["port"] = configData.port;

  if(serializeJson(document, file) == 0){
    Serial.println("Failed to write into the file");
  }

  file.close();
}

void printFile(const char *filename, bool fromSD){
  if(fromSD == true){  
    File file = SD.open(filename);
    if(!file){
      Serial.println(F("Failed to read file"));
    }
    while(file.available()){
      Serial.print((char)file.read());
    }
    Serial.println();
    file.close();
    }else{
    }
}