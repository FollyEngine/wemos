

#include "servo.h"
#include <ArduinoJson.h>

#include <Wire.h>
#include <Servo.h>  // esp8266 only


char *ServoDevice::deviceType = "servo";


// mosquitto_pub -h mqtt -t esp8266-4c11aedd147/servo/angle -m '{"angle": 120}'

// use D3 (GPIO0) and 
#define ServoPin1 00
// D6 (GPIO12)
#define ServoPin2 12



Servo servo1;  // create servo object to control a servo
Servo servo2;  // create servo object to control a servo
// twelve servo objects can be created on most boards

ServoDevice::ServoDevice(Mqtt *m) {
    mqtt = m;
}

void ServoDevice::setup() {
  //Serial.begin(115200);
  Serial.println("Servo Shield Testing...");

  servo1.attach(ServoPin1);
  servo2.attach(ServoPin2);
}

void ServoDevice::loop() {
  if (!initialised) {
    delay(1);
    setup();
    subscribe();
  }
}

void ServoDevice::subscribe() {
  initialised = mqtt->subscribe(mqtt->getHostname(), deviceType, "angle");
  initialised = mqtt->subscribe("all", deviceType, "angle");
  Serial.printf("loop Subscription returned: %s\r\n", initialised ? "true" : "false");
}


void ServoDevice::mqtt_callback_fn(const char* topic, const char* payload, unsigned int length) {
  // 10 elements in the json payload?
//   DynamicJsonBuffer jb(JSON_OBJECT_SIZE(10));
//   JsonObject& obj = jb.parseObject(payload);
  StaticJsonDocument<300> obj;
  DeserializationError error = deserializeJson(obj, payload);
  if (error) {
    Serial.printf("Failed to parse JSON: %s\n", error.c_str());
    return;
  }

  int angle = obj["angle"] | 0;
  Serial.printf("Servo angle: %d\r\n", angle);
  servo1.write(angle);   // tell servo to go to position
  servo2.write(angle);   // tell servo to go to position
}
