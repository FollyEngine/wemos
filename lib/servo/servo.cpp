

#include "servo.h"
#include <ArduinoJson.h>

#include <Wire.h>
#include <Servo.h>


char deviceType[] = "servo";


// mosquitto_pub -h mqtt -t esp8266-4c11aedd147/servo/angle -m '{"angle": 120}'

//#define ServoPin 14   //D5 is GPIO14
#define ServoPin 04   //D2 is GPIO04 and pin 13?

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

ServoDevice::ServoDevice(Mqtt *m) {
    mqtt = m;
}

void ServoDevice::setup() {
  //Serial.begin(115200);
  Serial.println("Servo Shield Testing...");

  myservo.attach(ServoPin); // attaches the servo on GIO2 to the servo object
}

void ServoDevice::loop() {
  if (!initialised) {

    delay(1);
    initialised = mqtt->subscribe(mqtt->getHostname(), deviceType, "angle");
    initialised = mqtt->subscribe("all", deviceType, "angle");
    Serial.printf("loop Subscription returned: %s\r\n", initialised ? "true" : "false");

    setup();
  }

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
  myservo.write(angle);   // tell servo to go to position
}
