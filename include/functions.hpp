#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <WiFiUdp.h>
#include <NTPClient.h>

const int utcOffset = -10800;
unsigned long previousTime2 = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.google.com", 0);

String ntpRaw()
{
    timeClient.update();

    return String(timeClient.getEpochTime());
}

/*
this function is used to re format the date and add the zeros when a date, or time
lenght is minor to 2
*/
String refactor(int time)
{
    if (time < 10)
    {
        String refactor = String("0") + String(time);
        return refactor;
    }
    else
    {
        return String(time);
    }
};

String formatedTime()
{
    time_t epochTime = timeClient.getEpochTime();

    timeClient.update();
    struct tm *ptm = gmtime((time_t *)&epochTime);

    String data = refactor(ptm->tm_mday) + "-" + refactor(ptm->tm_mon + 1) + "-" + String(ptm->tm_year + 1900) + " " + refactor(timeClient.getHours()) + ":" + refactor(timeClient.getMinutes()) + ":" + refactor(timeClient.getSeconds());
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
/*
{
  "device": {
    "UID": "296876",
    "name": "default"
  },
  "network": {
    "SSID": "default",
    "wifiPassword": "default",
    "ip": "",
    "subnetMask": "",
    "gateway": ""
  },
  "smtp": {
    "mailSender": "giulicrenna@outlook.com",
    "mailPassword": "kirchhoff2002",
    "mailReceiver": "",
    "smtpServer": "smtp.office365.com",
    "smtpPort": "587"
  },
  "ports": {
    "IO_0": "OTU",
    "IO_1": "OTD",
    "IO_2": "OTD",
    "IO_3": "OTD"
  },
  "etc": {
    "MQTTtemp": "1000",
    "MQTThum": "10000",
    "keepAlive": "600000"
  }
}
*/
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
    Serial.println(json);
    delay(2000);

    StaticJsonDocument<1024> newConfig;
    auto error = deserializeJson(newConfig, json);

    StaticJsonDocument<1024> writeConfig;

    if (error)
    {
        Serial.println("Failed to deserialize (1)");
        Serial.println(error.f_str());
        return 1;
    }

    String device_uid = newConfig["device"]["UID"];
    String device_name = newConfig["device"]["name"];
    String new_network_SSID = newConfig["network"]["SSID"];
    String new_network_wifiPassword = newConfig["network"]["wifiPassword"];
    String new_network_ip = newConfig["network"]["ip"];
    String new_network_submask = newConfig["network"]["subnetMask"];
    String new_network_gateway = newConfig["network"]["gateway"];
    String smtp_sender = newConfig["smtp"]["mailSender"];
    String smtp_passw = newConfig["smtp"]["mailPassword"];
    String smtp_receiver = newConfig["smtp"]["mailReceiver"];
    String smpt_server = newConfig["smtp"]["smtpServer"];
    String smtp_port = newConfig["smtp"]["smtpPort"];
    String ports_IO0 = newConfig["ports"]["IO_0"];
    String ports_IO1 = newConfig["ports"]["IO_1"];
    String ports_IO2 = newConfig["ports"]["IO_2"];
    String ports_IO3 = newConfig["ports"]["IO_3"];
    String ports_mqttMQTTtemp = newConfig["etc"]["MQTTtemp"];
    String ports_mqttMQTThum = newConfig["etc"]["MQTThum"];
    String ports_keepalive = newConfig["etc"]["keepAlive"];

    Serial.println("*** Starting new configuration...");

    writeConfig["device"]["UID"] = device_uid;
    writeConfig["device"]["name"] = device_name;
    writeConfig["network"]["SSID"] = new_network_SSID;
    writeConfig["network"]["wifiPassword"] = new_network_wifiPassword;
    writeConfig["network"]["ip"] = new_network_ip;
    writeConfig["network"]["subnetMask"] = new_network_submask;
    writeConfig["network"]["gateway"] = new_network_gateway;
    writeConfig["smtp"]["mailSender"] = smtp_sender;
    writeConfig["smtp"]["mailPassword"] = smtp_passw;
    writeConfig["smtp"]["mailReceiver"] = smtp_receiver;
    writeConfig["smtp"]["smtpServer"] = smpt_server;
    writeConfig["smtp"]["smtpPort"] = smtp_port;
    writeConfig["ports"]["IO_0"] = ports_IO0;
    writeConfig["ports"]["IO_1"] = ports_IO1;
    writeConfig["ports"]["IO_2"] = ports_IO2;
    writeConfig["ports"]["IO_3"] = ports_IO3;
    writeConfig["etc"]["MQTTtemp"] = ports_mqttMQTTtemp;
    writeConfig["etc"]["MQTThum"] = ports_mqttMQTThum;
    writeConfig["etc"]["keepAlive"] = ports_keepalive;

    File fileWrite = fs.open("/config.json", "w");

    Serial.println("*** Opening new file...");

    if (!fileWrite)
    {
        Serial.println("Error opening config file");
    }
    auto error3 = serializeJsonPretty(writeConfig, fileWrite);

    if (!error3)
    {
        Serial.println("Failed to Serialize (3)");
    }

    fileWrite.close();

    ESP.restart();

    return 0;
}

int restoreConfig(fs::FS &fs)
{
    Serial.println("*** Restoring default configuration ***");

    fs.remove("/config.json");
    File restoreFile = fs.open("/restore.json", "r");
    File configFile = fs.open("/config.json", "w+");

    String content;
    if (!restoreFile.available())
    {
        Serial.println("Couldn't open the file");
        return 1;
    }
    while (restoreFile.available())
    {
        content += restoreFile.readString();
        break;
    }

    configFile.print(content);

    configFile.close();
    restoreFile.close();

    return 0;
}
#endif



