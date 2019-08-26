
#include "neopixel.h"


// this function sets all the pixels in a group to the same colour
void leds_set(Adafruit_NeoPixel &leds, uint8 R, uint8 G, uint8 B);

void updateColourRGB(Adafruit_NeoPixel &leds, int red, int green, int blue);

void updateColour(Adafruit_NeoPixel &leds, const char * colourName);

void pixie_dust(Adafruit_NeoPixel &leds, int bright);
// Fadeout... starts at bright white and fades to almost zero
void fadeout(Adafruit_NeoPixel & leds);


// first number is 'wait' delay, shorter num == shorter twinkle
// second number is how many neopixels to simultaneously light up
// THIS FUNCTION IS NOT USED AND PROBABLY DOESN'T WORK
void flashRandom(Adafruit_NeoPixel &leds, int wait, uint8_t howmany);