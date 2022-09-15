#ifndef DATASENSORS_H
#define DATASENSORS_H
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <DHT.h>

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
#define EEPROM_SIZE 3
#define OneWirePin 4 // One WIre will be the temp. and humidity sensors
#define DHTPin 5
#define DHTType AM2301

OneWire oneWire(D2);
DHT myDHT(D1, DHTType);
DallasTemperature DS18B20(&oneWire);

class dataSensors
{
private:
    int sensorsCount = 0;
    float tempC;
    float DHCTemp, DHCHum;

public:
    float t0;
    float t1;
    float h0;

    void sensorsSetup()
    {
        // Temperature sensor setup
        DS18B20.begin();

        myDHT.begin();
        Serial.println("Searching temperature devices...");
        sensorsCount = DS18B20.getDeviceCount();
        Serial.print(sensorsCount, DEC);
        Serial.print(" Devices detected");
        Serial.println("");
    }
    std::vector<std::string> rawDataOneWire()
    {
        std::vector<std::string> temperatureVector;
        // Temp loop
        DS18B20.requestTemperatures();
        /*
        USE THIS "FOR" LOOP IN CASE YOU WANT TO CONNECT MORE TEMPERATURE SENSORS IN THE ONEWIRE
        BUS.
        for (int n = 0; n <= sensorsCount; n++)
        {
            String sensorNumber = "";
            sensorNumber += "Sensor" + String(n);
            if (n < sensorsCount)
            {
                temperatureVector.push_back(sensorNumber.c_str());
                temperatureVector.push_back(String(DS18B20.getTempCByIndex(n)).c_str());
            }
        }
        */
        // THE BELOW LINE IS FOR A SINGLE TEMPERATURE SENSOR
        temperatureVector.push_back("Sensor_0");
        temperatureVector.push_back(String(DS18B20.getTempCByIndex(0)).c_str());

        DHCHum = myDHT.readHumidity();
        DHCTemp = myDHT.readTemperature(false);

        temperatureVector.push_back(String(DHCTemp).c_str());
        temperatureVector.push_back(String(DHCHum).c_str());

        return temperatureVector;

        temperatureVector.clear();
    }
    String singleSensorRawdataTemp(int sensorNumber)
    {
        DS18B20.requestTemperatures();
        return String(DS18B20.getTempCByIndex(sensorNumber));
    }

    /*!
     *  @brief  Setup sensor boolean for data type
     *  @param  use
     *          True: return DHT humidity
     *          False: return DHT temperature
     */
    String singleSensorRawdataDHT(bool type)
    {
        if (type)
        {
            return String(myDHT.readHumidity());
        }
        else
        {
            return String(myDHT.readTemperature(false));
        }
        return "None";
    }
};

#endif