int port = 1883;
int eventInterval = 1500;
int previousTimeScreen = 0;
int previousTimeTemporalData = 0;
int previousTimeMQTT = 0;
int previousKeepAliveTime = 0;
int temporalDataRefreshTime = 10000;
int MQTTmsgTime;
int keepAliveTime;

String deviceName;
String staticIpAP;
String gatewayAP;
String subnetMaskAP;
String host;
String smtpSender;
String smtpPass;
String SmtpReceiver;
String SmtpServer;
String tempString0, tempString1, tempString2;
String configTopic = "DeviceConfig/" + String(ESP.getChipId());
String root_topic_subscribe = "DeviceConfig/" + String(ESP.getChipId());
String root_topic_publish = "DeviceData/" + String(ESP.getChipId());
String keep_alive_topic_publish = "DeviceStatus/" + String(ESP.getChipId());

