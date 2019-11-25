#ifndef FollyEngine_Servo_h
#define FollyEngine_Servo_h

#include <device.h>
#include <mqtt.h>

class ServoDevice: public Device {
    public:
    ServoDevice(Mqtt *mqtt);

    virtual void setup();
    virtual void loop();
    virtual void subscribe();
    virtual const char* IsMessageForMe(const char *topic);
    virtual void mqtt_callback_fn(const char* topic, const char* payload, unsigned int length);

    private:
    static char *deviceType;
    Mqtt *mqtt;
    boolean initialised = false;
};

#endif