#ifndef FollyEngine_Device_h
#define FollyEngine_Device_h

class Device {
    public:
    virtual void setup();
    virtual void loop();
    virtual void mqtt_callback_fn(const char* topic, const char* payload, unsigned int length);
};

#endif