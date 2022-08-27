#include <ArduinoMqttClient.h>
#include <ESP8266WiFi.h>

WiFiClient client_;
MqttClient mqttClient(client_);

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    Serial.print((char)mqttClient.read());
  }
  
  Serial.println();
}

void mqttSetup(const char *MQTT_SERVER, uint16_t MQTT_PORT, const char *PATH, WiFiClient client, const char *PATH_ALT = "")
{
    if (!mqttClient.connect(MQTT_SERVER, MQTT_PORT))
    {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());

        while (1)
            ;
    }
    mqttClient.setId(String(ESP.getChipId()));

    mqttClient.onMessage(onMqttMessage);

    mqttClient.subscribe(MQTT_SERVER, 1);

    Serial.println("You're connected to the MQTT broker!");
}

void mqttOnLoop(const char *MQTT_SERVER, uint16_t MQTT_PORT, const char *PATH, WiFiClient client, const char *PATH_ALT = "",
                const char *TOPIC = "$SYS", const char *MESSAGE = "")
{
    mqttClient.poll();

    if (!mqttClient.beginMessage(PATH))
    {
        Serial.println("(MQTT instance) could not publish into the topic");
    }
    mqttClient.print(MESSAGE);
    mqttClient.endMessage();
}