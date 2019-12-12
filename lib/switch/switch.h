#ifndef FollyEngine_Switch_h
#define FollyEngine_Switch_h

#include <device.h>
#include <mqtt.h>

class SwitchDevice: public Device {
    public:
    SwitchDevice(Mqtt *mqtt);

    virtual void setup();
    virtual void loop();
    virtual void subscribe();
    virtual const char* IsMessageForMe(const char *topic);
    virtual void mqtt_callback_fn(const char* topic, const char* payload, unsigned int length);

    private:
    static char *deviceType;
    Mqtt *mqtt;
    boolean initialised = false;

    // the current state of the button
    int buttonState = LOW;

    // the current and previous readings from the input pin
    int thisButtonState = LOW;
    int lastButtonState = LOW;

    // time is measured in milliseconds and will quickly exceed limitations of an integer, so we use a long for these two
    unsigned long lastDebounceTime = 0;  // the time the button state last switched
    unsigned long debounceDelay = 50;    // the state must remain the same for this many millis to register the button press
};

#endif