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
#include <ArduinoJson.h>
#include <Arduino.h>
#include <vector>
#include <cstdlib>
#include <WiFiManager.h>
#include <Preferences.h>
#include <LittleFS.h>


#include "global.hpp"
#include "functions.hpp"
#ifdef AP_CUSTOM_PORTAL
#include "apMode.hpp"
#endif
#ifdef SMTP_CLIENT
#include "smtp.hpp"
#endif
#include "dataSensors.hpp"
#ifdef I2C
#include "screenController.hpp"
#endif
#ifdef LOCAL_DASHBOARD
#include "httpServer.hpp"
#endif
#include "inputController.hpp"
#include "jsonizer.hpp"
#include "myMqtt.hpp"

void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
void readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
void appendFile(fs::FS &fs, const char *path, const char *message);
void renameFile(fs::FS &fs, const char *path1, const char *path2);
void deleteFile(fs::FS &fs, const char *path);
void loadData(fs::FS &fs, const char *path);
void callback(char *topic, byte *payload, unsigned int lenght);
void loadTemporalData(std::string t0, std::string t1, std::string h0,
                      std::string d0, std::string d1, std::string d2, std::string d3);
void loadDataPreferences();
void refreshScreen();

// Constructor for the sensors, the wifi and the MQTT Object
#ifdef AP_CUSTOM_PORTAL
apMode apInstance;
#else
WiFiManager myManager;
#endif
WiFiClient espClient;
dataSensors mySensors;
#ifdef I2C
Screen myScreen;
#endif
inputController myInputs;
JSONIZER jsonSession;

void setup()
{
  // Serial setup
  Serial.begin(115000);
  // Load and visualize data
  #ifdef PREFERENCES
  listDir(LittleFS, "/", 1);
  loadDataPreferences();
  #endif
  #ifndef PREFERENCES
  LittleFS.begin();
  readFile(LittleFS, "/config.json");
  loadData(LittleFS, "/config.json");
  LittleFS.end();
  #endif
  // AP setup
  apInstance.setupServer();
  DHCPtoStatic(staticIpAP, gatewayAP, subnetMaskAP);
  // File System and configuration setup
  if (!LittleFS.begin())
  {
    Serial.println("(Setup Instance) SPIFFS Mount Failed");
  }
  // Devices setup
  mySensors.sensorsSetup();
#ifdef I2C
  myScreen.screenSetup();
  // Screen
  myScreen.screenClean();
  myScreen.printScreen("Starting device...", 0, 1, true);
#endif
  myInputs.inputSetup();
  delay(2000);
  // MQTT
  mqttSetup(host.c_str(), port, root_topic_publish.c_str(), espClient, keep_alive_topic_publish.c_str());
// Local Dashboard
#ifdef LOCAL_DASHBOARD
  setupServer();
#endif
}

void checkConn()
{
  bool conn = WiFi.isConnected();
  delay(100);
  if (!conn)
  {
    ESP.restart();
  }
}

