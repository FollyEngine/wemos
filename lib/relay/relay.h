#ifndef FollyEngine_Relay_h
#define FollyEngine_Relay_h

#include <device.h>
#include <mqtt.h>

class RelayDevice: public Device {
    public:
    RelayDevice(Mqtt *mqtt);

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