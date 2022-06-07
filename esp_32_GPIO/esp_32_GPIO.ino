#include <OneWire.h>
#include <DallasTemperature.h>

#define humiditySensor0 12
#define tempSensor1 32

OneWire oneWire(tempSensor1);

DallasTemperature DS18B20(&oneWire);

int sensorsCount = 0;
float tempC;

void setup() {
  Serial.begin(115200);
  DS18B20.begin();
  Serial.print("Buscando sensores de temperatura...");
  sensorsCount = DS18B20.getDeviceCount();
  Serial.print(sensorsCount, DEC);
  Serial.print(" disposituvos");
  Serial.println(""); 
}
 
void loop() {
   DS18B20.requestTemperatures();

   for(int i = 0; i < sensorsCount; i++){
    Serial.print("Sensor ");
    Serial.print(i+1);
    Serial.print(" : ");
    tempC = DS18B20.getTempCByIndex(i);
    Serial.print(tempC);
    Serial.print(" °C"); //para acordarme es el caracter de grados... 
    Serial.println("");
    }
   Serial.println("");

   delay(1000);
}