void loop()
{
  checkConn();
  myInputs.inputData();
  // SMTP test
  if (std::atof(mySensors.singleSensorRawdataTemp(0).c_str()) >= std::atof("50"))
  {
#ifdef SMTP_CLIENT
    sendMail("Alerta", "You have Overpass the temperature");
#endif
  }

  // HTTP and mDNS loop
  setupHttpServer();
#ifdef I2C
  refreshScreen();
#endif
  //  Temporal data to EEPROM
  if (millis() - previousTimeTemporalData >= temporalDataRefreshTime)
  {
    loadTemporalData(mySensors.singleSensorRawdataTemp(0).c_str(), mySensors.singleSensorRawdataDHT(false).c_str(), mySensors.singleSensorRawdataDHT(true).c_str(),
                     myInputs.returnSingleInput(16), myInputs.returnSingleInput(14),
                     myInputs.returnSingleInput(12), myInputs.returnSingleInput(13));
    previousTimeTemporalData = millis();
  }
  // MQTT temp
  if (millis() - previousTimeMQTT_DHT > MQTTDHT)
  {
    myInputs.readInputs();
    // JSON data creation
    DynamicJsonDocument dataJson_0(512);
    std::string data_0, dataPretty_0;

    dataJson_0["DeviceId"] = chipId;
    dataJson_0["DeviceName"] = deviceName.c_str();
    dataJson_0["Timestamp"] = ntpRaw();
    dataJson_0["MsgType"] = "Data";
    dataJson_0["Value"][0]["Port"] = portsNames.DHTSensor_temp_name;
    dataJson_0["Value"][0]["Value"] = mySensors.singleSensorRawdataDHT(false);
    dataJson_0["Value"][1]["Port"] = portsNames.DHTSensor_hum_name;
    dataJson_0["Value"][1]["Value"] = mySensors.singleSensorRawdataDHT(true);
    serializeJson(dataJson_0, data_0);
    serializeJsonPretty(dataJson_0, dataPretty_0);
    // Serial.println(dataPretty_0.c_str());
    mqttOnLoop(host.c_str(), port, root_topic_publish.c_str(), espClient, keep_alive_topic_publish.c_str(), root_topic_publish.c_str(),
              data_0.c_str());
    previousTimeMQTT_DHT = millis();
  }
  // MQTT Sigle temperature
  if (millis() - previousMQTTsingleTemp > MQTTsingleTemp)
  {
    myInputs.readInputs();
    // JSON data creation
    DynamicJsonDocument dataJson_1(512);
    std::string data_1, dataPretty;

    String sensorData = mySensors.singleSensorRawdataTemp(0);
    dataJson_1["DeviceId"] = chipId;
    dataJson_1["DeviceName"] = deviceName.c_str();
    dataJson_1["Timestamp"] = ntpRaw();
    dataJson_1["MsgType"] = "Data";
    dataJson_1["Value"][0]["Port"] = portsNames.TempSensor_name;
    dataJson_1["Value"][0]["Value"] = sensorData;
    serializeJson(dataJson_1, data_1);
    serializeJsonPretty(dataJson_1, dataPretty);

    if (sensorData != "None")
    {
      mqttOnLoop(host.c_str(), port, root_topic_publish.c_str(), espClient, keep_alive_topic_publish.c_str(), root_topic_publish.c_str(),
                 data_1.c_str());
      previousMQTTsingleTemp = millis();
    }
  }
  // Keep alive message
  if (millis() - previousKeepAliveTime > keepAliveTime)
  {
    String aliveMessage = String("{\"deviceStatus\": \"") + chipId + String("\"}");
    mqttOnLoop(host.c_str(), port, keep_alive_topic_publish.c_str(), espClient, keep_alive_topic_publish.c_str(), keep_alive_topic_publish.c_str(),
               aliveMessage.c_str());
    previousKeepAliveTime = millis();
  }

  delay(1);
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
void loadTemporalData(std::string t0, std::string t1, std::string h0,
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

#ifdef I2C
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
    myScreen.printScreen(formatedTime(), 15, 0, false);
    myScreen.printScreen(tempString0, 0, 1, false);
    myScreen.printScreen(tempString1, 0, 2, false);
    myScreen.printScreen(tempString2, 0, 3, false);
  }
  tempString0 = "";
  tempString1 = "";
  tempString2 = "";
  previousTimeScreen = currentTime;
}
#endif

