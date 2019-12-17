
#include "helpers.h"


// this function sets all the pixels in a group to the same colour
void leds_set(CRGB *leds, uint len, uint8_t R, uint8_t G, uint8_t B) {
  fill_solid(leds, len, CRGB(R, G, B));
  FastLED.show();
}

uint8_t currentColours[3];
typedef struct {
  const char* name;
  CRGB colour;
} colour_def;
// from https://learn.adafruit.com/sparkle-skirt/code-battery
// Here is where you can put in your favorite colors that will appear!
// just add new {nnn, nnn, nnn}, lines.
colour_def myFavoriteColors[] = {
  {"gold", CRGB::Gold},
  {"blue", CRGB::Blue},
  {"orange", CRGB::Orange},
  {"purple", CRGB::Purple},
  {"green", CRGB::Green},
  {"off", CRGB::Black},
  {"white", CRGB::White},
  {"red", CRGB::Red},
  {"yellow", CRGB::Yellow},
  {"pink", CRGB::Pink}
};
#define FAVCOLORS 11u

int dim = 20;
int moredim = 4;
int colour = 3;


void updateColourRGB(CRGB *leds, uint len, int red, int green, int blue) {
  red = red / moredim;
  green = green / moredim;
  blue = blue / moredim;


  leds_set(leds, len, red, green, blue);
  currentColours[0] = red;
  currentColours[1] = green;
  currentColours[2] = blue;
}


void updateColour(CRGB *leds, uint len, const char * colourName) {
  for (uint i = 0; i < FAVCOLORS; i++) {
    if (strcmp(colourName, myFavoriteColors[i].name) == 0) {
      colour = i;
      break;
    }
  }
  Serial.printf("Asked for %s, got %s\n", colourName, myFavoriteColors[colour].name);

  fill_solid(leds, len, myFavoriteColors[colour].colour);
  FastLED.show();

}

// from https://learn.adafruit.com/neopixel-pixie-dust-bag/arduino-code
#define DELAY_MILLIS 10  // how long each light stays bright for, smaller numbers are faster 
#define DELAY_MULT 8     // Randomization multiplier on the delay speed of the effect

bool oldState = HIGH; //sets the initial variable for counting touch sensor button pushes

void pixie_dust(CRGB *leds, uint len, int bright, unsigned long twinkleDelay) {
  //color (0-255) values to be set by cycling touch switch, initially GOLD
  uint8_t red = currentColours[0];
  uint8_t green = currentColours[1];
  uint8_t blue = currentColours[2];

  //sparkling
  int p = random(len-1); //select a random pixel
  leds[p] = CRGB(red*bright, green*bright, blue*bright);
  FastLED.show();

  delay(twinkleDelay * random(DELAY_MULT) ); //delay value randomized to up to DELAY_MULT times longer
  leds[p] = CRGB(red, green, blue);
  FastLED.show();
  delay(twinkleDelay * random(DELAY_MULT) ); //delay value randomized to up to DELAY_MULT times longer
}


// Fadeout... starts at bright white and fades to almost zero
void fadeout(CRGB *leds, uint len) {
  // swap these two loops to spin around the LEDs
  for (uint16_t fade = 255; fade > 0; fade = fade - 17) {
    for (uint16_t i = 0; i < len; i++) {
      // now we will 'fade' it in steps
      leds[i] = CRGB(fade, fade, fade);
    }
    FastLED.show();
    delay(5); // milliseconds
  }
  // now make sure they're all set to 0
  for (uint16_t i = 0; i < len; i++) {
    leds[i] = CRGB(0, 0, 0);
  }
  FastLED.show();
}
