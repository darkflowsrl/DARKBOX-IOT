#define WEBSERVER_H
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include "dataSensors.h"

void changeCredentials(fs::FS &fs, const char *path, const char *device0, const char *device1, const char *device2);
void loadDevices(fs::FS &fs, const char *path);
void readVariables(fs::FS &fs);
String proccesor();
String getReadings();

dataSensors _mySensors;
AsyncWebServer server(80);
MDNSResponder mDns;

String localDeviceName = String("darkflow-") + String(ESP.getChipId());
String t0, t1, h0, d0, d1, d2, d3, io0, io1, io2, io3, allvalues;

void setupServer()
{
  // mDNS setup
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
   server.on("/allvalues", HTTP_GET, [](AsyncWebServerRequest *request)
            {
            request->send(200, "application/json", proccesor());
            });

  server.begin();
}

void setupHttpServer()
{
  mDns.update();
}

String proccesor()
{
  t0 = String(TemporalAccess.t0);
  t1 = String(TemporalAccess.t1);
  h0 = String(TemporalAccess.h0);
  d0 = String(TemporalAccess.d0);
  d1 = String(TemporalAccess.d1);
  d2 = String(TemporalAccess.d2);
  d3 = String(TemporalAccess.d3);

  String allValues = t0 + String(";") + t1 + String(";") + h0 + String(";") + d0 + String(";") + d1 + String(";") + d2 + String(";") + d3;

  return allValues; 
}
