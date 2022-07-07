#ifdef ESP32
#include <WiFi.h>
//#include <SPIFFS.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <PubSubClient.h> 
#include "dataSensors.h"
#include "screenController.h"
#include "inputController.h"
#include "apMode.h"

void listDir(fs::FS &fs, const char * dirname, uint8_t levels); 
void readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void renameFile(fs::FS &fs, const char * path1, const char * path2);
void deleteFile(fs::FS &fs, const char * path);
void loadData(fs::FS &fs, const char * path);
void callback(char* topic, byte* payload, unsigned int lenght);
void reconnect();
void setup_wifi();
void refreshScreen();

//Constructor for the sensors, the wifi and the MQTT Object
WiFiClient espClient; 
PubSubClient client(espClient);
dataSensors mySensors;
apMode apInstance;
screen myScreen;
inputController myInputs;

std::vector<String> myVector;
String tempString0, tempString1, tempString2, tempString3 = "";
const unsigned long eventInterval = 1500;
unsigned long previousTime = 0;

String ssid;
String wifiPassword;
String host;
String root_topic_subscribe;
String root_topic_publish;
const char* userName;
const char* password;
const int port = 1883;  
bool inSetup = true;
bool isWifi = true; //Change to false

void setup(){
  Serial.begin(115000);

  mySensors.sensorsSetup();
  myScreen.screenSetup();
  myInputs.inputSetup();

  myScreen.printScreen("Starting device...", 0, 1, true);
  delay(2000);
  myScreen.screenClean();

  //File System and configuration setup
  if(!LittleFS.begin()){
    myScreen.screenClean();
    myScreen.printScreen("Failed to mount...", 0, 1, true);
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  listDir(LittleFS, "/", 0);
  readFile(LittleFS, "/config.json");
  loadData(LittleFS, "/config.json"); 
  //AP setup
  myScreen.printScreen("Configuring...", 0, 1, true);
  apInstance.setupServer();
}

void loop() {
  if(isWifi == false){
    setup_wifi();
    if(WiFi.status() == WL_CONNECT_FAILED){
      Serial.println("Bad ssid or password");
      isWifi = true; /* I change this boolean to true, 
      because if connection fails, the user can configure it again*/
      setup();
    }
    isWifi = true;
  }
  if(inSetup == false){
    //MQTT
    client.setServer(host.c_str(), port);
    client.setCallback(callback); //Callback 
    //Sensors setup
    mySensors.sensorsSetup();
    //Sensors data as string
    std::string rawData = mySensors.rawData();
    //MQTT
    if(!client.connected()){
      reconnect();
    }
    if(client.connected()){
      Serial.println(rawData.c_str());
      client.publish(root_topic_publish.c_str(), rawData.c_str());
      delay(1);
    }
    client.loop();
      // Data to screen
    std::string tempData = mySensors.rawData();
    std::string inputsData = myInputs.inputData();
    Serial.println(tempData.c_str());
    if(inputsData != "{"){Serial.println(inputsData.c_str());}

    refreshScreen();
  }else{
    Serial.println("Device not configured");
    delay(5000);
  }
}

void refreshScreen(){
    unsigned long currentTime = millis();
     //I will use millis to create the time trigger to refresh the screen
    tempString0 += "Sensor0: " + mySensors.singleSensorRawata(0);
    tempString1 += "Sensor1: " + mySensors.singleSensorRawata(1);
    tempString2 += "Sensor2: " + mySensors.singleSensorRawata(2);

    if(currentTime - previousTime >= eventInterval){
      myScreen.printScreen("SENSORS DATA:", 0, 0, false);
      myScreen.printScreen(tempString0, 0, 1, false);
      myScreen.printScreen(tempString1, 0, 2, false);
      myScreen.printScreen(tempString2, 0, 3, false);
    }
    tempString0 = "";
    tempString1 = "";
    tempString2 = "";
}


//function> setup_wifi: None -> int
void setup_wifi(){
  //WiFi connection
  Serial.print("Conectando a ");
  Serial.print(ssid);
  Serial.println(" ");

  WiFi.begin(ssid.c_str(), wifiPassword.c_str());

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
      if(client.subscribe(root_topic_subscribe.c_str())){
        Serial.println("Subscripción exitosa");
      }else{
        Serial.println("subscripción Fallida...");
      }
    }else{
      Serial.println("Falló la conexión / Error >");
      Serial.print(client.state());
      Serial.println("Intentando nuevamente en 10 Segundos");
      delay(10000);
    }
  }
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname, "r");
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path, "r");
    if(!file || file.isDirectory()){
        Serial.println("...failed to open file for reading");
        return;
    }

    Serial.println("...read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    Serial.println("");

    file.close();   
}



void loadData(fs::FS &fs, const char * path){
    File file_ = fs.open(path, "r");
    String content;
    if(!file_.available()){
      Serial.println("Couldn't open the file");  
    }
    while (file_.available()) {
      content += file_.readString();
      break;
    }
    StaticJsonDocument<1024> config;
    auto error = deserializeJson(config, content);

    if(error){
      Serial.println("Failed to deserialize");
      Serial.println(error.f_str());
    }
    
    Serial.println(config.size());

    ssid = (const char*)config["ssid"];
    wifiPassword = (const char*)config["wifiPassword"];
    host = (const char*)config["host"];
    root_topic_subscribe = (const char*)config["root_topic_subscribe"];
    root_topic_publish = (const char*)config["root_topic_publish"];

    Serial.println("#### CONFIG LOADED ####");
    Serial.println(ssid);
    Serial.println(wifiPassword);
    Serial.println(host);
    Serial.println(root_topic_subscribe);
    Serial.println(root_topic_publish);

    file_.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, "w");
    if(!file){
        Serial.println("...failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("...file written");
    } else {
        Serial.println("...write failed");
    }
    file.close();
}

/*
void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("...failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("...message appended");
    } else {
        Serial.println("...append failed");
    }
    file.close();
}
*/

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("...file renamed");
    } else {
        Serial.println("...rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("...file deleted");
    } else {
        Serial.println("...delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    Serial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, "w");
    if(!file){
        Serial.println("...failed to open file for writing");
        return;
    }

    size_t i;
    Serial.print("...writing" );
    uint32_t start = millis();
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
          Serial.print(".");
        }
        file.write(buf, 512);
    }
    Serial.println("");
    uint32_t end = millis() - start;
    Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();

    file = fs.open(path, "r");
    start = millis();
    end = start;
    i = 0;
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        Serial.print("...reading" );
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F){
              Serial.print(".");
            }
            len -= toRead;
        }
        Serial.println("");
        end = millis() - start;
        Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
        file.close();
    } else {
        Serial.println("...failed to open file for reading");
    }
}
