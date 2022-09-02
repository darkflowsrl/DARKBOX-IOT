#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <sstream>
#include <NTPClient.h>
#include <EMailSender.h>
#include <WiFiUdp.h>
#include <string.h>

const int utcOffset = -10800;
unsigned long previousTime2 = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.south-america.pool.ntp.org", utcOffset);

void sendEmail(const char *sender, const char *password, const char *receiver, const char *host, int port)
{
    String msg = "Sending from " + String(sender) + " to " + String(receiver) + " over " + String(host);
    Serial.println(msg);
    EMailSender emailSend(sender, password, sender, "Darkflow-Device");

    emailSend.setSMTPServer(host);
    emailSend.setSMTPPort((uint16_t)port);

    EMailSender::EMailMessage message;
    message.subject = "Alerta";
    message.message = "El sensor superó la temperatura dada";

    EMailSender::Response response = emailSend.send(receiver, message);

    Serial.println(response.status);
    Serial.println(response.code);
    Serial.println(response.desc);
}

String ntpRawNoDay()
{
    timeClient.update();

    String minutes, seconds, hour;

    if (String(timeClient.getMinutes()).length() == 1)
    {
        minutes = "0" + String(timeClient.getMinutes());
    }
    else
    {
        minutes = String(timeClient.getMinutes());
    }
    if (String(timeClient.getHours()).length() == 1)
    {
        hour = "0" + String(timeClient.getHours());
    }
    else
    {
        hour = String(timeClient.getHours());
    }
    if (String(timeClient.getSeconds()).length() == 1)
    {
        seconds = "0" + String(timeClient.getSeconds());
    }
    else
    {
        seconds = String(timeClient.getSeconds());
    }

    String data = hour + ":" + minutes + ":" + seconds;

    return data;
}

String formatedTime()
{
    time_t epochTime = timeClient.getEpochTime();

    timeClient.update();
    struct tm *ptm = gmtime((time_t *)&epochTime);

    uint16_t monthDay = ptm->tm_mday;
    uint16_t currentMonth = ptm->tm_mon + 1;
    uint16_t currentYear = ptm->tm_year + 1900;

    String data = String(monthDay) + "-" + String(currentMonth) + "-" + String(currentYear) + " " + String(timeClient.getHours()) + ":" + String(timeClient.getMinutes()) + ":" + String(timeClient.getSeconds());
    return data;
}

IPAddress strToIp(String miIp)
{

    std::stringstream s(miIp.c_str());

    int oct0, oct1, oct2, oct3; // to store the 4 ints

    char ch; // to temporarily store the '.'

    s >> oct0 >> ch >> oct1 >> ch >> oct2 >> ch >> oct3;

    return IPAddress(oct0, oct1, oct2, oct3);
}

/**
 * @brief
 *
 * @param fs
 * @param json
 * @return int
 */
