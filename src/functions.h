#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <EMailSender.h>

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

    String device_UID = actualConfig["device"]["UID"];
    String device_name = actualConfig["device"]["name"];
    String network_ssid = actualConfig["network"]["SSID"];
    String smtp_sender = actualConfig["smtp"]["mailSender"];
    String smtp_pass = actualConfig["smtp"]["mailPassword"];
    String smtp_receiver = actualConfig["smtp"]["mailReceiver"];
    String smtp_server = actualConfig["smtp"]["smtpServer"];
    String smtp_port = actualConfig["smtp"]["smtpPort"];

    String network_SSID = actualConfig["network"]["SSID"];
    String network_wifiPassword = actualConfig["network"]["wifiPassword"];
    String network_ip = actualConfig["network"]["ip"];
    String network_submask = actualConfig["network"]["subnetMask"];
    String network_gateway = actualConfig["network"]["gateway"];
    String mqtt_host = actualConfig["mqtt"]["host"];
    String mqtt_port = actualConfig["mqtt"]["port"];
    String refresh_mqttmsg = actualConfig["refreshTimes"]["MQTTmsg"];
    String refresh_keepalive = actualConfig["refreshTimes"]["keepAlive"];

    String new_network_SSID = newConfig["network"]["SSID"];
    String new_network_wifiPassword = newConfig["network"]["wifiPassword"];
    String new_network_ip = newConfig["network"]["ip"];
    String new_network_submask = newConfig["network"]["subnetMask"];
    String new_network_gateway = newConfig["network"]["gateway"];
    String new_mqtt_host = newConfig["mqtt"]["host"];
    String new_mqtt_port = newConfig["mqtt"]["port"];
    String new_refresh_mqttmsg = newConfig["refreshTimes"]["MQTTmsg"];
    String new_refresh_keepalive = newConfig["refreshTimes"]["keepAlive"];


    Serial.println("*** Starting new configuration...");

    writeConfig["device"]["UID"] = device_UID;
    writeConfig["device"]["name"] = device_name;
    writeConfig["network"]["SSID"] = network_ssid;
    writeConfig["smtp"]["mailSender"] = smtp_sender;
    writeConfig["smtp"]["mailPassword"] = smtp_pass;
    writeConfig["smtp"]["mailReceiver"] = smtp_receiver;
    writeConfig["smtp"]["smtpServer"] = smtp_server;
    writeConfig["smtp"]["smtpPort"] = smtp_port;

    if (new_network_SSID == "None")
    {
        writeConfig["network"]["SSID"] = network_SSID;
    }
    else
    {
        writeConfig["network"]["SSID"] = new_network_SSID;
    }

    if (new_network_wifiPassword == "None")
    {
        writeConfig["network"]["wifiPassword"] = network_wifiPassword;
    }
    else
    {
        writeConfig["network"]["wifiPassword"] = new_network_wifiPassword;
    }

    if (new_network_ip == "None")
    {
        writeConfig["network"]["ip"] = network_ip;
    }
    else
    {
        writeConfig["network"]["ip"] = new_network_ip;
    }

    if (new_network_submask == "None")
    {
        writeConfig["network"]["subnetMask"] = network_submask;
    }
    else
    {
        writeConfig["network"]["subnetMask"] = new_network_submask;
    }

    if (new_network_gateway == "None")
    {
        writeConfig["network"]["gateway"] = network_gateway;
    }
    else
    {
        writeConfig["network"]["gateway"] = new_network_gateway;
    }

    if (new_mqtt_host == "None")
    {
        writeConfig["mqtt"]["host"] = mqtt_host;
    }
    else
    {
        writeConfig["mqtt"]["host"] = new_mqtt_host;
    }

    if (new_mqtt_port == "None")
    {
        writeConfig["mqtt"]["port"] = mqtt_port;
    }
    else
    {
        writeConfig["mqtt"]["port"] = new_mqtt_port;
    }

    if (new_refresh_mqttmsg == "None")
    {
        writeConfig["refreshTimes"]["MQTTmsg"] = refresh_mqttmsg;
    }
    else
    {
        writeConfig["refreshTimes"]["MQTTmsg"] = new_refresh_mqttmsg;
    }

    if (new_refresh_keepalive == "None")
    {
        writeConfig["refreshTimes"]["keepAlive"] = refresh_keepalive;
    }
    else
    {
        writeConfig["refreshTimes"]["keepAlive"] = new_refresh_keepalive;
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

#endif