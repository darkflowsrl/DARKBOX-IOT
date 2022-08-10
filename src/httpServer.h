#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

const char MAIN_page[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html>

  <head>
      <title>Darkflow Web Server</title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <style>
        .invert {
          background-color: black;
          filter: invert(100%);
          }
      </style>
  </head>
  <body>
      <h1><strong>DarkFlow Device</strong></h1>
      <form action="" method="get">
          <div><label for="Sensor_1">Choose the Sensor 1: </label><select name="Sensor_1">
                  <option value="OneWire">OneWire</option>
                  <option value="DHC">Temperature and Humidity</option>
                  <option value="TAC">Tacometro</option>
                  <option value="LUX">Lux&oacute;metro</option>
              </select></div>
          <div><label for="Sensor_2">Choose the Sensor 2: </label><select name="Sensor_2">
                  <option value="OneWire">OneWire</option>
                  <option value="DHC">Temperature and Humidity</option>
                  <option value="TAC">Tacometro</option>
                  <option value="LUX">Lux&oacute;metro</option>
              </select></div>
          <div><label for="Sensor_3">Choose the Sensor 3: </label><select name="Sensor_3">
                  <option value="OneWire">OneWire</option>
                  <option value="DHC">Temperature and Humidity</option>
                  <option value="TAC">Tacometro</option>
                  <option value="LUX">Lux&oacute;metro</option>
              </select></div>
          <input type="submit" value="Configurar Salidas" />
      </form>
  </body>
  </html>
)=====";

void loadRequiredDataforHttpServer(fs::FS &fs, const char *path);
void changeCredentials(fs::FS &fs, const char *path, const char *device0, const char *device1, const char *device2);
void loadDevices(fs::FS &fs, const char *path);
void handleRoot();
void handleReset();
void handleSensor0();
void handleSensor1();
void handleSensor2();

const char *uid_;
const char *deviceName_;
const char *ssid_;
const char *passw_;
const char *staticIpAP_;
const char *subnetMaskAP_;
const char *gatewayAP_;
const char *host_;
const char *root_topic_subscribe_;
const char *root_topic_publish_;
const char *port_;
const char *smtpSender_;
const char *smtpPass_;
const char *SmtpReceiver_;
const char *SmtpServer_;
const char *SmtpPort_;
const char *sensor0_;
const char *sensor1_;
const char *sensor2_;

String localDeviceName;
String httpLocalIp;
ESP8266WebServer server(80);
MDNSResponder mDns;
WiFiClient client_;

void setupDnsServer()
{
  loadRequiredDataforHttpServer(LittleFS, "config.json");
  Serial.print("Local DNS: " + localDeviceName + ".local ");
  const char *resolutionName = localDeviceName.c_str();

  if (!mDns.begin(resolutionName, WiFi.localIP()))
  {
    Serial.println("(mDns instance) Error setting up DNS server");
  }
  else
  {
    Serial.println("(mDns instance) DNS server started succesfully");
  }

  mDns.addService("http", "tcp", 80);

  server.on("/", handleRoot);
  server.on("/?", handleReset);
  server.begin();
}

void setupHttpServer()
{
  mDns.update();
  server.handleClient();
}

void loadDevices(fs::FS &fs, const char *path)
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

  uid_ = (const char *)config["device"]["UID"];
  deviceName_ = (const char *)config["device"]["name"];

  ssid_ = (const char *)config["network"]["SSID"];
  passw_ = config["network"]["wifiPassword"];
  staticIpAP_ = (const char *)config["network"]["ip"];
  subnetMaskAP_ = (const char *)config["network"]["subnetMask"];
  gatewayAP_ = (const char *)config["network"]["gateway"];

  host_ = (const char *)config["mqtt"]["host"];
  root_topic_subscribe_ = (const char *)config["mqtt"]["root_topic_subscribe"];
  root_topic_publish_ = (const char *)config["mqtt"]["root_topic_publish"];
  port_ = (const char *)config["mqtt"]["port"];

  smtpSender_ = (const char *)config["smtp"]["mailSender"];
  smtpPass_ = (const char *)config["smtp"]["mailPassword"];
  SmtpReceiver_ = (const char *)config["smtp"]["mailReceiver"];
  SmtpServer_ = (const char *)config["smtp"]["smtpServer"];
  SmtpPort_ = (const char *)config["smtp"]["smtpPort"];

  Serial.println("#### CONFIG LOADED ####");

  file_.close();
}

void changeCredentials(fs::FS &fs, const char *path, const char *device0, const char *device1, const char *device2)
{
  File file_ = fs.open(path, "w");
  String content;
  if (!file_.available())
  {
    Serial.println("Couldn't open the file");
  }
  StaticJsonDocument<1024> config;

  config["device"]["UID"] = uid_;
  config["device"]["name"] = deviceName_;
  config["network"]["SSID"] = ssid_;
  config["network"]["wifiPassword"] = passw_;
  config["network"]["ip"] = staticIpAP_;
  config["network"]["subnetMask"] = subnetMaskAP_;
  config["network"]["gateway"] = gatewayAP_;
  config["mqtt"]["host"] = host_;
  config["mqtt"]["root_topic_subscribe"] = root_topic_subscribe_;
  config["mqtt"]["root_topic_publish"] = root_topic_publish_;
  config["mqtt"]["port"] = port_;
  config["smtp"]["mailSender"] = smtpSender_;
  config["smtp"]["mailPassword"] = smtpPass_;
  config["smtp"]["mailReceiver"] = SmtpReceiver_;
  config["smtp"]["smtpServer"] = SmtpServer_;
  config["smtp"]["smtpPort"] = SmtpPort_;
  config["devices"]["sensor0"] = device0;
  config["devices"]["sensor1"] = device1;
  config["devices"]["sensor2"] = device2;

  Serial.println("#### CONFIG WRITTEN ####");

  auto error = serializeJsonPretty(config, file_);

  if (error)
  {
    Serial.println("Failed to Serialize");
  }
};

void loadRequiredDataforHttpServer(fs::FS &fs, const char *path)
{
  File file_ = fs.open(path, "r");
  String content;
  if (!file_.available())
  {
    Serial.println("(DNS instance) Couldn't open the file");
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
    Serial.println("(DNS instance) Failed to deserialize");
    Serial.println(error.f_str());
  }

  httpLocalIp = (const char *)config["network"]["ip"];
  localDeviceName = (const char *)config["device"]["name"];

  file_.close();
}

void handleRoot()
{
  Serial.println("You called root page");
  server.send(200, "text/html", String(MAIN_page)); // Send web page
  if (server.args() > 0)
  {
    for (uint8_t n = 0; n < server.args(); n++)
    {
      Serial.print(server.argName(n) + "-");
      String argument = server.argName(n);
      if (argument == "Sensor_1")
      {
        sensor0_ = server.arg(n).c_str();
      }
      else if (argument == "Sensor_2")
      {
        sensor1_ = server.arg(n).c_str();
      }
      else if (argument == "Sensor_3")
      {
        sensor2_ = server.arg(n).c_str();
      }
    }
    
    loadDevices(LittleFS, "config.json");
    changeCredentials(LittleFS, "config.json", sensor0_, sensor1_, sensor2_);
    ESP.restart();
  }
}

void handleReset(){
  server.send(200, "text/html", "<h1>RESETING DEVICE</h1>");
}
