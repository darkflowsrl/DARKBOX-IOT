unsigned int port = 1883;
unsigned int eventInterval = 1500;
unsigned int previousTimeScreen = 0;
unsigned int previousTimeTemporalData = 0;
unsigned int previousTimeMQTT = 0;
unsigned int previousKeepAliveTime = 0;
unsigned int temporalDataRefreshTime = 10000;
unsigned int MQTTmsgTime;
unsigned int keepAliveTime;


String IO_0;
String IO_1;
String IO_2;
String IO_3;
String deviceName;
String staticIpAP;
String subnetMaskAP;
String gatewayAP;
String smtpSender;
String smtpPass;
String SmtpReceiver;
String SmtpServer;
String tempString0, tempString1, tempString2;
String host = "mqtt.darkflow.com.ar";
String configTopic = "DeviceConfig/" + String(ESP.getChipId());
String root_topic_subscribe = "DeviceConfig/" + String(ESP.getChipId());
String root_topic_publish = "DeviceData/" + String(ESP.getChipId());
String keep_alive_topic_publish = "DeviceStatus/" + String(ESP.getChipId());
