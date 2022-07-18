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