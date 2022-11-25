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
int updateConfig(fs::FS &fs, String json)
{
    if (fs.begin())
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

        Serial.println("*** Starting new configuration...");

        writeConfig["device"]["UID"] = (const char *)newConfig["device"]["UID"];
        writeConfig["device"]["name"] = (const char *)newConfig["device"]["name"];
        writeConfig["network"]["SSID"] = (const char *)newConfig["network"]["SSID"];
        writeConfig["network"]["wifiPassword"] = (const char *)newConfig["network"]["wifiPassword"];
        writeConfig["network"]["ip"] = (const char *)newConfig["network"]["ip"];
        writeConfig["network"]["subnetMask"] = (const char *)newConfig["network"]["subnetMask"];
        writeConfig["network"]["gateway"] = (const char *)newConfig["network"]["gateway"];
        writeConfig["smtp"]["mailSender"] = (const char *)newConfig["smtp"]["mailSender"];
        writeConfig["smtp"]["mailPassword"] = (const char *)newConfig["smtp"]["mailPassword"];
        writeConfig["smtp"]["mailReceiver"] = (const char *)newConfig["smtp"]["mailReceiver"];
        writeConfig["smtp"]["smtpServer"] = (const char *)newConfig["smtp"]["smtpServer"];
        writeConfig["smtp"]["smtpPort"] = (const char *)newConfig["smtp"]["smtpPort"];
        writeConfig["ports"]["IO_0"] = (const char *)newConfig["ports"]["IO_0"];
        writeConfig["ports"]["IO_1"] = (const char *)newConfig["ports"]["IO_1"];
        writeConfig["ports"]["IO_2"] = (const char *)newConfig["ports"]["IO_2"];
        writeConfig["ports"]["IO_3"] = (const char *)newConfig["ports"]["IO_3"];
        writeConfig["names"]["DHTSensor_hum_name"] = (const char *)newConfig["names"]["DHTSensor_hum_name"];
        writeConfig["names"]["DHTSensor_temp_name"] = (const char *)newConfig["names"]["DHTSensor_temp_name"];
        writeConfig["names"]["TempSensor_name"] = (const char *)newConfig["names"]["TempSensor_name"];
        writeConfig["names"]["d0_name"] = (const char *)newConfig["names"]["d0_name"];
        writeConfig["names"]["d1_name"] = (const char *)newConfig["names"]["d1_name"];
        writeConfig["names"]["d2_name"] = (const char *)newConfig["names"]["d2_name"];
        writeConfig["names"]["d3_name"] = (const char *)newConfig["names"]["d3_name"];
        writeConfig["etc"]["DHT"] = (const char *)newConfig["etc"]["DHT"];
        writeConfig["etc"]["SingleTemp"] = (const char *)newConfig["etc"]["SingleTemp"];
        writeConfig["etc"]["keepAlive"] = (const char *)newConfig["etc"]["keepAlive"];

        Serial.println("*** Opening new file...");

        if (fs.remove("/config.json"))
        {
            Serial.println("*** Old config erased...");

            File fileWrite = fs.open("/config.json", "w+");

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
        }
        else
        {
            File fileWrite = fs.open("/config.json", "w");
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
        }

        fs.end();

        ESP.restart();

        return 0;
    }
    else
    {
        Serial.print("couldn't mount filesystem");
        return 1;
    }
}

int restoreConfig(fs::FS &fs)
{
    Serial.println("*** Restoring default configuration ***");

    fs.begin();
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

    fs.end();

    return 0;
}
#endif
