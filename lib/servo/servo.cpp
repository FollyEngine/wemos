

#include "servo.h"
#include <ArduinoJson.h>

#include <Wire.h>
#include <Servo.h>  // esp8266 only


char *ServoDevice::deviceType = "servo";


// both to same angle (only used as a default, angle1 and angle2 override)
// mosquitto_pub -h mqtt -t esp8266-4c11aedd147/servo/angle -m '{"angle": 120}'
// servo 1
// mosquitto_pub -h mqtt -t esp8266-4c11aedd147/servo/angle -m '{"angle1": 120}'
// servo 2
// mosquitto_pub -h mqtt -t esp8266-4c11aedd147/servo/angle -m '{"angle2": 120}'
// servo 1 and 2 to different angles
// mosquitto_pub -h mqtt -t esp8266-4c11aedd147/servo/angle -m '{"angle1": 95, "angle2": 120}'

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

const char *ServoDevice::IsMessageForMe(const char * topic) {
  size_t length = strlen(ServoDevice::deviceType);
  if (strncmp(ServoDevice::deviceType, topic, length) == 0) {
    return topic+length+1;
  }
  return NULL;
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
  Serial.printf("%s loop Subscription returned: %s\r\n", ServoDevice::deviceType, initialised ? "true" : "false");
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

  int angle = obj["angle"] | 999;
  int angle1 = obj["angle1"] | angle;
  int angle2 = obj["angle2"] | angle;
  Serial.printf("Servo angle1: %d, angle1: %d\r\n", angle1, angle2);
  if (angle1 != 999) {
    servo1.write(angle1);   // tell servo to go to position
  }
  if (angle2 != 999) {
    servo2.write(angle2);   // tell servo to go to position
  }
}
