#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <vector>
#include <string.h>
#include "jsonizer.h"

#define DHTTYPE  DHT21 //Define the type of DHT sensor
#define humiditySensor_PIN 23 // GPIO pins from the ESP-32
#define tempSensor1_PIN 32
#define extraPin0 27
#define extraPin1 26
#define extraPin2 25
#define extraPin3 35
#define extraPin4 34

OneWire oneWire(tempSensor1_PIN); 
DallasTemperature DS18B20(&oneWire);
DHT humiditySensor(humiditySensor_PIN, DHTTYPE);
JSONIZER jsonSession;

class dataSensors{
    private:
        int sensorsCount = 0;
        float tempC;
        std::vector<std::string> myVector;
    public:        
        void sensorsSetup(){
            //Temperature sensor setup
            DS18B20.begin();
            Serial.println("Searching temperature devices...");
            sensorsCount = DS18B20.getDeviceCount();
            Serial.print(sensorsCount, DEC);
            Serial.println(" Devices");
            Serial.println(""); 
            //Humidity sensor config
            humiditySensor.begin();
        }
        std::string rawData(){
            //Temp loop
            DS18B20.requestTemperatures();
            for(int n = 0; n < sensorsCount; n++){
                String sensorNumber = "Sensor" + String(n);
                if(n < sensorsCount){
                myVector.push_back(sensorNumber.c_str());
                myVector.push_back(String(DS18B20.getTempCByIndex(n)).c_str());
                }
            }
            //Humidity data
            float ambientHumidity = humiditySensor.readTemperature();
            myVector.push_back(String("Humidity").c_str());
            myVector.push_back(String(ambientHumidity).c_str());
            //Create Json
            std::string rawData = jsonSession.toSJSON(myVector);
            myVector.clear();
            return rawData;
        }
};