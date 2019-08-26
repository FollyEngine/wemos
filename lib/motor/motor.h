#ifndef FollyEngine_Motor_h
#define FollyEngine_Motor_h

#include <device.h>
#include <mqtt.h>

class MotorDevice: public Device {
    public:
    MotorDevice(Mqtt *mqtt);

    virtual void setup();
    virtual void loop();
    virtual void mqtt_callback_fn(const char* topic, const char* payload, unsigned int length);

    private:
    Mqtt *mqtt;
    boolean initialised = false;
};

#endif