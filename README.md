# IOT_FIRMWARE_MQTT
 
### Required Libreries

External dependencies
```
OneWire
DallasTemperature
Adafruit Unified Sensor
LiquidCrystal_I2C
ArduinoJson
Arduino_JSON
LittleFS_esp32
NTPClient
ESP Mail Client
EMailSender
ESPAsyncWebServer-esphome
DHT sensor library
ArduinoMqttClient
WiFiManager
```

### Inputs Status
All the inputs states will be shown in the following topic: DeviceData/<chipId>

### Relay
In order to change the state of the relay send a message to the topic: RelaySwich/<DeviceID>

```
{
    "state": 1/0
}
```


### Required Drivers (For windows or mac)
CP210x 
```
Available on: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
```

```

//SMTP test
if (std::atof(mySensors.singleSensorRawdataTemp(0).c_str()) >= std::atof("50"))
{
#ifdef SMTP_CLIENT
sendMail("Alerta", "You have Overpass the temperature");
#endif
}

```