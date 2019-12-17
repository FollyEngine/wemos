

#include "neopixel.h"
#include <ArduinoJson.h>
#include "helpers.h"

const char *NeopixelString::deviceType = "neopixel";

// TODO: set brightness and colour set from mqtt payload
//#define BRIGHTNESS 140   // 140 is reasonably bright

NeopixelString::NeopixelString(CRGB *leds, uint len, Mqtt *m) {
    mqtt = m;
    left_leds = leds;
    ledLen = len;
}

void NeopixelString::setup() {
fill_solid(left_leds, ledLen, CRGB::Black);
FastLED.show();

  subscribe();
}

void NeopixelString::subscribe() {
    initialised = mqtt->subscribe(mqtt->getHostname(), deviceType, "twinkle");
    initialised = mqtt->subscribe("all", deviceType, "twinkle");
    Serial.printf("%s loop Subscription returned: %s\r\n", NeopixelString::deviceType, initialised ? "true" : "false");
}

const char *NeopixelString::IsMessageForMe(const char * topic) {
  size_t length = strlen(NeopixelString::deviceType);
  if (strncmp(NeopixelString::deviceType, topic, length) == 0) {
    return topic+length+1;
  }
  return NULL;
}

void NeopixelString::loop() {
  if (!initialised) {
    delay(100);

    setup();
  }

  if (operationDuration > millis()) {
    if (strcmp(operation, "twinkle") == 0) {
      // sparkle each led once (but in random order, so some of them might sparkle twice and some not at all)
      // TODO actually just choose a random LED and sparkle it, since this loop now repeats every time
      //for (int i = 0; i < ledNum; i++) {
      pixie_dust(left_leds, ledLen, 5, operationDelay);
      //}
    }
    if (strcmp(operation, "disco") == 0) {
      JsonArrayConst colours = obj["colours"];
      for (int i=0; i < colours.size(); i++) {
        //Serial.println(v.as<String>());
        String colour = colours[i].as<String>();
        Serial.printf("disco: %s\r\n", colour.c_str());
        updateColour(left_leds, ledLen, colour.c_str());
        delay(operationDelay);
      }
    }
  }
}


void NeopixelString::mqtt_callback_fn(const char* topic, const char* payload, unsigned int length) {
  if (operationDuration > millis()) {
    Serial.printf("busy, ignoring %s\r\n", payload);
    return; // we're busy, so ignore other messages
  }
  // 10 elements in the json payload?
//   DynamicJsonBuffer jb(JSON_OBJECT_SIZE(10));
//   JsonObject& obj = jb.parseObject(payload);
  DeserializationError error = deserializeJson(obj, payload);
  if (error) {
    Serial.printf("Failed to parse JSON: %s\n", error.c_str());
    return;
  }

  operation = obj["operation"];
  if (operation != NULL) {
    if (strcmp(operation, "setrgb") == 0) {
      const int red = obj["r"] | 0;
      const int green = obj["g"] | 0;
      const int blue = obj["b"] | 0;
      //const float alpha = obj["a"] | 0.5;
      updateColourRGB(left_leds, ledLen, red, green, blue);
    }

    if (strcmp(operation, "set") == 0) {
      const char* colour = obj["colour"] | "off";
      updateColour(left_leds, ledLen, colour);
    }
    if (strcmp(operation, "twinkle") == 0) {
      unsigned long duration = obj["duration"] | 1000; // default to one second
      operationDuration = millis() + duration;
      operationDelay = obj["delay"] | 5;
    }
    if (strcmp(operation, "disco") == 0) {
      unsigned long duration = obj["duration"] | 1000; // default to one second
      operationDuration = millis() + duration;
      operationDelay = obj["delay"] | 100;
      // Scan in a sequence and the mushrooms change constantly like disco lights for 30 seconds 
      // (1. Pink, 2. Green, 3. Blue, 4. Yellow)
      // can send duration=30s, delay=0.5(s), colours=[rgb, rbg, rgb, colourname] ?
    }
    if (strcmp(operation, "transition") == 0) {
      unsigned long duration = obj["duration"] | 1000; // default to one second
      operationDuration = millis() + duration;
      operationDelay = obj["delay"] | 100;
      // smooth transition from colour ot colour
      // can send duration=30s, delay=0.5(s), colours=[rgb, rbg, rgb, colourname] ?
      // duration is total time, delay is time from one colour to the next
    }
  }
}

