#ifndef MYMQTT_H
#define MYMQTT_H
#include <ArduinoMqttClient.h>

WiFiClient client_;
MqttClient mqttClient(client_);

void onMqttMessage(int messageSize)
{
  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  String newContent;
  
  // use the Stream interface to print the contents
  while (mqttClient.available())
  {
    newContent += (char)mqttClient.read();
  }

  Serial.println(newContent);

  updateConfig(LittleFS, newContent.c_str());
}

void mqttSetup(const char *MQTT_SERVER, uint16_t MQTT_PORT, const char *PATH, WiFiClient client, const char *PATH_ALT = "")
{
  while (!mqttClient.connect(MQTT_SERVER, MQTT_PORT))
  {
    Serial.print("MQTT connection failed! Error code: ");
    Serial.println(std::to_string(mqttClient.connectError()).c_str());
    delay(1500);
  }

  mqttClient.setId(String(ESP.getChipId()));

  mqttClient.setUsernamePassword("", "");

  mqttClient.setCleanSession(true);

  mqttClient.onMessage(onMqttMessage);

  mqttClient.subscribe(configTopic.c_str(), 2);

  Serial.println("You're connected to the MQTT broker!");
}

/**
 * @brief mqttOnLoop receive all the neccesary data to send a MQTT message
 *
 * @param MQTT_SERVER MQTT host
 * @param MQTT_PORT MQTT port (1883 no SSL, 8883 SSL)
 * @param PATH Topic where the message will be sent
 * @param client WiFi Instance
 * @param PATH_ALT Alternative topic, Empty String by default
 * @param TOPIC System topic
 * @param MESSAGE Message to be sent
 */
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

#endif