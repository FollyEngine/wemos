
#ifndef FollyEngine_NeopixelString_h
#define FollyEngine_NeopixelString_h

#include <device.h>
#include <mqtt.h>
#include <Adafruit_NeoPixel.h>

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
    NeopixelString(uint16_t ledPin, uint16_t ledNum, neoPixelType type, Mqtt *mqtt);

    virtual void setup();
    virtual void subscribe();
    virtual void loop();
    virtual const char* IsMessageForMe(const char *topic);
    virtual void mqtt_callback_fn(const char* topic, const char* payload, unsigned int length);

    private:
    Adafruit_NeoPixel *left_leds;

    static char *deviceType;
    Mqtt *mqtt;
    uint16_t ledPin;
    uint16_t ledNum;
    neoPixelType ledType;

    boolean initialised = false;
    boolean inLoop = false;

    unsigned long twinkleDelay;
    unsigned long twinkleUntil = 0;
};

#endif