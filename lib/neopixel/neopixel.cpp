

#include "neopixel.h"
#include <ArduinoJson.h>
#include "helpers.h"

char deviceType[] = "neopixel";

// TODO: set brightness and colour set from mqtt payload
//#define BRIGHTNESS 140   // 140 is reasonably bright

NeopixelString::NeopixelString(uint16_t pin, uint16_t num, neoPixelType type, Mqtt *m) {
    ledPin = pin;
    ledNum = num;
    ledType = type;
    mqtt = m;
}

void NeopixelString::setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin, LOW);

  left_leds = new Adafruit_NeoPixel(ledNum, ledPin, ledType);

  left_leds->begin(); // This initializes the NeoPixel library.

  updateColourRGB(*left_leds, 0, 0, 0);

  //left_leds.setBrightness(BRIGHTNESS);
  //left_leds.show();                // turn on all pixels
}

void NeopixelString::loop() {
  if (!initialised) {
    digitalWrite(ledPin, HIGH);

    delay(1);
    initialised = mqtt->subscribe(mqtt->getHostname(), deviceType, "twinkle");
    initialised = mqtt->subscribe("all", deviceType, "twinkle");
    Serial.printf("loop Subscription returned: %s\n", initialised ? "true" : "false");

    setup();
  }

  if (twinkleUntil > millis()) {
    // sparkle each led once (but in random order, so some of them might sparkle twice and some not at all)
    // TODO actually just choose a random LED and sparkle it, since this loop now repeats every time
    //for (int i = 0; i < ledNum; i++) {
    pixie_dust(*left_leds, 5, twinkleDelay);
    //}
  }
}


void NeopixelString::mqtt_callback_fn(const char* topic, const char* payload, unsigned int length) {
  // 10 elements in the json payload?
//   DynamicJsonBuffer jb(JSON_OBJECT_SIZE(10));
//   JsonObject& obj = jb.parseObject(payload);
  StaticJsonDocument<300> obj;
  DeserializationError error = deserializeJson(obj, payload);
  if (error) {
    Serial.printf("Failed to parse JSON: %s\n", error.c_str());
    return;
  }

  const char* operation = obj["operation"];
  if (operation != NULL) {
    if (strcmp(operation, "setrgb") == 0) {
      const int red = obj["r"] | 0;
      const int green = obj["g"] | 0;
      const int blue = obj["b"] | 0;
      //const float alpha = obj["a"] | 0.5;
      updateColourRGB(*left_leds, red, green, blue);
    }

    if (strcmp(operation, "set") == 0) {
      const char* colour = obj["colour"] | "off";
      updateColour(*left_leds, colour);
    }
    if (strcmp(operation, "twinkle") == 0) {
      unsigned long duration = obj["duration"] | 1000; // default to one second
      twinkleDelay = obj["delay"] | 5;
      twinkleUntil = millis() + duration;
    }
  }
}

