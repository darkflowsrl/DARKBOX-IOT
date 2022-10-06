#define WEBSERVER_H
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include "dataSensors.hpp"

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
            { request->send(200, "application/json", proccesor()); });

  server.on("/gota", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/gota.gif", "image/png"); });
  server.on("/termp", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/termp.gif", "image/png"); });
  server.on("/relay", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/relay.gif", "image/png"); });
  server.on("/reset", HTTP_POST, [](AsyncWebServerRequest *request)
            {
            request->send(200,"text/plain","ok");
            delay(2000);
            restoreConfig(LittleFS);
            ESP.eraseConfig();
            ESP.reset();
            ESP.restart(); });

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
  heap = String(ESP.getFreeHeap());
  bootVersion = String(ESP.getBootVersion());
  chipId_ = String(ESP.getChipId());
  CPUfreq = String(ESP.getCpuFreqMHz());
  coreVersion = String(ESP.getCoreVersion());
  flashChipId = String(ESP.getFlashChipId());
  flashRealSize = String(ESP.getFlashChipRealSize());
  flashChipSpeed = String(ESP.getFlashChipSpeed());
  freeSketchSize = String(ESP.getFreeSketchSpace());
  fullVersion = String(ESP.getFullVersion());
  vcc = String(ESP.getVcc());

  String allValues = t0 + String(";") + t1 + String(";") + h0 + String(";") + d0 + String(";") + d1 + String(";") + d2 + String(";") + d3 + String(";") + heap + String(";") + bootVersion + String(";") + chipId_ + String(";") + CPUfreq + String(";") + coreVersion + String(";") + flashChipId + String(";") + flashRealSize + String(";") + flashChipSpeed + String(";") + freeSketchSize + String(";") + fullVersion + String(";") + vcc + fullVersion + String(";") + releStatus;

  return allValues;
}
