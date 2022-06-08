//mosquitto_sub -h <host> -t <"topic"> 
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define humiditySensor0 12
#define tempSensor1_PIN 32

const char* ssid     = "chop";
const char* password = "741963258";

const char* host = "test.mosquitto.org";
const char* root_topic_subscribe = "giuli/temp";
const char* root_topic_publish = "giuli/data";
const int port = 1883;

//Global vars
OneWire oneWire(tempSensor1_PIN);
DallasTemperature DS18B20(&oneWire);
WiFiClient espClient;
PubSubClient client(espClient);

int sensorsCount = 0;
float tempC;

char message[50];
void callback(char* topic, byte* payload, unsigned int lenght);
void reconnect();
void setup_wifi();

void setup() {
  //Temp configuration
  Serial.begin(115200);
  DS18B20.begin();
  Serial.print("Buscando sensores de temperatura...");
  sensorsCount = DS18B20.getDeviceCount();
  Serial.print(sensorsCount, DEC);
  Serial.print(" dispositivos");
  Serial.println(""); 
  //WIFI
  setup_wifi();
  //MQTT
  client.setServer(host, port);
  client.setCallback(callback); //Callback 
  
}
 
void loop() {
  //Temp loop
 DS18B20.requestTemperatures();
 float temp0 = DS18B20.getTempCByIndex(0);
 float temp1 = DS18B20.getTempCByIndex(1);
 float temp2 = DS18B20.getTempCByIndex(2);
 /*
 Serial.print("Sensor0 = ");
 Serial.print(temp0);
 Serial.println(" °C");
 Serial.print("Sensor1 = ");
 Serial.print(temp1);
 Serial.println(" °C");
 Serial.print("Sensor2 = ");
 Serial.print(temp2);
 Serial.println(" °C");
 Serial.println("============================");*/
 //MQTT
 if(!client.connected()){
  reconnect();
 }
 if(client.connected()){
  String str = "{ temp0:" + String(temp0) + ", temp1:" + String(temp1) + ", temp2:" + String(temp2) + "}";
  Serial.println(str);
  str.toCharArray(message, 50);
  client.publish(root_topic_publish, message);
  delay(1000);
 }
 client.loop();
 // {"temp0":x,"temp1:x,"temp2":x}
 
}

void setup_wifi(){
  //WiFi connection
  Serial.print("Conectando a ");
  Serial.print(ssid);
  Serial.println(" ");

  WiFi.begin(ssid, password);

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
  Serial.println(">>" + incomingMessage);
}

void reconnect(){
  while(!client.connected()){
    String deviceId = "DarkFlow_";
    deviceId += String(random(0xffff), HEX);
  
    Serial.print("Intentando conectar a: ");
    Serial.println(host);
    if(client.connect(deviceId.c_str())){
      Serial.println("Coexión Exitosa");
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
