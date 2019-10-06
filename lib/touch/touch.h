#ifndef FollyEngine_Touch_h
#define FollyEngine_Touch_h

#include <device.h>
#include <mqtt.h>

class TouchDevice: public Device {
    public:
    TouchDevice(Mqtt *mqtt);

    virtual void setup();
    virtual void loop();
    virtual void mqtt_callback_fn(const char* topic, const char* payload, unsigned int length);

    private:
    Mqtt *mqtt;
    boolean initialised = false;

    int threshold = 40;
    // Don't use T1 - its connected to reset
    uint8_t pinMap[8] = {T0, T8, T2, T3, T4, T5, T6, T7};
    bool current[8] = {false,false,false,false,false,false,false,false};
    bool last[8] = {false,false,false,false,false,false,false,false};
    bool touchState[8] = {false,false,false,false,false,false,false,false};
};

#endif