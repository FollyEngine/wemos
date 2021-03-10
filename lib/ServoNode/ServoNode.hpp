#pragma once

#include <Homie.hpp>
#include <Servo.h> // esp8266 only

// // use D3 (GPIO0) and
// #define ServoPin1 00
// // D6 (GPIO12)
// #define ServoPin2 12

class ServoNode : public HomieNode
{

protected:
    virtual bool handleInput(const HomieRange &range, const String &property, const String &value) override;
    virtual void onReadyToOperate() override;
    virtual void setup() override;

public:
    // using the Folly 2 servo board - // use D3 (GPIO0) and D6 (GPIO12)
    static const int8_t ServoPin1 = 00;
    static const int8_t ServoPin2 = 12;

    ServoNode(const char *id,
              const char *name,
              int8_t pin = ServoPin1);
    void beforeHomieSetup();

private:
    int8_t servoPin;
    Servo servo; // create servo object to control a servo
};
