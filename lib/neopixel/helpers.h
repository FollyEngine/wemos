
#include "neopixel.h"


// this function sets all the pixels in a group to the same colour
void leds_set(CRGB *leds, uint len, uint8_t R, uint8_t G, uint8_t B);

void updateColourRGB(CRGB *leds, uint len, int red, int green, int blue);

void updateColour(CRGB *leds, uint len, const char * colourName);

void pixie_dust(CRGB *leds, uint len, int bright, unsigned long twinkleDelay);
// Fadeout... starts at bright white and fades to almost zero
void fadeout(CRGB *leds, uint len);
