//mosquitto_pub -h <host> -t <"topic"> -m <"message">
//mosquitto_sub -h <host> -t <"topic"> 
//mosquitto_pub -h mqtt.darkflow.com.ar -t giuli/data
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdio.h>

#include <Arduino.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DHT.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>

#define humiditySensor_PIN 23 // GPIO pins from the ESP-32
#define tempSensor1_PIN 32
#define DHTTYPE  DHT21 //Define the type of DHT sensor

using namespace std;

StaticJsonDocument<1024> config;
char json_[] = "{\"ssid\":\"Chopin\",\"wifiPassword\": \"Euler27182\",\"host\": \"mqtt.darkflow.com.ar\",\"root_topic_subscribe\": \"giuli/testing\",\"root_topic_publish\": \"giuli/data\",\"userName\": \" \",\"password\": \" \",\"port\":1883,\"mailSender\": \"giulicrenna@outlook.com\",\"mailReceiver\": \"kirchhoff2002\",\"mailPassword\": \"giuli_crenna@gmail.com\",\"smtpServer\": \"smtp.office365.com\",\"smtpPort\":587}";
DeserializationError error = deserializeJson(config, json_); // deserialize() allows a JSON data and an array that contains the JSON to deserialize 


const char* ssid = config["ssid"];
const char* wifiPassword = config["wifiPassword"];
const char* host = config["host"];
const char* root_topic_subscribe = config["root_topic_subscribe"];
const char* root_topic_publish = config["root_topic_publish"];
const char* userName = config["userName"];
const char* password = config["password"];
const int port = config["port"];  
String stringifiedJSON = "{"; //This variable will be used by the JSONIZER, here will the "data" be inserted.


//Constructor for the sensors, the wifi and the MQTT Object
OneWire oneWire(tempSensor1_PIN); 
DallasTemperature DS18B20(&oneWire);
DHT humiditySensor(humiditySensor_PIN, DHTTYPE);
WiFiClient espClient; 
PubSubClient client(espClient);

int sensorsCount = 0;
float tempC;

char message[100];
void callback(char* topic, byte* payload, unsigned int lenght);
void reconnect();
void setup_wifi();
void JSONIZER(String data0, float data1, bool doEnd);

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
    delay(2000);
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

//function> JSONIZER: Any, Any, bool -> void
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