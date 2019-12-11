

#include "relay.h"
#include <ArduinoJson.h>

//#include <Wire.h>


char *RelayDevice::deviceType = "relay";

// mosquitto_pub -h mqtt -t esp8266-4c11aedd147/relay/switch -m '{"relay": "on"}'

// use D1
#define RelayPin1 D1

RelayDevice::RelayDevice(Mqtt *m) {
    mqtt = m;
}

void RelayDevice::setup() {
  //Serial.begin(115200);
  Serial.println("Relay Shield Testing...");
  pinMode(RelayPin1, OUTPUT);
}

const char *RelayDevice::IsMessageForMe(const char * topic) {
  size_t length = strlen(RelayDevice::deviceType);
  if (strncmp(RelayDevice::deviceType, topic, length) == 0) {
    return topic+length+1;
  }
  return NULL;
}

void RelayDevice::loop() {
  if (!initialised) {
    delay(1);
    setup();
    subscribe();
  }
}

// TODO: add a status topic to request what the current state is
void RelayDevice::subscribe() {
  initialised = mqtt->subscribe(mqtt->getHostname(), deviceType, "switch");
  initialised = mqtt->subscribe("all", deviceType, "switch");
  Serial.printf("%s loop Subscription returned: %s\r\n", RelayDevice::deviceType, initialised ? "true" : "false");
}


void RelayDevice::mqtt_callback_fn(const char* topic, const char* payload, unsigned int length) {
  // 10 elements in the json payload?
//   DynamicJsonBuffer jb(JSON_OBJECT_SIZE(10));
//   JsonObject& obj = jb.parseObject(payload);
  StaticJsonDocument<300> obj;
  DeserializationError error = deserializeJson(obj, payload);
  if (error) {
    Serial.printf("Failed to parse JSON: %s\n", error.c_str());
    return;
  }

  String cmd = obj["relay"] | "off";
  Serial.printf("Relay set: %s\r\n", cmd.c_str());
  if (cmd == "on") {
    digitalWrite(RelayPin1, !LOW);
  } else {
    digitalWrite(RelayPin1, LOW);
  }
}
