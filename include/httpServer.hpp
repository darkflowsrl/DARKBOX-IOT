#ifndef WEBSERVER_H
#define WEBSERVER_H
#ifndef DASHBOARD_OUTLINE
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#endif
#include "dataSensors.hpp"
#include "functions.hpp"

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
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/www/pico.min.css", "text/css"); });
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
            restoreConfig(LittleFS);
            ESP.eraseConfig();
            ESP.reset();
            ESP.restart(); });
  server.on("/reboot", HTTP_POST, [](AsyncWebServerRequest *request)
            {
            request->send(200, "text/plain", "Rebooting Device...");
            ESP.restart(); });
  server.on("/save", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String inputMessage;

              if (request->hasParam("wifi"))
              {
                  inputMessage = request->getParam("wifi")->value();
              }
        
              request->send(200, "text/html", "WebServer cerrado."); 
              server.end(); 
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
  heap = String(ESP.getFreeHeap());
  vcc = String(float(ESP.getVcc() / 10000));

  String allValues = t0 + String(";");
  allValues += t1 + String(";");
  allValues += h0 + String(";"); 
  allValues += d0 + String(";");
  allValues += d1 + String(";");
  allValues += d2 + String(";");
  allValues += d3 + String(";");
  allValues += heap + String(";");
  allValues += bootVersion + String(";");
  allValues += chipId + String(";");
  allValues += CPUfreq + String(";");
  allValues += coreVersion + String(";");
  allValues += flashChipId + String(";");
  allValues += flashRealSize + String(";");
  allValues += flashChipSpeed + String(";");
  allValues += freeSketchSize + String(";");
  allValues += fullVersion + String(";");
  allValues += vcc + String(";");
  allValues += releStatus + String(";");
  allValues +=  formatedTime() + String(";");
  allValues +=  WiFi.localIP().toString();

  return allValues;
}

#endif