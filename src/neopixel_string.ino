#include "secrets.h"
#include <mqtt.h>

#include <ArduinoJson.h>

#include <Adafruit_NeoPixel.h>

char deviceType[] = "neopixel";


//
//breif:
//change all pixels to a specified colour
//length of transition from one colour to another
//and make it twinkle for a length of time (using the specified colour as the background

// Listen to mqtt messages and change LEDs in response.  Test with a message like
// mosquitto_pub -h "mqtt" -t "all/neopixel/twinkle" -m '{"operation": "set"}'
// mosquitto_pub -h "mqtt" -t "all/neopixel/twinkle" -m '{"operation": "twinkle", "duration": 1000}'

// GO READ https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/

//BUILD with "LOLIN(WEMOS) D1 R2 & mini"
Mqtt mqtt = Mqtt(SECRET_SSID, SECRET_PASSWORD, "10.10.10.129", 1883, deviceType);

// D4 is the default pin for the 6 LED RBG shield
// https://wiki.wemos.cc/products:d1_mini_shields:rgb_led_shield
// https://github.com/wemos/D1_mini_Examples/blob/master/examples/04.Shields/RGB_LED_Shield/simple/simple.ino
#define LEDPIN   D4
#define LED_NUM 50


// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
//#define LEDPIN   D5
//#define LED_NUM 50


Adafruit_NeoPixel left_leds = Adafruit_NeoPixel(LED_NUM, LEDPIN, NEO_GRB + NEO_KHZ800);

unsigned long twinkleUntil = 0;
void mqtt_callback_fn(const char* topic, const byte* raw_payload, unsigned int length) {
  char payload[256];
  strncpy(payload, (char *) raw_payload, length);
  payload[length] = 0;
  Serial.printf("Callback: %s: %s\n", topic, payload);

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
      updateColour(colour);
    }
    if (strcmp(operation, "twinkle") == 0) {
      unsigned long duration = obj["duration"] | 1000; // default to one second
      twinkleUntil = millis() + duration;
    }
  }
}

boolean initialised = false;

// TODO: set brightness and colour set from mqtt payload
//#define BRIGHTNESS 140   // 140 is reasonably bright

void setup() {
  //while (!Serial);
  Serial.begin(115200);
  Serial.printf("Started\n");


  // initialize the LED pin as an output:
  pinMode(LEDPIN, OUTPUT);
  pinMode(LEDPIN, LOW);

  left_leds.begin(); // This initializes the NeoPixel library.
  //left_leds.setBrightness(BRIGHTNESS);
  left_leds.show();                // turn on all pixels

  mqtt.setCallback(mqtt_callback_fn);
  mqtt.setup();

}

// this function sets all the pixels in a group to the same colour
void leds_set(Adafruit_NeoPixel &leds, uint8 R, uint8 G, uint8 B) {
  for (uint16_t i = 0; i < LED_NUM; i++) {
    leds.setPixelColor(i, R, G, B);
    leds.show();
    //delay(50);
  }
}


typedef struct {
  char* name;
  uint8_t colours[3];
} colour_def;
// from https://learn.adafruit.com/sparkle-skirt/code-battery
// Here is where you can put in your favorite colors that will appear!
// just add new {nnn, nnn, nnn}, lines.
colour_def myFavoriteColors[] = {
  {"gold", {255,   222,  30}},   // Pixie GOLD
  {"blue", {50, 255, 255}},    // Alchemy BLUE
  {"orange", {255, 100, 0}},     // Animal Orange
  {"purple", {242,    90, 255}},   // Garden PINK
  {"green", {0,    255, 40}},   // Tinker GREEN
  {"off", {0, 0, 0}},
  {"white", {180, 180, 180}},
  {"red", {255, 0, 0}},
  {"green", {0, 255, 0}},
  {"blue", {0, 0, 255}},
  {"yellow", {255, 255, 0}},
};
#define FAVCOLORS 11u

int dim = 20;
int moredim = 50;
int colour = 3;

void updateColour(const char * colourName) {
  for (int i = 0; i < FAVCOLORS; i++) {
    if (strcmp(colourName, myFavoriteColors[i].name) == 0) {
      colour = i;
      break;
    }
  }
  Serial.printf("Asked for %s, got %s\n", colourName, myFavoriteColors[colour].name);
  //  if (colour > FAVCOLORS) {
  //    colour = 0;
  //  }
  leds_set(left_leds,
           myFavoriteColors[colour].colours[0] / moredim,
           myFavoriteColors[colour].colours[1] / moredim,
           myFavoriteColors[colour].colours[2] / moredim);

}

