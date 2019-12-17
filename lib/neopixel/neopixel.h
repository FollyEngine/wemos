
#ifndef FollyEngine_NeopixelString_h
#define FollyEngine_NeopixelString_h

#include <device.h>
#include <mqtt.h>
#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>

class NeopixelString: public Device {
    public:

// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
    NeopixelString(CRGB *leds, uint len, Mqtt *mqtt);

    virtual void setup();
    virtual void subscribe();
    virtual void loop();
    virtual const char* IsMessageForMe(const char *topic);
    virtual void mqtt_callback_fn(const char* topic, const char* payload, unsigned int length);

    private:
    CRGB *left_leds;
    uint ledLen;

    static const char *deviceType;
    Mqtt *mqtt;

    boolean initialised = false;
    boolean inLoop = false;

    const char* operation = NULL;
    unsigned long operationDelay;
    unsigned long operationDuration = 0;
    StaticJsonDocument<500> obj;
};

#endif