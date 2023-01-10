#ifndef WEBSERVER_H
#define WEBSERVER_H
#ifndef DASHBOARD_OUTLINE
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#endif
#include "dataSensors.hpp"

String proccesor();

dataSensors _mySensors;
AsyncWebServer server(80);
MDNSResponder mDns;

String t0, t1, h0, d0, d1, d2, d3, io0, io1, io2, io3, allvalues;

void setupServer()
{
  // mDNS setup
  Serial.print("Local DNS: " + localDeviceName + ".local ");
  
  if (!mDns.begin(localDeviceName.c_str(), WiFi.localIP()))
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
            { request->send(LittleFS, "/www/index.html", "text/html"); });
  server.on("/allvalues", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", proccesor()); });

  server.on("/gota", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/www/gota.gif", "image/png"); });
  server.on("/termp", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/www/termp.gif", "image/png"); });
  server.on("/relay", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/www/relay.gif", "image/png"); });
  server.on("/reset", HTTP_POST, [](AsyncWebServerRequest *request)
            {
            request->send(200, "text/plain", "Resetting Device...");
            delay(5000);
            restoreConfig(LittleFS);
            ESP.eraseConfig();
            ESP.reset();
            ESP.restart(); });
    server.on("/reboot", HTTP_POST, [](AsyncWebServerRequest *request)
            {
            request->send(200, "text/plain", "Rebooting Device...");
            delay(5000);
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
  vcc = String(ESP.getVcc());

  String allValues = t0 + String(";") + t1 + String(";") + h0 + String(";") + d0 + String(";") + d1 + String(";") + d2 + String(";") + d3 + String(";") + heap + String(";") + bootVersion + String(";") + chipId + String(";") + CPUfreq + String(";") + coreVersion + String(";") + flashChipId + String(";") + flashRealSize + String(";") + flashChipSpeed + String(";") + freeSketchSize + String(";") + fullVersion + String(";") + vcc + fullVersion + String(";") + releStatus;

  return allValues;
}

#endif