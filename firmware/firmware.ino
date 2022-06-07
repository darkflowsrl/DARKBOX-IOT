#include <PubSubClient.h>

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

#include <BluetoothSerial.h> //For future implementations
#include <BTAddress.h>
#include <BTAdvertisedDevice.h>
#include <BTScan.h>

const char* ssid = "Chop";
const char* password =  "741963258";
const char* host = "test.mosquitto.org";
int port = 1883;

String randStringGen(){
  String letters[40] = {"a", "b", "c", "d", "e", "f","g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};  
  String randString = "";
  for(int n = 0; n<7; n++){
    randString = randString + letters[random(0,40)];
    }
  return randString;
}
void setup() {  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("La IP del dispositivo es:");
  Serial.println(WiFi.localIP());
  WiFiClient clienteW; //Creates the WIFI client
  PubSubClient mqttClient(clienteW); 
  mqttClient.setServer(host, port); 
  //mqttClient.setCallback(callback_function);
  mqttClient.connect("aaaaa");
  mqttClient.subscribe("test");
}

void loop() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando a red inalámbrica..");
  }
  
  mqttClient.publish("topic", "{"var" : "var"}");
}

/*Agregar lectura de datos de los sensores del nodemcu
 * Los tópicos de publicación deben salir de un archivo de configuración y se tienen que publicar en un bucle que los lea
 * y con su correspondiente Payload.
 * El SSID y el password deben salir de un archivo de configuracion, y el host y el puerto a verse.
 */