int updateConfig(fs::FS &fs, const char *json)
{
    Serial.println("UPDATING CONFIGURATION");

    delay(2000);

    File fileRead = fs.open("/config.json", "r");

    Serial.println("config.json opened");
    delay(1000);

    String content;

    while (fileRead.available())
    {
        content += fileRead.readString();
        break;
    }

    Serial.println("Content Loaded");
    delay(1000);

    StaticJsonDocument<1024> actualConfig;
    auto error = deserializeJson(actualConfig, content);
    StaticJsonDocument<1024> newConfig;
    auto error2 = deserializeJson(newConfig, json);

    StaticJsonDocument<1024> writeConfig;

    if (error)
    {
        Serial.println("Failed to deserialize (1)");
        Serial.println(error.f_str());
        return 1;
    }

    if (error2)
    {
        Serial.println("Failed to deserialize (2)");
        Serial.println(error.f_str());
        return 1;
    }

    fileRead.close();
    File fileWrite = fs.open("/config.json", "w");

    writeConfig["device"]["UID"] = actualConfig["device"]["UID"];
    writeConfig["device"]["name"] = actualConfig["device"]["name"];
    writeConfig["network"]["SSID"] = actualConfig["network"]["SSID"];
    writeConfig["smtp"]["mailSender"] = actualConfig["smtp"]["mailSender"];
    writeConfig["smtp"]["mailPassword"] = actualConfig["smtp"]["mailPassword"];
    writeConfig["smtp"]["mailReceiver"] = actualConfig["smtp"]["mailReceiver"];
    writeConfig["smtp"]["smtpServer"] = actualConfig["smtp"]["smtpServer"];
    writeConfig["smtp"]["smtpPort"] = actualConfig["smtp"]["smtpPort"];

    if (newConfig["network"]["SSID"] == "None")
    {
        writeConfig["network"]["SSID"] = actualConfig["network"]["SSID"];
    }
    else
    {
        writeConfig["network"]["SSID"] = newConfig["network"]["SSID"];
    }
    if (newConfig["network"]["wifiPassword"] == "None")
    {
        writeConfig["network"]["wifiPassword"] = actualConfig["network"]["wifiPassword"];
    }
    else
    {
        writeConfig["network"]["wifiPassword"] = newConfig["network"]["wifiPassword"];
    }
    if (newConfig["network"]["ip"] == "None")
    {
        writeConfig["network"]["ip"] = actualConfig["network"]["ip"];
    }
    else
    {
        writeConfig["network"]["ip"] = newConfig["network"]["ip"];
    }
    if (newConfig["network"]["subnetMask"] == "None")
    {
        writeConfig["network"]["subnetMask"] = actualConfig["network"]["subnetMask"];
    }
    else
    {
        writeConfig["network"]["subnetMask"] = newConfig["network"]["subnetMask"];
    }
    if (newConfig["network"]["gateway"] == "None")
    {
        writeConfig["network"]["gateway"] = actualConfig["network"]["gateway"];
    }
    else
    {
        writeConfig["network"]["gateway"] = newConfig["network"]["gateway"];
    }

    if (newConfig["mqtt"]["host"] == "None")
    {
        writeConfig["mqtt"]["host"] = actualConfig["mqtt"]["host"];
    }
    else
    {
        writeConfig["mqtt"]["host"] = newConfig["mqtt"]["host"];
    }
    if (newConfig["mqtt"]["root_topic_subscribe"] == "None")
    {
        writeConfig["mqtt"]["root_topic_subscribe"] = actualConfig["mqtt"]["root_topic_subscribe"];
    }
    else
    {
        writeConfig["mqtt"]["root_topic_subscribe"] = newConfig["mqtt"]["root_topic_subscribe"];
    }
    if (newConfig["mqtt"]["root_topic_publish"] == "None")
    {
        writeConfig["mqtt"]["root_topic_publish"] = actualConfig["mqtt"]["root_topic_publish"];
    }
    else
    {
        writeConfig["mqtt"]["root_topic_publish"] = newConfig["mqtt"]["root_topic_publish"];
    }
    if (newConfig["mqtt"]["keep_alive_topic_publish"] == "None")
    {
        writeConfig["mqtt"]["keep_alive_topic_publish"] = actualConfig["mqtt"]["keep_alive_topic_publish"];
    }
    else
    {
        writeConfig["mqtt"]["keep_alive_topic_publish"] = newConfig["mqtt"]["keep_alive_topic_publish"];
    }
    if (newConfig["mqtt"]["port"] == "None")
    {
        writeConfig["mqtt"]["port"] = actualConfig["mqtt"]["port"];
    }
    else
    {
        writeConfig["mqtt"]["port"] = newConfig["mqtt"]["port"];
    }

    if (newConfig["refreshTimes"]["MQTTmsg"] == "None")
    {
        writeConfig["refreshTimes"]["MQTTmsg"] = actualConfig["MQTTmsg"]["port"];
    }
    else
    {
        writeConfig["refreshTimes"]["MQTTmsg"] = newConfig["MQTTmsg"]["port"];
    }
    if (newConfig["refreshTimes"]["keepAlive"] == "None")
    {
        writeConfig["refreshTimes"]["keepAlive"] = actualConfig["keepAlive"]["port"];
    }
    else
    {
        writeConfig["refreshTimes"]["keepAlive"] = newConfig["keepAlive"]["port"];
    }

    if (newConfig["refreshTimes"]["temporalData"] == "None")
    {
        writeConfig["refreshTimes"]["temporalData"] = actualConfig["MQTTmsg"]["temporalData"];
    }
    else
    {
        writeConfig["refreshTimes"]["temporalData"] = newConfig["MQTTmsg"]["temporalData"];
    }

    auto error3 = serializeJsonPretty(writeConfig, fileWrite);

    if (error3)
    {
        Serial.println("Failed to Serialize (3)");
        return 1;
    }

    fileWrite.close();

    return 0;
}
/*
New Config JSON Structure

{
  "network": {
    "SSID": "Chop",
    "wifiPassword": "741963258",
    "ip": "192.168.1.67",
    "subnetMask": "255.255.255.0",
    "gateway": "192.168.1.1"
  },
  "mqtt": {
    "host": "None",
    "root_topic_subscribe": "None",
    "root_topic_publish": "None",
    "keep_alive_topic_publish" : "None",
    "port": "None"
  },
  "refreshTimes": {
    "MQTTmsg" : "1000",
    "keepAlive" : "600000",
    "temporalData" : "10000"
  }
}

*/

#endif