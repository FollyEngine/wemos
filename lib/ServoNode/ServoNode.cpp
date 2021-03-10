#include "ServoNode.hpp"

#include <Wire.h>

ServoNode::ServoNode(const char *id, const char *name, int8_t pin)
    : HomieNode(id, name, "Servo"),
      servoPin(pin)
{
    advertise("angle").setDatatype("integer").setFormat("0:180").setName("Servo Angle").settable();
}

bool ServoNode::handleInput(const HomieRange &range, const String &property, const String &value)
{
    Homie.getLogger() << "Message(" << getName() << "): " << property << " " << value << endl;
    if (property.equals("angle"))
    {
        servo.write(value.toInt());
        return true;
    }
    return false;
}

void ServoNode::beforeHomieSetup()
{
    //   _maxTimeout->setDefaultValue(600).setValidator([](long candidate) {
    //     return (candidate >= 0);
    //   });
}

void ServoNode::onReadyToOperate(){
    //setRelay(false, 0);
};

void ServoNode::setup()
{
    Homie.getLogger() << "ServoNode::setup()" << endl;
    servo.attach(servoPin);
}