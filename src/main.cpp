/**
 * @file main.cpp
 * @author Giuliano Crenna (giulicrenna@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-09-02
 *
 * @copyright Copyright (c) 2022
 *
 */

// SilFe2655
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <vector>
#include <cstdlib>

#include "global.h"
#include "apMode.h"
#include "dataSensors.h"
#include "screenController.h"
#include "inputController.h"
#include "jsonizer.h"
#include "functions.h"
#include "httpServer.h"
#include "myMqtt.h"

void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
void readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
void appendFile(fs::FS &fs, const char *path, const char *message);
void renameFile(fs::FS &fs, const char *path1, const char *path2);
void deleteFile(fs::FS &fs, const char *path);
void loadData(fs::FS &fs, const char *path);
void callback(char *topic, byte *payload, unsigned int lenght);
void loadTemporalData(fs::FS &fs, std::string t0, std::string t1, std::string h0,
                      std::string d0, std::string d1, std::string d2, std::string d3);
void reconnect();
void refreshScreen();

// Constructor for the sensors, the wifi and the MQTT Object
WiFiClient espClient;
dataSensors mySensors;
apMode apInstance;
Screen myScreen;
inputController myInputs;
JSONIZER jsonSession;

void setup()
{
  Serial.begin(115000);
  // File System and configuration setup
  if (!LittleFS.begin())
  {
    myScreen.screenClean();
    myScreen.printScreen("Failed to mount...", 0, 1, true);
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  // Load and visualize data
  listDir(LittleFS, "/", 0);
  readFile(LittleFS, "/config.json");
  loadData(LittleFS, "/config.json");
  // AP setup
  apInstance.setupServer(staticIpAP, gatewayAP, subnetMaskAP);
  // Devices setup
  mySensors.sensorsSetup();
  myScreen.screenSetup();
  myInputs.inputSetup();
  // Screen
  myScreen.printScreen("Starting device...", 0, 1, true);
  delay(2000);
  myScreen.screenClean();
  // mDNS setup
  setupServer();
  // MQTT
  mqttSetup(host.c_str(), port, root_topic_publish.c_str(), espClient, keep_alive_topic_publish.c_str());
}

void loop()
{
  // SMTP test
  /*
  if (std::atof(mySensors.singleSensorRawdataTemp(0).c_str()) >= std::atof("50"))
  {
    sendEmail(smtpSender.c_str(), smtpPass.c_str(), SmtpReceiver.c_str(),
              SmtpServer.c_str(), 587);
  }*/
  // HTTP and mDNS loop
  myInputs.inputData();
  // setupHttpServer();
  //  Data to screen
  //  refreshScreen();
  //  Temporal data to EEPROM
  if (millis() - previousTimeTemporalData >= temporalDataRefreshTime)
  {
    loadTemporalData(LittleFS, mySensors.singleSensorRawdataTemp(0).c_str(), mySensors.singleSensorRawdataDHT(false).c_str(), mySensors.singleSensorRawdataDHT(true).c_str(),
                     myInputs.returnSingleInput(13), myInputs.returnSingleInput(12),
                     myInputs.returnSingleInput(14), myInputs.returnSingleInput(16));
    previousTimeTemporalData = millis();
  }
  // MQTT temp
  if (millis() - previousTimeMQTTtemp > MQTTtemp)
  {
    myInputs.readInputs();
    // JSON data creation
    DynamicJsonDocument dataJson_0(512);
    std::string data_0, dataPretty_0;

    dataJson_0["DeviceId"] = String(ESP.getChipId());
    dataJson_0["DeviceName"] = deviceName.c_str();
    dataJson_0["Timestamp"] = formatedTime();
    dataJson_0["MsgType"] = "Data";
    dataJson_0["Value"][0]["Port"] = "DHT_TEMPERATURE";
    dataJson_0["Value"][0]["Value"] = mySensors.singleSensorRawdataDHT(false);
    serializeJson(dataJson_0, data_0);
    serializeJsonPretty(dataJson_0, dataPretty_0);

    Serial.println(dataPretty_0.c_str());
    mqttOnLoop(host.c_str(), port, root_topic_publish.c_str(), espClient, keep_alive_topic_publish.c_str(), root_topic_publish.c_str(),
               data_0.c_str());
    previousTimeMQTTtemp = millis();
  }
  // MQTT Humidity
  if (millis() - previousTimeMQTThum > MQTThum)
  {
    myInputs.readInputs();
    // JSON data creation
    DynamicJsonDocument dataJson_1(512);
    std::string data_1, dataPretty;

    dataJson_1["DeviceId"] = String(ESP.getChipId());
    dataJson_1["DeviceName"] = deviceName.c_str();
    dataJson_1["Timestamp"] = formatedTime();
    dataJson_1["MsgType"] = "Data";
    dataJson_1["Value"][0]["Port"] = "DHT_HUMIDITY";
    dataJson_1["Value"][0]["Value"] = mySensors.singleSensorRawdataDHT(true);
    serializeJson(dataJson_1, data_1);
    serializeJsonPretty(dataJson_1, dataPretty);

    Serial.println(dataPretty.c_str());
    mqttOnLoop(host.c_str(), port, root_topic_publish.c_str(), espClient, keep_alive_topic_publish.c_str(), root_topic_publish.c_str(),
               data_1.c_str());
    previousTimeMQTThum = millis();
  }
  if (millis() - previousKeepAliveTime > keepAliveTime)
  {
    String aliveMessage = String("{\"deviceStatus\": \"") + String(ESP.getChipId()) + String("\"}");
    mqttOnLoop(host.c_str(), port, keep_alive_topic_publish.c_str(), espClient, keep_alive_topic_publish.c_str(), keep_alive_topic_publish.c_str(),
               aliveMessage.c_str());
    previousKeepAliveTime = millis();
  }
  myInputs.readInputs();
}

/**
 * @brief This function load a temporal file
 * called temp.json wich is readed by the HTML and JS methods to
 * take the sensors data.
 * @param fs file system
 * @param t0 temperature 0
 * @param t1 temperature 1
 * @param h0 humidity 0
 * @param d0 digital io 0
 * @param d1 digital io 1
 * @param d2 digital io 2
 * @param d3 digital io 3
 */
void loadTemporalData(fs::FS &fs, std::string t0, std::string t1, std::string h0,
                      std::string d0, std::string d1, std::string d2, std::string d3)
{
  TemporalAccess.t0 = std::atoi(t0.c_str());
  TemporalAccess.t1 = std::atoi(t1.c_str());
  TemporalAccess.h0 = std::atoi(h0.c_str());
  TemporalAccess.d0 = d0.c_str();
  TemporalAccess.d1 = d1.c_str();
  TemporalAccess.d2 = d2.c_str();
  TemporalAccess.d3 = d3.c_str();
}

void refreshScreen()
{
  unsigned long currentTime = millis();
  // I will use millis to create the time trigger to refresh the screen
  tempString0 += "Sensor0: " + mySensors.singleSensorRawdataTemp(0);
  tempString1 += "Sensor1: " + mySensors.singleSensorRawdataTemp(1);
  tempString2 += "Sensor2: " + mySensors.singleSensorRawdataTemp(2);

  if (currentTime - previousTimeScreen >= eventInterval)
  {
    myScreen.printScreen("SENSORS DATA:", 0, 0, false);
    myScreen.printScreen(ntpRawNoDay(), 15, 0, false);
    myScreen.printScreen(tempString0, 0, 1, false);
    myScreen.printScreen(tempString1, 0, 2, false);
    myScreen.printScreen(tempString2, 0, 3, false);
  }
  tempString0 = "";
  tempString1 = "";
  tempString2 = "";
  previousTimeScreen = currentTime;
}

/**
 * @brief
 *
 * @param fs
 * @param dirname
 * @param levels
 */
void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname, "r");
  if (!root)
  {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.name(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void readFile(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path, "r");
  if (!file || file.isDirectory())
  {
    Serial.println("...failed to open file for reading");
    return;
  }

  Serial.println("...read from file:");
  while (file.available())
  {
    Serial.write(file.read());
  }
  Serial.println("");

  file.close();
}

void loadData(fs::FS &fs, const char *path)
{
  File file_ = fs.open(path, "r");
  String content;
  if (!file_.available())
  {
    Serial.println("Couldn't open the file");
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
    Serial.println("Failed to deserialize");
    Serial.println(error.f_str());
  }

  // Serial.println(config.size());

  deviceName = (const char *)config["device"]["name"];

  staticIpAP = (const char *)config["network"]["ip"];
  subnetMaskAP = (const char *)config["network"]["subnetMask"];
  gatewayAP = (const char *)config["network"]["gateway"];

  smtpSender = (const char *)config["smtp"]["mailSender"];
  smtpPass = (const char *)config["smtp"]["mailPassword"];
  SmtpReceiver = (const char *)config["smtp"]["mailReceiver"];
  SmtpServer = (const char *)config["smtp"]["smtpServer"];

  IO_0 = (const char *)config["ports"]["IO_0"];
  IO_1 = (const char *)config["ports"]["IO_1"];
  IO_2 = (const char *)config["ports"]["IO_2"];
  IO_3 = (const char *)config["ports"]["IO_3"];
  MQTTtemp = std::stoi((const char *)config["etc"]["MQTTtemp"]);
  MQTThum = std::stoi((const char *)config["etc"]["MQTThum"]);
  keepAliveTime = std::stoi((const char *)config["etc"]["keepAlive"]);

  Serial.println("#### CONFIG LOADED ####");

  file_.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, "w");
  if (!file)
  {
    Serial.println("...failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("...file written");
  }
  else
  {
    Serial.println("...write failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2)
{
  Serial.printf("Renaming file %s to %s\r\n", path1, path2);
  if (fs.rename(path1, path2))
  {
    Serial.println("...file renamed");
  }
  else
  {
    Serial.println("...rename failed");
  }
}

void deleteFile(fs::FS &fs, const char *path)
{
  Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path))
  {
    Serial.println("...file deleted");
  }
  else
  {
    Serial.println("...delete failed");
  }
}

void testFileIO(fs::FS &fs, const char *path)
{
  Serial.printf("Testing file I/O with %s\r\n", path);

  static uint8_t buf[512];
  size_t len = 0;
  File file = fs.open(path, "w");
  if (!file)
  {
    Serial.println("...failed to open file for writing");
    return;
  }

  size_t i;
  Serial.print("...writing");
  uint32_t start = millis();
  for (i = 0; i < 2048; i++)
  {
    if ((i & 0x001F) == 0x001F)
    {
      Serial.print(".");
    }
    file.write(buf, 512);
  }
  Serial.println("");
  uint32_t end = millis() - start;
  Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
  file.close();

  file = fs.open(path, "r");
  start = millis();
  end = start;
  i = 0;
  if (file && !file.isDirectory())
  {
    len = file.size();
    size_t flen = len;
    start = millis();
    Serial.print("...reading");
    while (len)
    {
      size_t toRead = len;
      if (toRead > 512)
      {
        toRead = 512;
      }
      file.read(buf, toRead);
      if ((i++ & 0x001F) == 0x001F)
      {
        Serial.print(".");
      }
      len -= toRead;
    }
    Serial.println("");
    end = millis() - start;
    Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
    file.close();
  }
  else
  {
    Serial.println("...failed to open file for reading");
  }
}
