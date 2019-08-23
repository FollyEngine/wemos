/*
  mqtt.h - Library for simplified use of PubSubClient.
  Created by Sven Dowideit, 2018.
  Released into the public domain.
*/

#ifndef FollyEngine_MQTT_h
#define FollyEngine_MQTT_h

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <PubSubClient.h>
#include <ArduinoJson.h>

// Send a Status update every 60 seconds
#define STATUSRATE (60)

class Mqtt
{
  public:
    // NOTE: this won't work with SSL MQTT - that requires certs and stuff to work.
    Mqtt(const char *SSID, const char *PASS, const char *Mqttserver, int Mqttport, const char *MqttUser, const char *MqttPass, const char *object);
    Mqtt(const char *SSID, const char *PASS, const char *Mqttserver, int Mqttport, const char *object);

    const char *getHostname();
    const char *getObject();

    void setCallback(MQTT_CALLBACK_SIGNATURE);

    void setup();
    bool loop();  // returns false if we had to re-init

    void publish(const char *object, const char *verb, JsonObject& root);
    void status(JsonObject& root);

    void publishString(const char *object, const char *verb, const char *message);
    void statusString(const char *message);
    
    boolean subscribe(const char *host, const char *object, const char *verb);
  private:
    char m_Hostname[255];
    char *m_Object;
    char *m_SSID;
    char *m_PASS;
    char *m_MQTTServer;
    char *m_MqttUser;
    char *m_MqttPass;
    int m_MQTTPort;
    MQTT_CALLBACK_SIGNATURE;

    // I wonder if the WifiClient should be static
    WiFiClient espWifi;
    PubSubClient client;

    time_t m_lastStatus;

};

#endif