void loop() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  mqtt.loop();

  if (!initialised) {
    //leds_set(left_leds, 0, 0, 0);
    // turn off the cpu board led
    digitalWrite(LEDPIN, HIGH);

    delay(1);
    initialised = mqtt.subscribe(mqtt.getHostname(), deviceType, "twinkle");
    initialised = mqtt.subscribe("all", deviceType, "twinkle");
    Serial.printf("loop Subscription returned: %s\n", initialised ? "true" : "false");

    leds_set(left_leds,
             myFavoriteColors[colour].colours[0] / moredim,
             myFavoriteColors[colour].colours[1] / moredim,
             myFavoriteColors[colour].colours[2] / moredim);

    for (int i = 0; i < 2 * LED_NUM; i++) {
      pixie_dust(left_leds, colour);
      delay(1);
    }
  }

  if (twinkleUntil > millis()) {
    // sparkle each led once (but in random order, so some of them might sparkle twice and some not at all)
    // TODO actually just choose a random LED and sparkle it, since this loop now repeats every time
    //for (int i = 0; i < LED_NUM; i++) {
    pixie_dust(left_leds, colour);
    delay(2);
    //}
  }
}


// from https://learn.adafruit.com/neopixel-pixie-dust-bag/arduino-code
#define DELAY_MILLIS 10  // how long each light stays bright for, smaller numbers are faster 
#define DELAY_MULT 8     // Randomization multiplier on the delay speed of the effect

bool oldState = HIGH; //sets the initial variable for counting touch sensor button pushes

void pixie_dust(Adafruit_NeoPixel & leds, int showColor) {
  //color (0-255) values to be set by cycling touch switch, initially GOLD
  uint8 red = myFavoriteColors[colour].colours[0];
  uint8 green = myFavoriteColors[colour].colours[1];
  uint8 blue = myFavoriteColors[colour].colours[2];

  //sparkling
  int p = random(LED_NUM); //select a random pixel
  leds.setPixelColor(p, red, green, blue); //color value comes from cycling state of momentary switch
  leds.show();
  delay(DELAY_MILLIS * random(DELAY_MULT) ); //delay value randomized to up to DELAY_MULT times longer
  leds.setPixelColor(p, red / dim, green / dim, blue / dim); //set to a dimmed version of the state color
  leds.show();
  leds.setPixelColor(p + 1, red / moredim, green / moredim, blue / moredim); //set a neighbor pixel to an even dimmer value
  leds.show();

  //      //button check to cycle through color value sets
  //      bool newState = digitalRead(TOUCH_PIN); //Get the current button state
  //      // Check if state changed from high to low (button press).
  //      if (newState == LOW && oldState == HIGH) {
  //        // Short delay to debounce button.
  //        delay(20);
  //        // Check if button is still low after debounce.
  //        newState = digitalRead(TOUCH_PIN);
  //        if (newState == LOW) {
  //          showColor++;
  //          if (showColor > 4)
  //            showColor=0;
  //           }
  //      }
  // Set the last button state to the old state.
  //      oldState = newState;

}




// Fadeout... starts at bright white and fades to almost zero
void fadeout(Adafruit_NeoPixel & leds) {
  // swap these two loops to spin around the LEDs
  for (uint16_t fade = 255; fade > 0; fade = fade - 17) {
    for (uint16_t i = 0; i < LED_NUM; i++) {
      // now we will 'fade' it in steps

      leds.setPixelColor(i, leds.Color(fade, fade, fade));
    }
    leds.show();
    delay(5); // milliseconds
  }
  // now make sure they're all set to 0
  for (uint16_t i = 0; i < LED_NUM; i++) {
    leds.setPixelColor(i, leds.Color(0, 0, 0));
  }
  leds.show();

}


// first number is 'wait' delay, shorter num == shorter twinkle
// second number is how many neopixels to simultaneously light up
// THIS FUNCTION IS NOT USED AND PROBABLY DOESN'T WORK
void flashRandom(Adafruit_NeoPixel & leds, int wait, uint8_t howmany) {

  for (uint16_t i = 0; i < howmany; i++) {
    // pick a random favorite color!
    int c = random(FAVCOLORS);
    int red = myFavoriteColors[c].colours[0];
    int green = myFavoriteColors[c].colours[1];
    int blue = myFavoriteColors[c].colours[2];

    // get a random pixel from the list
    int j = random(leds.numPixels());
    //Serial.print("Lighting up "); Serial.println(j);

    // now we will 'fade' it in 5 steps
    for (int x = 0; x < 5; x++) {
      int r = red * (x + 1); r /= 5;
      int g = green * (x + 1); g /= 5;
      int b = blue * (x + 1); b /= 5;

      leds.setPixelColor(j, leds.Color(r, g, b));
      leds.show();
      delay(wait);
    }
    // & fade out in 5 steps
    for (int x = 5; x >= 0; x--) {
      int r = red * x; r /= 5;
      int g = green * x; g /= 5;
      int b = blue * x; b /= 5;

      leds.setPixelColor(j, leds.Color(r, g, b));
      leds.show();
      delay(wait);
    }
  }
  // LEDs will be off when done (they are faded to 0)
}
