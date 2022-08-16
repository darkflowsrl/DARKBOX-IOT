#include <Arduino.h>
#include <ESPAsyncTCP.h>
#define WEBSERVER_H
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Arduino_JSON.h>
#include <ESP8266mDNS.h>
#include "dataSensors.h"

void loadRequiredDataforHttpServer(fs::FS &fs, const char *path);
void changeCredentials(fs::FS &fs, const char *path, const char *device0, const char *device1, const char *device2);
void loadDevices(fs::FS &fs, const char *path);
String getReadings();

dataSensors _mySensors;
AsyncWebServer server(80);
AsyncEventSource events("/events");
MDNSResponder mDns;

String localDeviceName, httpLocalIp;
JSONVar readings;
unsigned long previous_time = 0;
unsigned long Delay = 2000;

void setupServer()
{
  // mDNS setup
  loadRequiredDataforHttpServer(LittleFS, "config.json");
  Serial.print("Local DNS: " + localDeviceName + ".local ");
  const char *resolutionName = localDeviceName.c_str();

  if (!mDns.begin(resolutionName, WiFi.localIP()))
  {
    Serial.println("(mDns instance) Error setting up DNS server");
  }
  else
  {
    Serial.println("(mDns instance) DNS server started succesfully");
  }

  mDns.addService("http", "tcp", 80);

  // ESPAsyncWebServer Setup
  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html"); });

  server.serveStatic("/", LittleFS, "/");

  // Request for the latest sensor readings
  server.on("/readings", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String json = getReadings();
    request->send(200, "application/json", json);
    json = String(); });

  events.onConnect([](AsyncEventSourceClient *client)
                   {
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    client->send("hello!", NULL, millis(), 10000); });
  server.addHandler(&events);
  server.begin();
}

void setupHttpServer()
{
  if ((millis() - previous_time) > Delay)
  {
    mDns.update();
    events.send("ping", NULL, millis());
    events.send(getReadings().c_str(), "new_readings", millis());
    previous_time = millis();
  }
}

String getReadings()
{
  readings["temperature0"] = _mySensors.singleSensorRawdataTemp(0);
  readings["temperature1"] = _mySensors.singleSensorRawdataTemp(1);
  readings["temperature2"] = _mySensors.singleSensorRawdataTemp(2);
  readings["temperature3"] = _mySensors.singleSensorRawdataDHT(false);
  readings["humidity"] = _mySensors.singleSensorRawdataDHT(true);
  String jsonString = JSON.stringify(readings);
  return jsonString;
}

void loadRequiredDataforHttpServer(fs::FS &fs, const char *path)
{
  File file_ = fs.open(path, "r");
  String content;
  if (!file_.available())
  {
    Serial.println("(DNS instance) Couldn't open the file");
  }
  while (file_.available())
  {
    content += file_.readString();
    break;
  }

  StaticJsonDocument<1024> config;
  auto error = deserializeJson(config, content);

  if (error)
  {
    Serial.println("(DNS instance) Failed to deserialize");
    Serial.println(error.f_str());
  }

  httpLocalIp = (const char *)config["network"]["ip"];
  localDeviceName = (const char *)config["device"]["name"];

  file_.close();
}
