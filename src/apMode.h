#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#endif
#include <string>
#include <Arduino.h>
#include <LittleFS.h>
#include "screenController.h"
//#include <ESPAsyncWebServer.h>
//#include <AsyncTCP.h>

//AsyncWebServer server(80);
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void changeCredentials(fs::FS &fs, const char * path, String mailReceiver);
void loadCredentials(fs::FS &fs, const char * path);
//void notFound(AsyncWebServerRequest *request);
bool setup_wifi();
void saveConfigCallback();
String dataAsString(const char * path);

WiFiManager myManager;
Screen myScreenAp;
//String ssid;
//String wifiPassword;
//String HTML = dataAsString("/index.html");

//const char* ssidParam = "ssidParam";
//const char* passwParam = "passwParam";
//const char* mailReceiverParam = "mailReceiverParam";

//const String ssidInput= "SSID";  
//const String passwInput= "WIFI_PASSWORD";
//const String mailReceiverInput= "MAIL_RECEIVER";

bool shouldSaveConfig = false;

class apMode{
    private:
    const char* ssid     = "Darkflow-Device";
    const char* password = "123456789";
    char smtpMail[40];
    public:
        /*The function setuServer() has to be executed in the setup
        line from the main file*/
        void setupServer(){
          //WiFiManagerParameter customInput();          
          myManager.setAPStaticIPConfig(local_ip, gateway, subnet);

          myManager.setSaveConfigCallback(saveConfigCallback);

          myManager.setTimeout(120);
          
          myManager.setCustomHeadElement("<style>html{filter: invert(100%); -webkit-filter: invert(100%);}</style>");

          String msg0 = "SSID:" + String(ssid);
          myScreenAp.printScreen("CONFIGURE WIFI HDP..", 0, 0, true);
          myScreenAp.printScreen(msg0, 0, 1, false);
          myScreenAp.printScreen("IP: 192.168.1.1", 0, 2, false);
          myScreenAp.printScreen("2 Minutes timeout", 0, 3, false);

          WiFiManagerParameter smtpUser("mail", "SMTP mail", smtpMail, 40);
          myManager.addParameter(&smtpUser);
          
          bool isConnected = myManager.autoConnect(ssid);

          strcpy(smtpMail, smtpUser.getValue());
          changeCredentials(LittleFS, "/config.json", String(smtpMail));

          if(!isConnected){
            myScreenAp.printScreen("Hit timeout...", 0, 1, true);
            myScreenAp.printScreen("Restarting Device", 0, 2, false);
            Serial.println("Time Out...");
            delay(2000);
            ESP.restart();
          }
          
          Serial.println(WiFi.localIP());
          
        }

        void reset(){
          Serial.println("*** Resetting WiFi credentials ***");
          myScreenAp.printScreen("  Resetting Device ", 0, 1, true);
          delay(5000);
          myManager.resetSettings();
          ESP.eraseConfig();
          ESP.reset();
          ESP.restart();
        }
};

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void changeCredentials(fs::FS &fs, const char * path, String mailReceiver){
  File file_ = fs.open(path, "w");
    String content;
    if(!file_.available()){
      Serial.println("Couldn't open the file");  
    }
    StaticJsonDocument<1024> config;

    config["host"] = "mqtt.darkflow.com.ar";
    config["root_topic_subscribe"] = "giuli/testing";
    config["root_topic_publish"] = "giuli/data";
    config["mailSender"] = "giulicrenna@outlook.com";
    config["mailPassword"] = "kirchhoff2002";
    config["mailReceiver"] = mailReceiver.c_str();;
    config["smtpServer"] = "smtp.office365.com";
    config["smtpPort"] = "587";

    Serial.println("#### CONFIG WRITTEN ####");
    Serial.println((const char*)config["mailReceiver"]);

    auto error = serializeJson(config, file_);

    if(error){
      Serial.println("Failed to Serialize");
    }
}

/*
String dataAsString(const char * path){
    File file_ = LittleFS.open(path, "w");
    String data;
    if(!file_.available()){
      Serial.println("Couldn't open the file");  
    }
    while (file_.available()) {
      data += file_.readString();
      break;
    }

    file_.close();
    return data;
}


//function> setup_wifi: None -> int
bool setup_wifi(){
  //WiFi connection
  Serial.print("Conectando a ");
  Serial.print(ssid);
  Serial.println(" ");

  int counter = 0;

  WiFi.begin(ssid.c_str(), wifiPassword.c_str());

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      counter += 1;
      if(counter == 20){
        break;
        return false;
      }
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("IP local: ");
  Serial.println(WiFi.localIP());

  return true;
}

    file_.close();
}

void loadCredentials(fs::FS &fs, const char * path){
    File file_ = fs.open(path, "r");
    String content;
    if(!file_.available()){
      Serial.println("Couldn't open the file");  
    }
    while (file_.available()) {
      content += file_.readString();
      break;
    }
    StaticJsonDocument<1024> config;
    auto error = deserializeJson(config, content);

    if(error){
      Serial.println("Failed to deserialize");
      Serial.println(error.f_str());
    }
    
    Serial.println(config.size());

    ssid = (const char*)config["ssid"];
    wifiPassword = (const char*)config["wifiPassword"];

    Serial.println("#### CONFIG LOADED ####");
    Serial.println(ssid);
    Serial.println(wifiPassword);

    file_.close();
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

*/