#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <PubSubClient.h> 
#include "dataSensors.h"
#include "screenController.h"
#include "inputController.h"
#include "apMode.h"


void refreshScreen();

dataSensors mySensors;
screen myScreen;
inputController myInputs;

std::vector<String> myVector;
String tempString0, tempString1, tempString2, tempString3 = "";
const unsigned long eventInterval = 1500;
unsigned long previousTime = 0;

void setup() {
  Serial.begin(115000);

  mySensors.sensorsSetup();
  myScreen.screenSetup();
  myInputs.inputSetup();

  myScreen.printScreen("Starting device...", 0, 1, true);
  delay(2000);
  myScreen.screenClean();
}

void loop() {
  std::string tempData = mySensors.rawData();
  std::string inputsData = myInputs.inputData();
  Serial.println(tempData.c_str());
  if(inputsData != "{"){Serial.println(inputsData.c_str());}

  refreshScreen();
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