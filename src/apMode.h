#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <string>
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
//#include <AsyncTCP.h>

AsyncWebServer server(80);
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void notFound(AsyncWebServerRequest *request);
String dataAsString(const char * path);

String HTML = dataAsString("/index.html");

const char* ssidParam = "SSID";
const char* passwParam = "WIFI_PASSWORD";
const char* mailReceiverParam = "MAIL_RECEIVER";

const String ssidInput= "SSID";  
const String passwInput= "WIFI_PASSWORD";
const String mailReceiverInput= "MAIL_RECEIVER";


class apMode{
    private:
    const char* ssid     = "Darkflow-device";
    const char* password = "123456789";
    public:
        /*The function setuServer() has to be executed in the setup
        line from the main file*/
        void setupServer(){
            Serial.println("Starting HTTP server");
            WiFi.softAP(ssid, password);
            WiFi.softAPConfig(local_ip, gateway, subnet);
            IPAddress apIp = WiFi.softAPIP();
            Serial.print("AP IP address: ");
            Serial.println(apIp);
            server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
              request->send(SPIFFS, "/index.html", String(), false);
            });
            /*
            server.on("/main.css", HTTP_GET, [](AsyncWebServerRequest *request){
              request->send(SPIFFS, "/main.css", "text/css");
            });
            */
           //get?ssidParam=awdaw&passwParam=dawdaw&mailReceiverParam=awd
            server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
                String inputMessage1,inputMessage2,inputMessage3;
                // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
                if (request->hasParam(ssidParam)) {
                  inputMessage1 = request->getParam(ssidParam)->value();
                } else {
                  inputMessage1 = "none";
                }
                // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
                if (request->hasParam(passwParam)) {
                  inputMessage2 = request->getParam(passwParam)->value();
                } else {
                  inputMessage2 = "none";
                }
                // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
                if (request->hasParam(mailReceiverParam)) {
                  inputMessage3 = request->getParam(mailReceiverParam)->value();
                } else {
                  inputMessage3 = "none";
                }
                Serial.println(ssidInput + ": " + inputMessage1);
                Serial.println(passwInput + ": " + inputMessage2);
                Serial.println(mailReceiverInput + ": " + inputMessage3);
                //request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" + ssidInput + ") with value: " + inputMessage1 + ", (" + passwParam + ") with value: " + inputMessage2 + ", (" + mailReceiverInput + ") with value: " + inputMessage3 + "<br><a href=\"/\">Return to Home Page</a>"); 
            });
            //server.onNotFound(notFound);
            server.begin();
        }
};

String dataAsString(const char * path){
    File file_ = SPIFFS.open(path);
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


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}