

#include "mqtt.h"

#include <Time.h>
#include <NtpClientLib.h>

#ifdef ESP8266
#include <ESP8266mDNS.h>
#else
// #include <ESPmDNS.h>
#endif


Mqtt::Mqtt(
		const char *SSID,
		const char *PASS,
		const char *Mqttserver,
		int Mqttport,
		const char *MqttUser,
		const char *MqttPass,
		const char *object
) {
  Mqtt(SSID, PASS, Mqttserver, Mqttport, object);

  m_MqttUser = (char*) malloc(strlen(MqttUser) + 1);
  strcpy(m_MqttUser, MqttUser);
  m_MqttPass = (char*) malloc(strlen(MqttPass) + 1);
  strcpy(m_MqttPass, MqttPass);
}

Mqtt::Mqtt(const char *SSID, const char *PASS, const char *Mqttserver, int Mqttport, const char *object) {
  m_SSID = (char*) malloc(strlen(SSID) + 1);
  strcpy(m_SSID, SSID);
  m_PASS = (char*) malloc(strlen(PASS) + 1);
  strcpy(m_PASS, PASS);
  m_MQTTServer = (char*) malloc(strlen(Mqttserver) + 1);
  strcpy(m_MQTTServer, Mqttserver);
  m_MQTTPort = Mqttport;
  m_MqttUser = NULL;
  m_MqttPass = NULL;
  m_Object = (char*) malloc(strlen(object) + 1);
  strcpy(m_Object, object);
}

void Mqtt::setCallback(MQTT_CALLBACK_SIGNATURE) {
  this->callback = callback;
  if (client.connected()) {
    client.setCallback(this->callback);
  }
}

const char *Mqtt::getHostname() {
  return m_Hostname;
}

const char *Mqtt::getObject() {
  return m_Object;
}


void Mqtt::setup() {
    client = PubSubClient(espWifi);
    delay(10);

    WiFi.begin(m_SSID, m_PASS);

    uint8_t mac[6];
    WiFi.macAddress(mac);
#ifdef ESP8266
    char *hostPrefix = "esp8266";
#else
    char *hostPrefix = "esp32";
#endif
    snprintf(m_Hostname, 255, "%s-%x%x%x%x%x%x", hostPrefix, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

#ifndef ESP8266
    // TODO: does this exist in some other way?
    WiFi.setHostname(m_Hostname);
#endif
    while (WiFi.status() != WL_CONNECTED) {
      Serial.println(".");
      delay(300);
    }

    Serial.println("Setup Wifi Connected:");
    Serial.println( WiFi.localIP());

    NTP.begin (); // Only statement needed to start NTP sync.
    //timeZone = 0;
    //minutesTimeZone = 0;
    //NTP.begin ("pool.ntp.org", timeZone, true, minutesTimeZone);
    //NTP.setInterval (63);

    Serial.printf("Host %s, Port %d", m_MQTTServer, m_MQTTPort);
    client.setServer(m_MQTTServer, m_MQTTPort);
    client.setCallback(callback);

    if (!MDNS.begin(m_Hostname)) {             // Start the mDNS responder for esp8266.local
      Serial.println("Error setting up MDNS responder!");
    } else {
      Serial.printf("mDNS responder started (%s)\r\n", m_Hostname);
    }

    StaticJsonDocument<300> root;
    status(root);
  };

  bool Mqtt::loop()
  {
    bool reInit = true;

    int resetCounter = 0;
    while (!client.connected())
    {
      resetCounter = resetCounter++;
      if (resetCounter > 20) {
        Serial.printf("Too many failures, restarting.....\r\n");
        ESP.restart();
      }
      reInit = false;
      while (!client.connected())
      {
        delay(300);
        Serial.printf("(reset:%d) Wifi Connected (%d) : %s, %s\r\n", resetCounter, WiFi.status(), m_Hostname, WiFi.localIP().toString().c_str());
        Serial.printf("MQTT Host %s, Port %d\r\n", m_MQTTServer, m_MQTTPort);
        boolean ok;
        if (m_MqttPass == NULL)
        {
          ok = client.connect(m_Hostname);
        }
        else
        {
          ok = client.connect(m_Hostname, m_MqttUser, m_MqttPass);
        }
        Serial.printf("MQTT: %s (state: %d)\r\n", (ok ? "true" : "false"), client.state());
      }
      client.setCallback(callback);
      // and here we need to re-init the subscriptions
    }
    client.loop();

    if (client.connected())
    {
      time_t time_now = now();
      if (time_now - m_lastStatus > (STATUSRATE))
      {
        m_lastStatus = time_now;

        StaticJsonDocument<300> root;
        status(root);
      }
    }

    MDNS.update();

    return reInit;
  };

  void Mqtt::publishString(const char *object, const char *verb, const char *message) {
    char topic[81];
    snprintf(topic, 80, "%s/%s/%s", m_Hostname, object, verb);
    client.publish(topic, message);
  };

  void Mqtt::publish(const char *object, const char *verb, JsonDocument& root) {
    char str[301];
    // TODO: only set it if its not already set
    root["device"] = String(object);
    // TODO: only set it if its not already set
    time_t t = now();
    //2018-11-17T06:52:44.747234
    snprintf(str, 100, "%d-%02d-%02dT%02d:%02d:%02d", year(t), month(t), day(t), hour(t), minute(t), second(t));
    root["time"] = String(str);

    serializeJson(root, str);
    publishString(object, verb, str);
  };

  //String hello = String("hello");
  char str[301];
  void Mqtt::status(JsonDocument& root) {
    // TODO: only set it if its not already set
    root["ip"] = WiFi.localIP().toString();
//    root["device"] = String(m_Object);
    root["from"] = String(m_Hostname);
    root["ping"] = String(m_Object);

    // TODO: only set it if its not already set
    time_t t = now();
    //2018-11-17T06:52:44.747234
    snprintf(str, 100, "%d-%02d-%02dT%02d:%02d:%02d", year(t), month(t), day(t), hour(t), minute(t), second(t));
    root["time"] = String(str);

    serializeJson(root, str);
    statusString(str);
  };


  const char *status_topic = "node-red/status/ping";
  void Mqtt::statusString(const char *message) {
//    snprintf(status_topic, 80, "%s/%s/%s", m_Hostname, m_Object, "status");
    client.publish(status_topic, message, true);
  };

// TODO: need to add a callback...
boolean Mqtt::subscribe(const char *host, const char *object, const char *verb) {
    char topic[81];
    snprintf(topic, 80, "%s/%s/%s", host, object, verb);
    return client.subscribe(topic);
  };
