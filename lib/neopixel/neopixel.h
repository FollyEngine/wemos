
#ifndef FollyEngine_NeopixelString_h
#define FollyEngine_NeopixelString_h

#include <mqtt.h>
#include <Adafruit_NeoPixel.h>

class Device {
    public:
    virtual void setup();
    virtual void loop();
    virtual void mqtt_callback_fn(const char* topic, const char* payload, unsigned int length);
};

class NeopixelString: public Device {
    public:

    NeopixelString(int ledPin, int ledNum, Mqtt *mqtt);

    virtual void setup();
    virtual void loop();
    virtual void mqtt_callback_fn(const char* topic, const char* payload, unsigned int length);

    private:
    Adafruit_NeoPixel left_leds;

    Mqtt *mqtt;
    int ledPin;
    int ledNum;

    boolean initialised = false;

    unsigned long twinkleUntil = 0;
};

#endif