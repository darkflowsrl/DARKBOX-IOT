#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <jsonizer.h>

#include <Arduino.h>
#include <OneWire.h>
#include <DHT.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>

//mosquitto_pub -h <host> -t <"topic"> -m <"message">
//mosquitto_sub -h <host> -t <"topic"> 
//mosquitto_pub -h mqtt.darkflow.com.ar -t giuli/data -m 

#define humiditySensor_PIN 23 // GPIO pins from the ESP-32
#define tempSensor1_PIN 32
#define DHTTYPE  DHT21 //Define the type of DHT sensor

using namespace std;

void callback(char* topic, byte* payload, unsigned int lenght);
void reconnect();
void setup_wifi();

//Constructor for the sensors, the wifi and the MQTT Object
OneWire oneWire(tempSensor1_PIN); 
DallasTemperature DS18B20(&oneWire);
DHT humiditySensor(humiditySensor_PIN, DHTTYPE);
WiFiClient espClient; 
PubSubClient client(espClient);
JSONIZER JSONsession;

const string json = JSONsession.readFileIntoString(".\\config.json");
vector<string> configuration = JSONsession.toVECTOR(json);

const char* ssid = configuration[1].c_str();
const char* wifiPassword = configuration[3].c_str();
const char* host = configuration[5].c_str();
const char* root_topic_subscribe = configuration[7].c_str();
const char* root_topic_publish = configuration[9].c_str();
const char* userName = configuration[11].c_str();
const char* password = configuration[13].c_str();
const int port = stoi(configuration[15]);

int sensorsCount = 0;
float tempC;
char message[100];
vector<string> tempData;

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

  //Temp configuration
  Serial.begin(9600);
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
    if(n < sensorsCount){
      tempData.push_back(sensorNumber.c_str());
      tempData.push_back(String(DS18B20.getTempCByIndex(n), 3).c_str());   
    }
  }
  //Humidity data
  float ambientHumidity = humiditySensor.readTemperature();
  tempData.push_back("Humidity");
  tempData.push_back(String(ambientHumidity, 3).c_str());   
  //MQTT
  if(!client.connected()){
    reconnect();
  }
  if(client.connected()){
    string data = JSONsession.toSJSON(tempData);
    client.publish(root_topic_publish, data.c_str());
    delay(2000);
  }
  client.loop();
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

