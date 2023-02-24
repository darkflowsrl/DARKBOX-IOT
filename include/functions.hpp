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
    // timeClient.update();
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

/**
 * @brief
 *
 * @param fs
 * @param json
 * @return void
 */
void updateConfig(fs::FS &fs, String json)
{
#ifdef PREFERENCES
    myPref.begin("EPM", false);
    Serial.println("UPDATING CONFIGURATION");
    Serial.println(json);

    StaticJsonDocument<1512> newConfig;
    deserializeJson(newConfig, json);

    // myPref.putString("", (const char*)newConfig["device"]["UID"]);
    myPref.putString("deviceName", (const char *)newConfig["device"]["name"]);

    myPref.putString("ssid", (const char *)newConfig["network"]["SSID"]);
    myPref.putString("wifiPassword", (const char *)newConfig["network"]["wifiPassword"]);
    myPref.putString("staticIpAP", (const char *)newConfig["network"]["ip"]);
    myPref.putString("subnetMaskAP", (const char *)newConfig["network"]["subnetMask"]);
    myPref.putString("gatewayAP", (const char *)newConfig["network"]["gateway"]);

    myPref.putString("SmtpSender", (const char *)newConfig["smtp"]["mailSender"]);
    myPref.putString("SmtpPass", (const char *)newConfig["smtp"]["mailPassword"]);
    myPref.putString("SmtpReceiver", (const char *)newConfig["smtp"]["mailReceiver"]);
    myPref.putString("SmtpServer", (const char *)newConfig["smtp"]["smtpServer"]);
    myPref.putString("SmtpPort", (const char *)newConfig["smtp"]["smtpPort"]);

    myPref.putString("IO_0", (const char *)newConfig["ports"]["IO_0"]);
    myPref.putString("IO_1", (const char *)newConfig["ports"]["IO_1"]);
    myPref.putString("IO_2", (const char *)newConfig["ports"]["IO_2"]);
    myPref.putString("IO_3", (const char *)newConfig["ports"]["IO_3"]);

    myPref.putString("DHTSensor_hum_name", (const char *)newConfig["names"]["DHTSensor_hum_name"]);
    myPref.putString("DHTSensor_temp_name", (const char *)newConfig["names"]["DHTSensor_temp_name"]);
    myPref.putString("TempSensor_name", (const char *)newConfig["names"]["TempSensor_name"]);
    myPref.putString("d0_name", (const char *)newConfig["names"]["d0_name"]);
    myPref.putString("d1_name", (const char *)newConfig["names"]["d1_name"]);
    myPref.putString("d2_name", (const char *)newConfig["names"]["d2_name"]);
    myPref.putString("d3_name", (const char *)newConfig["names"]["d3_name"]);

    myPref.putString("MQTTDHT", (const char *)newConfig["etc"]["DHT"]);
    myPref.putString("keepAliveTime", (const char *)newConfig["etc"]["keepAliveTime"]);
    myPref.putString("MQTTsingleTemp", (const char *)newConfig["etc"]["SingleTemp"]);
    myPref.end();

    ESP.restart();
#endif

#ifndef PREFERENCES
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
#endif
}

int restoreConfig(fs::FS &fs)
{
#ifdef PREFERENCES
    myPref.begin("EPM", false);
    Serial.println("*** Restoring default configuration ***");

    myPref.putString("deviceName", "default");
    myPref.putString("staticIpAP", "");
    myPref.putString("subnetMaskAP", "");
    myPref.putString("gatewayAP", "");
    myPref.putString("SmtpSender", "default@outlook.com");
    myPref.putString("SmtpPass", "default123");
    myPref.putString("SmtpReceiver", "default@outlook.com");
    myPref.putString("SmtpServer", "smtp.default.com");
    myPref.putString("SmtpPort", "587");
    myPref.putString("IO_0", "OTU");
    myPref.putString("IO_1", "OTU");
    myPref.putString("IO_2", "OTU");
    myPref.putString("IO_3", "OTU");
    myPref.putString("DHTSensor_hum_name", "humedad");
    myPref.putString("DHTSensor_temp_name", "temperatura");
    myPref.putString("TempSensor_name", "temperatura");
    myPref.putString("d0_name", "digital0");
    myPref.putString("d1_name", "digital1");
    myPref.putString("d2_name", "digital2");
    myPref.putString("d3_name", "digital3");
    myPref.putString("MQTTDHT", "50000");
    myPref.putString("MQTTsingleTemp", "30000");
    myPref.putString("keepAliveTime", "70000");

    myPref.end();
    ESP.restart();
    return 0;
#endif
#ifndef PREFERENCES
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

    fs.end();t

    return 0;
#endif
}

float readBatteryLevel()
{
    float tempC;
    float batt_volts;
    float temp_var = 0; // holds cumulative reads for averaging
    unsigned int batt_raw;

    for (int i = 0; i <= 9; i++)
    {
        temp_var = temp_var + analogRead(A0);
        delay(1);
    }

    batt_raw = temp_var / 10;

    batt_volts = batt_raw * 0.0146; // scalling factor for 15V full scale with ADC range of 1V

    return batt_volts;
}

#endif
