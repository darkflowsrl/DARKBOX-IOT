#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#endif
#include <functions.h>
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
IPAddress dns(8, 8, 8, 8);

void changeCredentials(fs::FS &fs, const char * path, String mailReceiver,
  String StaticIP, String gateway, String subnet, String ssid, String password, String deviceName);
bool isConfigured(const char * path);
bool setup_wifi();
void saveConfigCallback();
String dataAsString(const char * path);

WiFiManager myManager;
Screen myScreenAp;

bool shouldSaveConfig = false;

class apMode{
    private:
    const char* ssid     = String("Darkflow-" + ESP.getChipId()).c_str();
    const char* password = "123456789";
    const char* _customHtml_checkbox = "type=\"checkbox\""; 
    /*
      <!-- INPUT CHOICE -->
      <br/>
      <p>Enable Sensors</p>
      <input style='display: inline-block;' type='radio' id='choice1' name='program_selection' value='1'>
      <label for='choice1'>Sensor 0</label><br/>
      <input style='display: inline-block;' type='radio' id='choice2' name='program_selection' value='2'>
      <label for='choice2'>Sensor 1</label><br/>
      <input style='display: inline-block;' type='radio' id='choice2' name='program_selection' value='2'>
      <label for='choice2'>Sensor 2</label><br/>
      <!-- INPUT SELECT -->
      <br/>
    

      <!-- INPUT CHOICE -->
      <br/>
      <p>Select Choice</p>
      <input style='display: inline-block;' type='radio' id='choice1' name='program_selection' value='1'>
      <label for='choice1'>Choice1</label><br/>
      <input style='display: inline-block;' type='radio' id='choice2' name='program_selection' value='2'>
      <label for='choice2'>Choice2</label><br/>
      <!-- INPUT SELECT -->
      <br/>
      <label for='input_select'>Label for Input Select</label>
      <select name="input_select" id="input_select" class="button">
      <option value="0">Option 1</option>
      <option value="1" selected>Option 2</option>
      <option value="2">Option 3</option>
      <option value="3">Option 4</option>
      </select>
      */
    /*
    const char *bufferStr = R"(
      <br/>
      <label for='sensor_select0'>Sensor 0</label>
      <select name="input_select" id="sensor_select0" class="button">
      <option value="0">OneWire</option>
      <option value="1">DHC</option>
      <option value="2">Tacometro</option>
      <option value="3">Luxometro</option>
      <option value="4"selected>Nothing</option>
      </select>

      <label for='sensor_select1'>Sensor 1</label>
      <select name="input_select1" id="sensor_select1" class="button">
      <option value="0">OneWire</option>
      <option value="1">DHC</option>
      <option value="2">Tacometro</option>
      <option value="3">Luxometro</option>
      <option value="4"selected>Nothing</option>
      </select>

      <label for='sensor_select2'>Sensor 2</label>
      <select name="input_select2" id="sensor_select2" class="button">
      <option value="0">OneWire</option>
      <option value="1">DHC</option>
      <option value="2">Tacometro</option>
      <option value="3">Luxometro</option>
      <option value="4"selected>Nothing</option>
      </select>
    )";
    */
    const char* icon = "<link rel='icon' type='image/png' sizes='16x16' href='data:image/x-icon;base64,AAABAAEAEBAAAAEAIABoBAAAFgAAACgAAAAQAAAAIAAAAAEAIAAAAAAAAAQAAAAAAAAAAAAAAAAAAAAAAADv7+//7+/v/+/v7//v7+//7+/v//Hw7//y8e//8fHv//Lx7//y8u//8vHv/+/v7//v7+//7+/v/+/v7//v7+//FxcX/xcXF/8XFxf/FxcX/xcXF/8QERf/DA4X/wwOF/8MDhf/CQsW/w0OF/8YGBf/GBgX/xcXF/8XFxf/FxcX/wAAAP8AAAD/AAAA/wIBAP8AAAD/XE0A/5R7AP+JcgD/iHEA/21bAP8uJQD/AAAA/wAAAP8AAAD/AAAA/wAAAP8BAQH/AQEB/wAAAf8EAwL/AAAB/5V+CP/sxwv/478L/+zGC//wygv/478L/5eAB/8YFAL/AAAB/wMDAv8BAQH/AAAA/wwKAf9iUwT/NSwC/wAAAP+KdAb/478K/1JFBP8kHgL/XE0E/66SB//sxwr/spYI/xANAf8AAAD/AQEA/wAAAP9wXgX/+dEK/0k9A/8AAAD/i3UG/+G9Cv9qWQX/NCwD/wMCAP8FBAD/kHkG/+3ICv95ZQX/AAAA/wMCAP8AAAD/c2EF/+rFCv9DOQP/AAAA/5mBBv/uyAr/6cQK//LLCv80LAL/AAAA/xkVAf/JqQn/w6QI/wsJAf8AAAD/AAAA/3JgBf/txwr/QzkD/wAAAP9SRAT/fGgF/29dBf96ZwX/LygC/wAAAP8AAAD/lHwG/967Cv8iHQL/AAAA/wAAAP9yYAX/7ccK/0M5A/8AAAD/YVIE/5R9Bv+Icwb/knoG/3FfBf8AAAD/AAAA/4dyBv/jvwn/KCEC/wAAAP8AAAD/cmAF/+3HCv9EOgP/AAAA/52EB//0zgr/478K/+rFCv/Ztwn/HBcC/wAAAP+skQf/1rQJ/xoWAf8AAAD/AAAA/3JgBf/txwr/RDkD/wAAAP8oIQL/OC8C/zUtAv81LQL/OzED/wQDAP86MQP/4L0J/62SB/8AAAD/AAAA/wAAAP9yYAX/7ccK/0A2A/8AAAD/AwIA/wAAAP8AAAD/AQEA/wAAAP8qIwL/vZ8I/+fDCv9URwT/AAAA/wMCAP8AAAD/cV8F/+XACv9wXgX/PzUD/0s/A/9KPgP/T0IE/2NTBP+QeQb/1LMJ/+rFCv+FcAb/AAAA/wEBAP8AAAD/AAAA/35qBf/yywr/58EK/+/ICv/uyAr/7sgK/+/ICv/wygr/68YK/8ioCP9lVQX/AwMA/wEAAP8AAAD/AAAA/wAAAP9BNwP/fGgF/3BeBf9xXwX/cmAF/3JfBf9yYAX/alkF/0c8A/8SDwH/AAAA/wICAP8BAAD/AAAA/wAAAP8BAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wMCAP8BAAD/AAAA/wAAAP8AAAD/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA==' />";
    char smtpMail[40];
    char staticIPAP[40];
    char gatewayAP[40];
    char subnetAP[40];
    char deviceName_[40];
    public:
        /*The function setuServer() has to be executed in the setup
        line from the main file*/
        void setupServer(String staticIpAP, String gatewayAP_, String subnetMaskAP){
          // Library Configuration
          WiFi.mode(WIFI_STA);

          myManager.setShowStaticFields(false);

          IPAddress miIp = strToIp(staticIpAP.c_str()); 
          IPAddress miGateway = strToIp(gatewayAP_.c_str());
          IPAddress miSubnet = strToIp(subnetMaskAP.c_str());

          if(staticIpAP != "" && subnetMaskAP != "" && gatewayAP_ != ""){
            myManager.setSTAStaticIPConfig(miIp, miGateway, miSubnet, IPAddress(8, 8, 8, 8)); //Repair this, static ip detected but no configured
          }
          
          myManager.setCustomHeadElement(icon);     
          
          myManager.setAPStaticIPConfig(local_ip, gateway, subnet);
          
          myManager.setSaveConfigCallback(saveConfigCallback);

          myManager.setDarkMode(true);

          myManager.setCountry("AR");

          myManager.setTitle("Darkflow Device");

          myManager.setTimeout(520);
          
          myManager.setScanDispPerc(true);

          // Shows information trough the I2C screen
          String msg0 = "SSID:" + String(ssid);
          myScreenAp.printScreen("CONFIGURE WIFI HDP..", 0, 0, true);
          myScreenAp.printScreen(msg0, 0, 1, false);
          myScreenAp.printScreen("IP: 192.168.1.1", 0, 2, false);
          myScreenAp.printScreen("2 Minutes timeout", 0, 3, false);

          // Custom parameters
          WiFiManagerParameter deviceName("deviceName", "Nombre del dispositivo (Campo obligatorio)", deviceName_, 40, "required");
          myManager.addParameter(&deviceName);
          WiFiManagerParameter smtpUser("mail", "SMTP mail", smtpMail, 40);
          myManager.addParameter(&smtpUser);
          WiFiManagerParameter customWarning("<p>Keep all network parameters in blank for DHCP</p>"); //Sensor 0 Parameters
          myManager.addParameter(&customWarning);
          WiFiManagerParameter StaticIpParam("StaticIP", "IP estatica", staticIPAP, 40, "pattern='\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}'");
          myManager.addParameter(&StaticIpParam);
          WiFiManagerParameter gatewayParam("GateWay", "Puerta de enlace predeterminada", gatewayAP, 40, "pattern='\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}'");
          myManager.addParameter(&gatewayParam);
          WiFiManagerParameter subnetParam("Subnet", "Mascara de subred", subnetAP, 40, "pattern='\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}'");
          myManager.addParameter(&subnetParam);
          /*WiFiManagerParameter customTextSensor0("<p>Sensor 0 Configuration</p>"); //Sensor 0 Parameters
          myManager.addParameter(&customTextSensor0);
          WiFiManagerParameter Sensor0_0("Sensor_0_OneWire", "OneWire", "T", 2, _customHtml_checkbox, WFM_LABEL_AFTER);
          myManager.addParameter(&Sensor0_0);
          WiFiManagerParameter Sensor0_1("Sensor_0_DHC", "DHC Temp and Humidity", "F", 2, _customHtml_checkbox, WFM_LABEL_AFTER);
          myManager.addParameter(&Sensor0_1);
          WiFiManagerParameter Sensor0_2("Sensor_0_Tac", "Tacometro", "F", 2, _customHtml_checkbox, WFM_LABEL_AFTER);
          myManager.addParameter(&Sensor0_2);
          WiFiManagerParameter Sensor0_3("Sensor_0_Lux", "Luxómetro", "F", 2, _customHtml_checkbox, WFM_LABEL_AFTER);
          myManager.addParameter(&Sensor0_3);
          WiFiManagerParameter customTextSensor1("<p>Sensor 1 Configuration</p>"); //Sensor 1 parameters
          myManager.addParameter(&customTextSensor1);
          WiFiManagerParameter Sensor1_0("Sensor_1_OneWire", "OneWire", "T", 2, _customHtml_checkbox, WFM_LABEL_AFTER);
          myManager.addParameter(&Sensor1_0);
          WiFiManagerParameter Sensor1_1("Sensor_1_DHC", "DHC Temp and Humidity", "F", 2, _customHtml_checkbox, WFM_LABEL_AFTER);
          myManager.addParameter(&Sensor1_1);
          WiFiManagerParameter Sensor1_2("Sensor_1_Tac", "Tacometro", "F", 2, _customHtml_checkbox, WFM_LABEL_AFTER);
          myManager.addParameter(&Sensor1_2);
          WiFiManagerParameter Sensor1_3("Sensor_1_Lux", "Luxómetro", "F", 2, _customHtml_checkbox, WFM_LABEL_AFTER);
          myManager.addParameter(&Sensor1_3);
          WiFiManagerParameter customTextSensor2("<p>Sensor 2 Configuration</p>"); //Sensor 2 parameters
          myManager.addParameter(&customTextSensor2);
          WiFiManagerParameter Sensor2_0("Sensor_2_OneWire", "OneWire", "T", 2, _customHtml_checkbox, WFM_LABEL_AFTER);
          myManager.addParameter(&Sensor2_0);
          WiFiManagerParameter Sensor2_1("Sensor_2_DHC", "DHC Temp and Humidity", "F", 2, _customHtml_checkbox, WFM_LABEL_AFTER);
          myManager.addParameter(&Sensor2_1);
          WiFiManagerParameter Sensor2_2("Sensor_2_Tac", "Tacometro", "F", 2, _customHtml_checkbox, WFM_LABEL_AFTER);
          myManager.addParameter(&Sensor2_2);
          WiFiManagerParameter Sensor2_3("Sensor_2_Lux", "Luxómetro", "F", 2, _customHtml_checkbox, WFM_LABEL_AFTER);
          
          WiFiManagerParameter custom_html_inputs(bufferStr);
          myManager.addParameter(&custom_html_inputs);
          myManager.addParameter(&Sensor2_3); */

          //<-

          //-> Start the client
          bool isConnected = myManager.autoConnect();
          //<-
          
          //->Save config into JSON
          strcpy(smtpMail, smtpUser.getValue());
          strcpy(staticIPAP, StaticIpParam.getValue());
          strcpy(gatewayAP, gatewayParam.getValue());
          strcpy(subnetAP, subnetParam.getValue());
          strcpy(deviceName_, deviceName.getValue());

          if(shouldSaveConfig){
            changeCredentials(LittleFS, "/config.json", String(smtpMail), String(staticIPAP), 
            String(gatewayAP), String(subnetAP), myManager.getWiFiSSID(), myManager.getWiFiPass(), String(deviceName_));
            ESP.restart();
          }
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
        //<-
};

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void changeCredentials(fs::FS &fs, const char * path, String mailReceiver,
  String StaticIP, String gateway, String subnet, String ssid, String password, String deviceName){
  
  File file_ = fs.open(path, "w");
    String content;
    if(!file_.available()){
      Serial.println("Couldn't open the file");  
    }
    StaticJsonDocument<1024> config;

    config["device"]["UID"] = std::to_string(ESP.getChipId()).c_str();
    config["device"]["name"] = deviceName.c_str();
    config["network"]["SSID"] = ssid.c_str();
    config["network"]["wifiPassword"] = password.c_str();
    config["network"]["ip"] = StaticIP.c_str();
    config["network"]["subnetMask"] = subnet.c_str();
    config["network"]["gateway"] = gateway.c_str();
    config["mqtt"]["host"] = "mqtt.darkflow.com.ar";
    config["mqtt"]["root_topic_subscribe"] = "giuli/testing";
    config["mqtt"]["root_topic_publish"] = "giuli/data";
    config["mqtt"]["port"] = "1883";
    config["smtp"]["mailSender"] = "giulicrenna@outlook.com";
    config["smtp"]["mailPassword"] = "kirchhoff2002";
    config["smtp"]["mailReceiver"] = mailReceiver.c_str();;
    config["smtp"]["smtpServer"] = "smtp.office365.com";
    config["smtp"]["smtpPort"] = "587";

    Serial.println("#### CONFIG WRITTEN ####");
    Serial.println((const char*)config["mailReceiver"]);

    auto error = serializeJsonPretty(config, file_);

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
*/
bool isConfigured(const char * path){
    File file_ = LittleFS.open(path, "r");
    String content;
    bool isConnected;
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

    if((const char*)config["ssid"] == "true"){
      isConnected = true;
    }else{
      isConnected = false;
    }

    file_.close();

    return isConnected;
}



