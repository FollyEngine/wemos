
#include "helpers.h"


// this function sets all the pixels in a group to the same colour
void leds_set(Adafruit_NeoPixel &leds, uint8_t R, uint8_t G, uint8_t B) {
  for (uint16_t i = 0; i < leds.numPixels(); i++) {
    leds.setPixelColor(i, R, G, B);
    leds.show();
    //delay(50);
  }
}

uint8_t currentColours[3];
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
int moredim = 4;
int colour = 3;


void updateColourRGB(Adafruit_NeoPixel &leds, int red, int green, int blue) {
  red = red / moredim;
  green = green / moredim;
  blue = blue / moredim;


  leds_set(leds, red, green, blue);
  currentColours[0] = red;
  currentColours[1] = green;
  currentColours[2] = blue;
}


void updateColour(Adafruit_NeoPixel &leds, const char * colourName) {
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
  updateColourRGB(leds, myFavoriteColors[colour].colours[0],
    myFavoriteColors[colour].colours[1],
    myFavoriteColors[colour].colours[2]);
}

// from https://learn.adafruit.com/neopixel-pixie-dust-bag/arduino-code
#define DELAY_MILLIS 10  // how long each light stays bright for, smaller numbers are faster 
#define DELAY_MULT 8     // Randomization multiplier on the delay speed of the effect

bool oldState = HIGH; //sets the initial variable for counting touch sensor button pushes

void pixie_dust(Adafruit_NeoPixel &leds, int bright, unsigned long twinkleDelay) {
  //color (0-255) values to be set by cycling touch switch, initially GOLD
  uint8_t red = currentColours[0];
  uint8_t green = currentColours[1];
  uint8_t blue = currentColours[2];

  //sparkling
  int p = random(leds.numPixels()); //select a random pixel
  leds.setPixelColor(p, red*bright, green*bright, blue*bright); 
  leds.show();
  delay(twinkleDelay * random(DELAY_MULT) ); //delay value randomized to up to DELAY_MULT times longer
  leds.setPixelColor(p, red, green, blue);
  leds.show();
  delay(twinkleDelay * random(DELAY_MULT) ); //delay value randomized to up to DELAY_MULT times longer
}


// Fadeout... starts at bright white and fades to almost zero
void fadeout(Adafruit_NeoPixel & leds) {
  // swap these two loops to spin around the LEDs
  for (uint16_t fade = 255; fade > 0; fade = fade - 17) {
    for (uint16_t i = 0; i < leds.numPixels(); i++) {
      // now we will 'fade' it in steps
      leds.setPixelColor(i, leds.Color(fade, fade, fade));
    }
    leds.show();
    delay(5); // milliseconds
  }
  // now make sure they're all set to 0
  for (uint16_t i = 0; i < leds.numPixels(); i++) {
    leds.setPixelColor(i, leds.Color(0, 0, 0));
  }
  leds.show();
}


// first number is 'wait' delay, shorter num == shorter twinkle
// second number is how many neopixels to simultaneously light up
// THIS FUNCTION IS NOT USED AND PROBABLY DOESN'T WORK
void flashRandom(Adafruit_NeoPixel &leds, int wait, uint8_t howmany) {

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
