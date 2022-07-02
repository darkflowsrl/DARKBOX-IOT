#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <vector>
#include <string.h>
#include "jsonizer.h"

 
/*
pinMode(switchUpPin, INPUT_PULLUP)

humiditySensor_PIN -> IO Onewire
extraPin0 -> Input
extraPin1 -> Input
extraPin2 -> Input
extraPin3 -> Input
extraPin4 -> Output Salida 4
extraPin5 -> Output Salida 3
extraPin6 -> Output Salida 2
extraPin7 -> Output Salida 1
*/
#define DHTTYPE  DHT21 //Define the type of DHT sensor

// GPIO pins from the ESP-32
#define OneWirePin 2 // One WIre will be the temp. and humidity sensors 
#define Input1 16
#define Input2 14
#define Input3 12
#define Input4 13
#define Output1 15
#define i2c_SCL 5
#define i2c_SDA 4

OneWire oneWire(OneWirePin); 
DallasTemperature DS18B20(&oneWire);
DHT humiditySensor(OneWirePin, DHTTYPE);

class dataSensors{
    private:
        int sensorsCount = 0;
        float tempC;
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
            JSONIZER jsonSession;
            std::vector<std::string> myVector;
            //Temp loop
            String sensorNumber = "";
            float ambientHumidity;
            std::string rawData;
            DS18B20.requestTemperatures();
            for(int n = 0; n < sensorsCount; n++){
                sensorNumber += "Sensor" + String(n);
                if(n < sensorsCount){
                myVector.push_back(sensorNumber.c_str());
                myVector.push_back(String(DS18B20.getTempCByIndex(n)).c_str());
                }
            }
            //Humidity data
            ambientHumidity = humiditySensor.readTemperature();
            myVector.push_back(String("Humidity").c_str());
            myVector.push_back(String(ambientHumidity).c_str());
            //Create Json
            rawData = jsonSession.toSJSON(myVector);
            myVector.clear();
            return rawData;
        }
};
