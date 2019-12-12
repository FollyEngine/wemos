

#include "switch.h"
#include <ArduinoJson.h>

//#include <Wire.h>


char *SwitchDevice::deviceType = "switch";

// esp8266-4c11aedd362/multipass/button {"state":0,"device":"multipass","time":"2019-12-12T08:14:44"}


// use D3
#define SwitchPin1 D3

SwitchDevice::SwitchDevice(Mqtt *m) {
    mqtt = m;
}

void SwitchDevice::setup() {
  //Serial.begin(115200);
  Serial.println("Switch Shield Testing...");
  pinMode(SwitchPin1, INPUT);
}

const char *SwitchDevice::IsMessageForMe(const char * topic) {
  size_t length = strlen(SwitchDevice::deviceType);
  if (strncmp(SwitchDevice::deviceType, topic, length) == 0) {
    return topic+length+1;
  }
  return NULL;
}

void SwitchDevice::loop() {
  if (!initialised) {
    delay(1);
    setup();
    subscribe();
  }
  // the buttonPin is read multiple times and the value must remain the same for debounceDelay millis to toggle the LED

  // read button state, HIGH when pressed, LOW when not
  thisButtonState = digitalRead(SwitchPin1);

  // if the current state does not match the previous state
  // the button was just pressed/released, or is transition noise
  if (thisButtonState != lastButtonState) {
    // reset the timer
    lastDebounceTime = millis();
  }

  // once delay millis have elapsed, if the state remains the same, register the press
  if ((millis() - lastDebounceTime) > debounceDelay) {

    // if the button state has changed
    if (thisButtonState != buttonState) {
      buttonState = thisButtonState;
      StaticJsonDocument<300> root;
      root["state"] = buttonState;
      mqtt->publish(mqtt->getObject(), "button", root);
    }
  }

  // persist for next loop iteration
  lastButtonState = thisButtonState;
}

// TODO: add a status topic to request what the current state is
void SwitchDevice::subscribe() {
  initialised = true;
}

void SwitchDevice::mqtt_callback_fn(const char* topic, const char* payload, unsigned int length) {
  // 10 elements in the json payload?
//   DynamicJsonBuffer jb(JSON_OBJECT_SIZE(10));
//   JsonObject& obj = jb.parseObject(payload);
  StaticJsonDocument<300> obj;
  DeserializationError error = deserializeJson(obj, payload);
  if (error) {
    Serial.printf("Failed to parse JSON: %s\n", error.c_str());
    return;
  }

  String cmd = obj["Switch"] | "off";
  Serial.printf("Switch set: %s\r\n", cmd.c_str());
  if (cmd == "on") {
    digitalWrite(SwitchPin1, !LOW);
  } else {
    digitalWrite(SwitchPin1, LOW);
  }
}
