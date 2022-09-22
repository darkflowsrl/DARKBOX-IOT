#include "myMqtt.h"
/*
pinMode(switchUpPin, INPUT_PULLUP)

extraPin0 -> Input
extraPin1 -> Input
extraPin2 -> Input
extraPin3 -> Input
extraPin4 -> Output Salida 4
extraPin5 -> Output Salida 3
extraPin6 -> Output Salida 2
extraPin7 -> Output Salida 1
*/

#define Input0 13
#define Input1 12
#define Input2 14
#define Input3 16
#define Output1 15

bool OTU_IO0;
bool OTU_IO1;
bool OTU_IO2;
bool OTU_IO3;

bool OTD_IO0;
bool OTD_IO1;
bool OTD_IO2;
bool OTD_IO3;

unsigned int T_IO0;
unsigned int T_IO1;
unsigned int T_IO2;
unsigned int T_IO3;

int IO_0_State = 0;
int IO_1_State = 0;
int IO_2_State = 0;
int IO_3_State = 0;

int last_IO_0_State = 0;
int last_IO_1_State = 0;
int last_IO_2_State = 0;
int last_IO_3_State = 0;

int previousTime_IO_0 = 0;
int previousTime_IO_1 = 0;
int previousTime_IO_2 = 0;
int previousTime_IO_3 = 0;

void checkReset(std::string inputJson)
{
    StaticJsonDocument<1024> config;
    auto error = deserializeJson(config, inputJson.c_str());
    if (error)
    {
        Serial.println("Failed to deserialize");
        Serial.println(error.f_str());
    }
    if (config["Value"]["Input_2"] == "HIGH")
    {
        for (int i = 0; i <= 2; i++)
        {
            delay(1000);
            if (i == 2)
            {
                Serial.println("*** Resetting WiFi credentials ***");
                myScreenAp.printScreen("Resetting Device ", 0, 1, true);
                delay(5000);
                myManager.resetSettings();
                ESP.eraseConfig();
                ESP.reset();
                ESP.restart();
            }
        }
    }
}

