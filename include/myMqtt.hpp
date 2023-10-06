#ifndef MYMQTT_H
#define MYMQTT_H

//#define SSL

#ifndef SSL
#include <WiFiClient.h>
#else
#include <WiFiClientSecure.h>
#include <loadCerts.hpp>
#endif

#include <ArduinoMqttClient.h>
#include <string.h>
#include <functions.hpp>

#ifndef SSL
WiFiClient client_;
#else
WiFiClientSecure client_;
#endif

MqttClient mqttClient(client_);

/**
 * @brief This function handle the incomming messages from a particular topic
 *
 * Topics -> resetDevice
 * @param messageSize
 */
void onMqttMessage(int messageSize)
{
	// we received a message, print out the topic and contents
	String incomeTopic = mqttClient.messageTopic();
	Serial.println("Received a message with topic '" + incomeTopic);

	String newContent;

	if (incomeTopic == configTopic)
	{
		// use the Stream interface to print the contents
		while (mqttClient.available())
		{
			newContent += (char)mqttClient.read();
		}

		updateConfig(LittleFS, newContent);
	}
	else if (incomeTopic == reset_topic)
	{
		while (mqttClient.available())
		{
			newContent += (char)mqttClient.read();
		}

		StaticJsonDocument<1024> reset_;
		deserializeJson(reset_, newContent);
		int temp_password = (int)reset_["password"];

		if (temp_password == password)
		{
#ifdef DEBUG
			Serial.println("Password:" + String(temp_password));
			Serial.println("Correct Password\nResetting device");
#endif
			ESP.eraseConfig();
			ESP.reset();
		}
		else
		{
#ifdef DEBUG
			Serial.println("Password:" + String(temp_password));
			Serial.println("Incorrect password...");
#endif
		}
	}
	else if (incomeTopic == relay_topic)
	{
		while (mqttClient.available())
		{
			newContent += (char)mqttClient.read();
		}

		StaticJsonDocument<1024> content;
		deserializeJson(content, newContent);
		int state = (int)content["state"];
		if (state == 1)
		{
			changeStatus(true);
		}
		else if (state == 0)
		{
			changeStatus(false);
		}
	}
}

#ifndef SSL
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
		}
		else
		{
			myPref.putString("mqtt_host", "test.mosquitto.org");
			myPref.putString("mqtt_port", "1883");
			myPref.putString("mqtt_username", "");
			myPref.putString("mqtt_password", "");
		}
	}

	mqttClient.setId(chipId);

	mqttClient.setUsernamePassword(mqtt_username, mqtt_password);

	mqttClient.setCleanSession(true);

	mqttClient.onMessage(onMqttMessage);

	mqttClient.subscribe(configTopic.c_str(), 2);
	mqttClient.subscribe(reset_topic.c_str(), 2);
	mqttClient.subscribe(relay_topic.c_str(), 2);

	Serial.println("(MQTT instance) You're connected to the MQTT broker!");
}

#else

void mqttSetup(const char *MQTT_SERVER, uint16_t MQTT_PORT, const char *PATH, WiFiServerSecure client, const char *PATH_ALT = "")
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
		}
		else
		{
			myPref.putString("mqtt_host", "test.mosquitto.org");
			myPref.putString("mqtt_port", "1883");
			myPref.putString("mqtt_username", "");
			myPref.putString("mqtt_password", "");
		}
	}

	mqttClient.setId(chipId);

	mqttClient.setUsernamePassword(mqtt_username, mqtt_password);

	mqttClient.setCleanSession(true);

	mqttClient.onMessage(onMqttMessage);

	mqttClient.subscribe(configTopic.c_str(), 2);
	mqttClient.subscribe(reset_topic.c_str(), 2);
	mqttClient.subscribe(relay_topic.c_str(), 2);

	Serial.println("(MQTT instance) You're connected to the MQTT broker!");
}

#endif

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
void mqttOnLoop(const char *MQTT_SERVER, uint16_t MQTT_PORT, const char *PATH, String MESSAGE = "")
{
	if (!mqttClient.connected())
	{
		mqttClient.connect(MQTT_SERVER, MQTT_PORT);
	}
	mqttClient.beginMessage(PATH);
	/*
	if (!mqttClient.beginMessage(PATH))
	{
	  Serial.println("(MQTT instance) could not publish into the topic");
	}
	*/
	mqttClient.print(MESSAGE);
	mqttClient.endMessage();
}

#endif
