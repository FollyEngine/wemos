#ifndef FollyEngine_Motor_h
#define FollyEngine_Motor_h

#include <device.h>
#include <mqtt.h>

class MotorDevice: public Device {
    public:
    MotorDevice(Mqtt *mqtt);

    virtual void setup();
    virtual void subscribe();
    virtual void loop();
    virtual const char* IsMessageForMe(const char *topic);
    virtual void mqtt_callback_fn(const char* topic, const char* payload, unsigned int length);

    private:
    static char *deviceType;
    Mqtt *mqtt;
    boolean initialised = false;
};

#endif