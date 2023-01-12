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