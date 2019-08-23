

#include "neopixel.h"
#include <ArduinoJson.h>
#include "helpers.h"

char deviceType[] = "neopixel";

// TODO: set brightness and colour set from mqtt payload
//#define BRIGHTNESS 140   // 140 is reasonably bright

NeopixelString::NeopixelString(int pin, int num, Mqtt *m) {
    ledPin = pin;
    ledNum = num;
    mqtt = m;
}

void NeopixelString::setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin, LOW);

  left_leds = Adafruit_NeoPixel(ledNum, ledPin, NEO_GRB + NEO_KHZ800);

  left_leds.begin(); // This initializes the NeoPixel library.
  //left_leds.setBrightness(BRIGHTNESS);
  left_leds.show();                // turn on all pixels
}

void NeopixelString::loop() {
  if (!initialised) {
    //leds_set(left_leds, 0, 0, 0);
    // turn off the cpu board led
    digitalWrite(ledPin, HIGH);

    delay(1);
    initialised = mqtt->subscribe(mqtt->getHostname(), deviceType, "twinkle");
    initialised = mqtt->subscribe("all", deviceType, "twinkle");
    Serial.printf("loop Subscription returned: %s\n", initialised ? "true" : "false");

    leds_set(left_leds,
             myFavoriteColors[colour].colours[0] / moredim,
             myFavoriteColors[colour].colours[1] / moredim,
             myFavoriteColors[colour].colours[2] / moredim);

    for (int i = 0; i < 2 * ledPin; i++) {
      pixie_dust(left_leds, colour);
      delay(1);
    }
  }

  if (twinkleUntil > millis()) {
    // sparkle each led once (but in random order, so some of them might sparkle twice and some not at all)
    // TODO actually just choose a random LED and sparkle it, since this loop now repeats every time
    //for (int i = 0; i < ledNum; i++) {
    pixie_dust(left_leds, colour);
    delay(2);
    //}
  }
}


void NeopixelString::mqtt_callback_fn(const char* topic, const char* payload, unsigned int length) {
  // 10 elements in the json payload?
  DynamicJsonBuffer jb(JSON_OBJECT_SIZE(10));
  JsonObject& obj = jb.parseObject(payload);
  if (!obj.success()) {
    Serial.printf("Failed to parse JSON\n");
    return;
  }

  const char* operation = obj["operation"];
  if (operation != NULL) {
    if (strcmp(operation, "set") == 0) {
      const char* colour = obj["colour"] | "off";
      updateColour(left_leds, colour);
    }
    if (strcmp(operation, "twinkle") == 0) {
      unsigned long duration = obj["duration"] | 1000; // default to one second
      twinkleUntil = millis() + duration;
    }
  }
}

