#ifndef MYMQTT_H
#define MYMQTT_H
#include <ArduinoMqttClient.h>
#include <WiFiClient.h>
#include <string.h>

WiFiClient client_;
MqttClient mqttClient(client_);

/**
 * @brief This function handle the incomming messages from a particular topic
 *
 * @param messageSize
 */
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

  updateConfig(LittleFS, newContent);
}

void mqttSetup(const char *MQTT_SERVER, uint16_t MQTT_PORT, const char *PATH, WiFiClient client, const char *PATH_ALT = "")
{
  int count = 0;
  while (!mqttClient.connect(MQTT_SERVER, MQTT_PORT))
  {
    if (count != 500)
    { // 500 (15 min aprox)
      Serial.print(String(count) + String(") "));
      Serial.print("(MQTT instance) MQTT connection failed! Error code: ");
      Serial.println(std::to_string(mqttClient.connectError()).c_str());
      count++;
      delay(1500);
    }
    else
    {
      restoreConfig(LittleFS);
      ESP.restart();
    }
  }

  mqttClient.setId(chipId);

  mqttClient.setUsernamePassword("", "");

  mqttClient.setCleanSession(true);

  mqttClient.onMessage(onMqttMessage);

  mqttClient.subscribe(configTopic.c_str(), 2);

  Serial.println("(MQTT instance) You're connected to the MQTT broker!");
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
void mqttOnLoop(const char *MQTT_SERVER, uint16_t MQTT_PORT, const char *PATH, const char *MESSAGE = "")
{
  if (!mqttClient.connected())
  {
    mqttClient.connect(MQTT_SERVER, MQTT_PORT);
  }

  if (!mqttClient.beginMessage(PATH, false, 2, false))
  {
    Serial.println("(MQTT instance) could not publish into the topic");
  }

  mqttClient.print(String(MESSAGE));
  mqttClient.endMessage();
}

#endif
