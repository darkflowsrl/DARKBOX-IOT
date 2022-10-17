#define AP_OUTLINE
#define AP_CUSTOM_PORTAL
//#define DASHBOARD_OUTLINE
#define LOCAL_DASHBOARD
//#define SMTP_CLIENT
//#define I2C 

unsigned int port = 1883;
unsigned int eventInterval = 1500;
unsigned int debouncerTime = 20;
unsigned int previousTimeScreen = 0;
unsigned int previousTimeTemporalData = 0;
unsigned int previousTimeMQTTtemp = 0;
unsigned int previousTimeMQTThum = 0;
unsigned int previousKeepAliveTime = 0;
unsigned int temporalDataRefreshTime = 10;
unsigned int MQTTtemp;
unsigned int MQTThum;
unsigned int keepAliveTime;
unsigned int SmtpPort;

String chipId = String(ESP.getChipId());
String IO_0;
String IO_1;
String IO_2;
String IO_3;
String deviceName;
String staticIpAP;
String subnetMaskAP;
String gatewayAP;
String SmtpSender;
String SmtpPass;
String SmtpReceiver;
String SmtpServer;
String heap;
String bootVersion = String(ESP.getBootVersion());
String chipId_;
String CPUfreq = String(ESP.getCpuFreqMHz());
String coreVersion = String(ESP.getCoreVersion());
String flashChipId = String(ESP.getFlashChipId());
String flashRealSize = String(ESP.getFlashChipRealSize());
String flashChipSpeed = String(ESP.getFlashChipSpeed());
String freeSketchSize = String(ESP.getFreeSketchSpace());
String fullVersion = String(ESP.getFullVersion());
String vcc;
String releStatus = "LOW";
String localDeviceName = String("darkflow-") + chipId;
String tempString0, tempString1, tempString2;
String host = "mqtt.darkflow.com.ar";
String configTopic = "DeviceConfig/" + chipId;
String root_topic_subscribe = "DeviceConfig/" + chipId;
String root_topic_publish = "DeviceData/" + chipId;
String keep_alive_topic_publish = "DeviceStatus/" + chipId;

struct {
  int t0;
  int t1;
  int h0;
  String d0;
  String d1;
  String d2;
  String d3;
}TemporalAccess;