#ifdef PREFERENCES
void loadDataPreferences(){
  myPref.begin("EPM", false);
  deviceName = myPref.getString("deviceName", "default");

  staticIpAP = myPref.getString("staticIpAP", "");
  subnetMaskAP = myPref.getString("subnetMaskAP", "");
  gatewayAP = myPref.getString("gatewayAP", "");

  SmtpSender = myPref.getString("SmtpSender", "default@outlook.com");
  SmtpPass = myPref.getString("SmtpPass", "default123");
  SmtpReceiver = myPref.getString("SmtpReceiver", "default@outlook.com");
  SmtpServer = myPref.getString("SmtpServer", "smtp.default.com");
  SmtpPort = std::stoi(myPref.getString("SmtpPort", "587").c_str());

  IO_0 = myPref.getString("IO_0", "OTU");
  IO_1 = myPref.getString("IO_1", "OTU");
  IO_2 = myPref.getString("IO_2", "OTU");
  IO_3 = myPref.getString("IO_3", "OTU");

  portsNames.DHTSensor_hum_name = myPref.getString("DHTSensor_hum_name", "humedad");
  portsNames.DHTSensor_temp_name = myPref.getString("DHTSensor_temp_name", "temperatura");
  portsNames.TempSensor_name = myPref.getString("TempSensor_name", "temperatura");
  portsNames.d0_name = myPref.getString("d0_name", "digital0");
  portsNames.d1_name = myPref.getString("d1_name", "digital1");
  portsNames.d2_name = myPref.getString("d2_name", "digital2");
  portsNames.d3_name = myPref.getString("d3_name", "digital3");

  MQTTDHT = std::stoll(myPref.getString("MQTTDHT", "50000").c_str());
  MQTTsingleTemp = std::stoll(myPref.getString("MQTTsingleTemp", "30000").c_str());
  keepAliveTime = std::stoll(myPref.getString("keepAliveTime", "60000").c_str());
  myPref.end();
  
  Serial.println("#####################################################################");
  Serial.print("\n## Current Config:\n## Device Name: " + deviceName);
  Serial.print("\n## UID: " + String(ESP.getChipId()));
  Serial.print("\n## IP: " + staticIpAP);
  Serial.print("\n## Subnet: " + subnetMaskAP);
  Serial.print("\n## Gateway: " + gatewayAP);
  Serial.print("\n## IO0 config: " + IO_0);
  Serial.print("\n## IO1 config: " + IO_1);
  Serial.print("\n## IO2 config: " + IO_2);
  Serial.print("\n## IO3 config: " + IO_3);
  Serial.print("\n## IO0 name: " + portsNames.d0_name);
  Serial.print("\n## IO1 name: " + portsNames.d1_name);
  Serial.print("\n## IO2 name: " + portsNames.d2_name);
  Serial.print("\n## IO3 name: " + portsNames.d3_name);
  Serial.print("\n## DHT humidity name: " + portsNames.DHTSensor_hum_name);
  Serial.print("\n## DHT temperature name: " + portsNames.DHTSensor_temp_name);
  Serial.print("\n## Temperature sensor name: " + portsNames.TempSensor_name);
  Serial.print("\n## DHT sensor sending time: " + String(MQTTDHT));
  Serial.print("\n## Single sensor sending time: " + String(MQTTsingleTemp));
Serial.println("\n## Keep alive sending time: " + String(keepAliveTime));
  Serial.println("#####################################################################");
}

#endif
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
#ifndef PREFERENCES

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
    restoreConfig(LittleFS);
  }

  // Serial.println(config.size());

  deviceName = (const char *)config["device"]["name"];

  staticIpAP = (const char *)config["network"]["ip"];
  subnetMaskAP = (const char *)config["network"]["subnetMask"];
  gatewayAP = (const char *)config["network"]["gateway"];

  SmtpSender = (const char *)config["smtp"]["mailSender"];
  SmtpPass = (const char *)config["smtp"]["mailPassword"];
  SmtpReceiver = (const char *)config["smtp"]["mailReceiver"];
  SmtpServer = (const char *)config["smtp"]["smtpServer"];
  SmtpPort = std::stoi((const char *)config["smtp"]["smtpPort"]);

  IO_0 = (const char *)config["ports"]["IO_0"];
  IO_1 = (const char *)config["ports"]["IO_1"];
  IO_2 = (const char *)config["ports"]["IO_2"];
  IO_3 = (const char *)config["ports"]["IO_3"];

  portsNames.DHTSensor_hum_name = String((const char *)config["names"]["DHTSensor_hum_name"]);
  portsNames.DHTSensor_temp_name = String((const char *)config["names"]["DHTSensor_temp_name"]);
  portsNames.TempSensor_name = String((const char *)config["names"]["TempSensor_name"]);
  portsNames.d0_name = String((const char *)config["names"]["d0_name"]);
  portsNames.d1_name = String((const char *)config["names"]["d1_name"]);
  portsNames.d2_name = String((const char *)config["names"]["d2_name"]);
  portsNames.d3_name = String((const char *)config["names"]["d3_name"]);

  MQTTDHT = std::stoi((const char *)config["etc"]["DHT"]);
  MQTTsingleTemp = std::stoi((const char *)config["etc"]["SingleTemp"]);
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

#endif