#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_Sensor.h>
#include <Wire.h> 
#include <vector>
#include <string.h>
 
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

// GPIO pins from the ESP-32
#define OneWirePin 2 // One WIre will be the temp. and humidity sensors 


OneWire oneWire(OneWirePin);
DallasTemperature DS18B20(&oneWire);

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
            Serial.print(" Devices detected");
            Serial.println("");
        }
        std::vector<std::string> rawData(){
            std::vector<std::string> temperatureVector;
             
            //Temp loop
            
           
            DS18B20.requestTemperatures();
            for(int n = 0; n <= sensorsCount; n++){
                String sensorNumber = "";
                sensorNumber += "Sensor" + String(n);
                if(n < sensorsCount){
                temperatureVector.push_back(sensorNumber.c_str());
                temperatureVector.push_back(String(DS18B20.getTempCByIndex(n)).c_str());
                }
            }
            //Concatenate Vectors
            /*
            std::vector<std::string> dataVector;    
            std::set_union(temperatureVector.begin(),
            temperatureVector.end(), inputVector.begin(), inputVector.end(),
            std::back_inserter(dataVector));
            */

            return temperatureVector;

            temperatureVector.clear();
        }

        String singleSensorRawdata(int sensorNumber){
            String dataSensor = String(DS18B20.getTempCByIndex(sensorNumber));
            
            return dataSensor;
        }
};

