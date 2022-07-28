#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <sstream>
#include <NTPClient.h>
#include <EMailSender.h>
#include <WiFiUdp.h>
#include <string.h>

const int utcOffset = -10800;
unsigned long previousTime2 = 0;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffset);

void sendEmail(const char* sender, const char* password, const char* receiver, const char* host, int port){
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

// NTP FETCHER
String ntpRawDay(){
    timeClient.update();
    String data = String(daysOfTheWeek[timeClient.getDay()]) + ":" + String(timeClient.getHours()) + ":" + String(timeClient.getMinutes());
    return data;
}

String ntpRawNoDay(){
    timeClient.update();
    
    String minutes;
    String hour;

    if(String(timeClient.getMinutes()).length() == 1){
        minutes = "0"+String(timeClient.getMinutes());
    }else{
        minutes = String(timeClient.getMinutes());        
    }
    if(String(timeClient.getHours()).length() == 1){
        hour = "0"+String(timeClient.getHours());
    }else{
        hour = String(timeClient.getHours());        
    }

    String data = hour + ":" + minutes;
    
    return data;
}

IPAddress strToIp(String miIp){

    std::stringstream s(miIp.c_str());

    int oct0, oct1, oct2, oct3; //to store the 4 ints

    char ch; //to temporarily store the '.'

    s >> oct0 >> ch >> oct1 >> ch >> oct2 >> ch >> oct3;

    return IPAddress(oct0, oct1, oct2, oct3);
}

/*
IPAddress strToIp(String miIp){
    std::vector<int> ip;

    for(int i = 0; i < miIp.length(); i++){
        String empty = "";
        int num = 0;

        switch (num){
        case 0:
            if(miIp.charAt(i) == '.'){num++;miIp.trim(); ip.push_back(std::stoi(empty.c_str()));empty = "";} 
            empty += miIp.charAt(i);
        case 1:
            if(miIp.charAt(i) == '.'){num++; ip.push_back(std::stoi(empty.c_str()));empty = "";} 
            empty += miIp.charAt(i);
        case 2:
            if(miIp.charAt(i) == '.'){num++; ip.push_back(std::stoi(empty.c_str()));empty = "";} 
            empty += miIp.charAt(i);
        case 3:
            if(i == miIp.length()-1){empty += miIp.charAt(i); ip.push_back(std::stoi(empty.c_str()));empty = "";} 
            empty += miIp.charAt(i);

        }
    }
    int oct0 = ip.at(0); int oct1 = ip.at(1); int oct2 = ip.at(2); int oct3 = ip.at(3);
    
    Serial.println(String(oct0) + "." + String(oct1) + "." + String(oct2) + "." + String(oct3));

    return IPAddress(oct0, oct1, oct2, oct3);
}
*/

#endif