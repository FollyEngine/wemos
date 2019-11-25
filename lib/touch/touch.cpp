#include "touch.h"
#include <ArduinoJson.h>

char *TouchDevice::deviceType = "touch";

TouchDevice::TouchDevice(Mqtt *m) {
    mqtt = m;
}

void TouchDevice::setup() {
  //Serial.begin(115200);
  //Serial.println("ESP32 Touch Interrupt Test");
  threshold = 40;
}

void TouchDevice::subscribe() {

}

void TouchDevice::loop() {
  if (!initialised) {

    delay(1);
    // initialised = mqtt->subscribe(mqtt->getHostname(), deviceType, "speed");
    // initialised = mqtt->subscribe("all", deviceType, "speed");
    // Serial.printf("loop Subscription returned: %s\r\n", initialised ? "true" : "false");

    setup();
  }

  StaticJsonDocument<300> root;
  char name[16];

  initialised = mqtt->loop();

  bool stateChange = false;
  for (int i=0; i<8; i++) {
    if (i == 7) {
      continue; // touch 7 was giving us trouble
    }
    int16_t reading = touchRead(pinMap[i]);

    if (reading <= threshold) {
      current[i] = true;
    }
    if (current[i] == last[i] && last[i] != touchState[i]) {
      // need to report this as a state change.
      stateChange = true;
      touchState[i] = current[i];
      if (current[i]) {
        Serial.printf("Touch %d detected (reading: %d)\r\n", i, reading);
      } else {
        Serial.printf("Touch %d released (reading: %d)\r\n", i, reading);
      }
      snprintf(name, 16, "touch%d", i);
      root[name] = touchState[i];
    }

    // prepare for next loop
    last[i] = current[i];
    current[i] = false;
  }
  if (stateChange) {
      mqtt->publish(mqtt->getObject(), "touch", root);
  }
  
  //Serial.printf("w(%s) %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n",
    // (stateChange?"true":"false"),
    // // touchState[0], touchState[1], touchState[2], touchState[3], touchState[4],
    // // touchState[5], touchState[6], touchState[7]);
    // touchRead(T0), touchRead(T1), touchRead(T2), touchRead(T3), touchRead(T4), touchRead(T5), touchRead(T6), touchRead(T7), touchRead(T8), touchRead(T9));  // get value using T0
  delay(50);
}

void TouchDevice::mqtt_callback_fn(const char* topic, const char* payload, unsigned int length) {
  // 10 elements in the json payload?
//   DynamicJsonBuffer jb(JSON_OBJECT_SIZE(10));
//   JsonObject& obj = jb.parseObject(payload);
  StaticJsonDocument<300> obj;
  DeserializationError error = deserializeJson(obj, payload);
  if (error) {
    Serial.printf("Failed to parse JSON: %s\n", error.c_str());
    return;
  }
}

