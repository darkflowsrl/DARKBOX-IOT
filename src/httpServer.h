#include <Arduino.h>
#define WEBSERVER_H
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
//#include <Arduino_JSON.h>
#include <ESP8266mDNS.h>
#include "dataSensors.h"

void loadRequiredDataforHttpServer(fs::FS &fs, const char *path);
void changeCredentials(fs::FS &fs, const char *path, const char *device0, const char *device1, const char *device2);
void loadDevices(fs::FS &fs, const char *path);
void readVariables(fs::FS &fs);
String proccesor(const String &var);
String getReadings();

dataSensors _mySensors;
AsyncWebServer server(80);
MDNSResponder mDns;

String localDeviceName, httpLocalIp, t0, t1, h0, d0, d1, d2, d3;

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
            { request->send(LittleFS, "/index.html", String(), false, proccesor); });
  server.on("/temperature0", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", t0); });
  server.on("/temperature1", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", t1); });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", h0); });

  server.begin();
}

void setupHttpServer()
{
  mDns.update();
}

String proccesor(const String &var)
{
  readVariables(LittleFS);
  if (var == "TEMPERATURE0")
  {
    return t0;
  }
  else if (var == "TEMPERATURE1")
  {
    return t1;
  }
  else if (var == "HUMIDITY")
  {
    return h0;
  }

  return String();
}

void readVariables(fs::FS &fs)
{
  File file_ = fs.open("temp.json", "r");
  String content;
  if (!file_)
  {
    Serial.println("(HTML variables load instance) Couldn't open the file");
  }
  while (file_.available())
  {
    content += file_.readString();
    break;
  }

  StaticJsonDocument<512> temporalData;
  auto error = deserializeJson(temporalData, content);

  if (error)
  {
    Serial.println("(HTML variables load instance) Failed to deserialize");
    Serial.println(error.f_str());
  }

  t0 = (const char *)temporalData["temp0"];
  t1 = (const char *)temporalData["tempDHT"];
  h0 = (const char *)temporalData["humDHT"];
  d0 = (const char *)temporalData["digital0"];
  d1 = (const char *)temporalData["digital1"];
  d2 = (const char *)temporalData["digital2"];
  d3 = (const char *)temporalData["digital3"];

  file_.close();
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