class inputController
{
private:
    WiFiClient wifiClient;

public:
    void inputSetup()
    {
        pinMode(Input0, INPUT_PULLUP);
        pinMode(Input1, INPUT_PULLUP);
        pinMode(Input2, INPUT_PULLUP);
        pinMode(Input3, INPUT_PULLUP);
        pinMode(Output1, OUTPUT);
        // Serial.println(IO_0 + " " + IO_1 + " " + IO_2 + " " + IO_3);
        if (IO_0 == "OTU" || IO_0 == "OTD")
        {
            if (IO_0 == "OTU")
            {
                OTU_IO0 = true;
                OTD_IO0 = false;
            }
            else
            {
                OTU_IO0 = false;
                OTD_IO0 = true;
            }
        }
        else
        {
            OTU_IO0 = false;
            OTD_IO0 = false;
            T_IO0 = std::stoi(IO_0.c_str());
        }

        if (IO_1 == "OTU" || IO_1 == "OTD")
        {
            if (IO_1 == "OTU")
            {
                OTU_IO1 = true;
                OTD_IO1 = false;
            }
            else
            {
                OTU_IO1 = false;
                OTD_IO1 = true;
            }
        }
        else
        {
            OTU_IO1 = false;
            OTD_IO2 = false;
            T_IO1 = std::stoi(IO_1.c_str());
        }

        if (IO_2 == "OTU" || IO_2 == "OTD")
        {
            if (IO_2 == "OTU")
            {
                OTU_IO2 = true;
                OTD_IO2 = false;
            }
            else
            {
                OTU_IO2 = false;
                OTD_IO2 = true;
            }
        }
        else
        {
            OTU_IO2 = false;
            OTD_IO2 = false;
            T_IO2 = std::stoi(IO_2.c_str());
        }

        if (IO_3 == "OTU" || IO_3 == "OTD")
        {
            if (IO_3 == "OTU")
            {
                OTU_IO3 = true;
                OTD_IO3 = false;
            }
            else
            {
                OTU_IO3 = false;
                OTD_IO3 = true;
            }
        }
        else
        {
            OTU_IO3 = false;
            OTD_IO3 = false;
            T_IO3 = std::stoi(IO_3.c_str());
        }
        Serial.println("( ) IO's Configured Succesfully");
    }
    void onTriggerFlag(int Input, String IO_name, bool ascendant = true)
    {
        if (IO_name == "Input_0")
        {
            IO_0_State = digitalRead(Input);

            if (IO_0_State != last_IO_0_State)
            {
                if (ascendant)
                {
                    String message;
                    DynamicJsonDocument data(512);
                    data["DeviceId"] = String(ESP.getChipId());
                    data["DeviceName"] = deviceName.c_str();
                    data["Timestamp"] = formatedTime();
                    data["MsgType"] = "Data";
                    data["Value"][0]["Port"] = IO_name;
                    data["Value"][0]["Value"] = "HIGH";
                    serializeJsonPretty(data, message);
                    Serial.println(message);
                    mqttOnLoop(host.c_str(), port, root_topic_publish.c_str(), wifiClient, keep_alive_topic_publish.c_str(), keep_alive_topic_publish.c_str(),
                               message.c_str());
                }
                else
                {
                    String message;
                    DynamicJsonDocument data(512);
                    data["DeviceId"] = String(ESP.getChipId());
                    data["DeviceName"] = deviceName.c_str();
                    data["Timestamp"] = formatedTime();
                    data["Value"][0]["Port"] = IO_name;
                    data["Value"][0]["Value"] = "LOW";
                    serializeJsonPretty(data, message);
                    Serial.println(message);
                    mqttOnLoop(host.c_str(), port, root_topic_publish.c_str(), wifiClient, keep_alive_topic_publish.c_str(), keep_alive_topic_publish.c_str(),
                               message.c_str());
                }
            }
            last_IO_0_State = IO_0_State;
        }
        else if (IO_name == "Input_1")
        {
            IO_1_State = digitalRead(Input);

            if (IO_1_State != last_IO_1_State)
            {
                if (ascendant)
                {
                    String message;
                    DynamicJsonDocument data(512);
                    data["DeviceId"] = String(ESP.getChipId());
                    data["DeviceName"] = deviceName.c_str();
                    data["Timestamp"] = formatedTime();
                    data["MsgType"] = "Data";
                    data["Value"][0]["Port"] = IO_name;
                    data["Value"][0]["Value"] = "HIGH";
                    serializeJsonPretty(data, message);
                    Serial.println(message);
                    mqttOnLoop(host.c_str(), port, root_topic_publish.c_str(), wifiClient, keep_alive_topic_publish.c_str(), keep_alive_topic_publish.c_str(),
                               message.c_str());
                }
                else
                {
                    String message;
                    DynamicJsonDocument data(512);
                    data["DeviceId"] = String(ESP.getChipId());
                    data["DeviceName"] = deviceName.c_str();
                    data["Timestamp"] = formatedTime();
                    data["Value"][0]["Port"] = IO_name;
                    data["Value"][0]["Value"] = "LOW";
                    serializeJsonPretty(data, message);
                    Serial.println(message);
                    mqttOnLoop(host.c_str(), port, root_topic_publish.c_str(), wifiClient, keep_alive_topic_publish.c_str(), keep_alive_topic_publish.c_str(),
                               message.c_str());
                }
            }
            last_IO_1_State = IO_1_State;
        }
        if (IO_name == "Input_2")
        {
            IO_2_State = digitalRead(Input);

            if (IO_2_State != last_IO_2_State)
            {
                if (ascendant)
                {
                    String message;
                    DynamicJsonDocument data(512);
                    data["DeviceId"] = String(ESP.getChipId());
                    data["DeviceName"] = deviceName.c_str();
                    data["Timestamp"] = formatedTime();
                    data["MsgType"] = "Data";
                    data["Value"][0]["Port"] = IO_name;
                    data["Value"][0]["Value"] = "HIGH";
                    serializeJsonPretty(data, message);
                    Serial.println(message);
                    mqttOnLoop(host.c_str(), port, root_topic_publish.c_str(), wifiClient, keep_alive_topic_publish.c_str(), keep_alive_topic_publish.c_str(),
                               message.c_str());
                }
                else
                {
                    String message;
                    DynamicJsonDocument data(512);
                    data["DeviceId"] = String(ESP.getChipId());
                    data["DeviceName"] = deviceName.c_str();
                    data["Timestamp"] = formatedTime();
                    data["MsgType"] = "Data";
                    data["Value"][0]["Port"] = IO_name;
                    data["Value"][0]["Value"] = "LOW";
                    serializeJsonPretty(data, message);
                    Serial.println(message);
                    mqttOnLoop(host.c_str(), port, root_topic_publish.c_str(), wifiClient, keep_alive_topic_publish.c_str(), keep_alive_topic_publish.c_str(),
                               message.c_str());
                }
            }
            last_IO_2_State = IO_2_State;
        }
        if (IO_name == "Input_3")
        {
            IO_3_State = digitalRead(Input);

            if (IO_3_State != last_IO_3_State)
            {
                if (ascendant)
                {
                    String message;
                    DynamicJsonDocument data(512);
                    data["DeviceId"] = String(ESP.getChipId());
                    data["DeviceName"] = deviceName.c_str();
                    data["Timestamp"] = formatedTime();
                    data["MsgType"] = "Data";
                    data["Value"][0]["Port"] = IO_name;
                    data["Value"][0]["Value"] = "HIGH";
                    serializeJsonPretty(data, message);
                    Serial.println(message);
                    mqttOnLoop(host.c_str(), port, root_topic_publish.c_str(), wifiClient, keep_alive_topic_publish.c_str(), keep_alive_topic_publish.c_str(),
                               message.c_str());
                }
                else
                {
                    String message;
                    DynamicJsonDocument data(512);
                    data["DeviceId"] = String(ESP.getChipId());
                    data["DeviceName"] = deviceName.c_str();
                    data["Timestamp"] = formatedTime();
                    data["MsgType"] = "Data";
                    data["Value"][0]["Port"] = IO_name;
                    data["Value"][0]["Value"] = "LOW";
                    serializeJsonPretty(data, message);
                    Serial.println(message);
                    mqttOnLoop(host.c_str(), port, root_topic_publish.c_str(), wifiClient, keep_alive_topic_publish.c_str(), keep_alive_topic_publish.c_str(),
                               message.c_str());
                }
            }
            last_IO_3_State = IO_3_State;
        }
    }
    void inputData()
    {

        std::vector<std::string> myVector;

        if (OTU_IO0 || OTD_IO0)
        {
            if (OTU_IO0)
            {
                onTriggerFlag(Input0, "Input_0");
            }
            else
            {
                onTriggerFlag(Input0, "Input_0", false);
            }
        }
        else
        {
            if (millis() - previousTime_IO_0 > T_IO0)
            {
                previousTime_IO_0 = millis();
                String message;
                DynamicJsonDocument data(512);
                data["DeviceId"] = String(ESP.getChipId());
                data["DeviceName"] = deviceName.c_str();
                data["Timestamp"] = formatedTime();
                data["MsgType"] = "Data";
                if (!digitalRead(Input0))
                {
                    data["Value"][0]["Port"] = "Input_0";
                    data["Value"][0]["Value"] = "HIGH";
                }
                else
                {
                    data["Value"][0]["Port"] = "Input_0";
                    data["Value"][0]["Value"] = "LOW";
                }

                serializeJsonPretty(data, message);
                Serial.println(message);
                mqttOnLoop(host.c_str(), port, root_topic_publish.c_str(), wifiClient, keep_alive_topic_publish.c_str(), keep_alive_topic_publish.c_str(),
                           message.c_str());
            }
        }

        if (OTU_IO1 || OTD_IO1)
        {
            if (OTU_IO1)
            {
                onTriggerFlag(Input1, "Input_1");
            }
            else
            {
                onTriggerFlag(Input1, "Input_1", false);
            }
        }
        else
        {
            if (millis() - previousTime_IO_1 > T_IO1)
            {
                previousTime_IO_1 = millis();
                String message;
                DynamicJsonDocument data(512);
                data["DeviceId"] = String(ESP.getChipId());
                data["DeviceName"] = deviceName.c_str();
                data["Timestamp"] = formatedTime();
                data["MsgType"] = "Data";
                if (!digitalRead(Input1))
                {
                    data["Value"][0]["Port"] = "Input_1";
                    data["Value"][0]["Value"] = "HIGH";
                }
                else
                {
                    data["Value"][0]["Port"] = "Input_1";
                    data["Value"][0]["Value"] = "LOW";
                }

                serializeJsonPretty(data, message);
                Serial.println(message);
                mqttOnLoop(host.c_str(), port, root_topic_publish.c_str(), wifiClient, keep_alive_topic_publish.c_str(), keep_alive_topic_publish.c_str(),
                           message.c_str());
            }
        }

        if (OTU_IO2 || OTD_IO2)
        {
            if (OTU_IO2)
            {
                onTriggerFlag(Input2, "Input_2");
            }
            else
            {
                onTriggerFlag(Input2, "Input_2", false);
            }
        }
        else
        {
            if (millis() - previousTime_IO_2 > T_IO2)
            {
                previousTime_IO_2 = millis();
                String message;
                DynamicJsonDocument data(512);
                data["DeviceId"] = String(ESP.getChipId());
                data["DeviceName"] = deviceName.c_str();
                data["Timestamp"] = formatedTime();
                data["MsgType"] = "Data";
                if (!digitalRead(Input2))
                {
                    data["Value"][0]["Port"] = "Input_2";
                    data["Value"][0]["Value"] = "HIGH";
                }
                else
                {
                    data["Value"][0]["Port"] = "Input_2";
                    data["Value"][0]["Value"] = "LOW";
                }

                serializeJsonPretty(data, message);
                Serial.println(message);
                // Device hard reset check
                checkReset(message.c_str());
                mqttOnLoop(host.c_str(), port, root_topic_publish.c_str(), wifiClient, keep_alive_topic_publish.c_str(), keep_alive_topic_publish.c_str(),
                           message.c_str());
            }
        }

        if (OTU_IO3 || OTD_IO3)
        {
            if (OTU_IO3)
            {
                onTriggerFlag(Input3, "Input_3");
            }
            else
            {
                onTriggerFlag(Input3, "Input_3", false);
            }
        }
        else
        {
            if (millis() - previousTime_IO_3 > T_IO3)
            {
                previousTime_IO_3 = millis();
                String message;
                DynamicJsonDocument data(512);
                data["DeviceId"] = String(ESP.getChipId());
                data["DeviceName"] = deviceName.c_str();
                data["Timestamp"] = formatedTime();
                data["MsgType"] = "Data";
                if (!digitalRead(Input3))
                {
                    data["Value"][0]["Port"] = "Input_3";
                    data["Value"][0]["Value"] = "HIGH";
                }
                else
                {
                    data["Value"][0]["Port"] = "Input_3";
                    data["Value"][0]["Value"] = "LOW";
                }

                serializeJsonPretty(data, message);
                Serial.println(message);
                mqttOnLoop(host.c_str(), port, root_topic_publish.c_str(), wifiClient, keep_alive_topic_publish.c_str(), keep_alive_topic_publish.c_str(),
                           message.c_str());
            }
        }
    }
    void readInputs()
    {
        if (!digitalRead(Input1))
        {
            digitalWrite(Output1, HIGH);
        }
        if (!digitalRead(Input2))
        {
            digitalWrite(Output1, HIGH);
        }
        if (!digitalRead(Input3))
        {
            digitalWrite(Output1, HIGH);
        }
        if (!digitalRead(Input0))
        {
            digitalWrite(Output1, HIGH);
        }
        if (digitalRead(Input1) && digitalRead(Input2) && digitalRead(Input3) && digitalRead(Input0))
        {
            digitalWrite(Output1, LOW);
        }
    }
    std::string returnSingleInput(uint8_t customInput)
    {
        if (!digitalRead(customInput))
        {
            return "HIGH";
        }
        return "LOW";
    }
};