

#include "motor.h"
#include <ArduinoJson.h>

#include <Wire.h>
#include <LOLIN_I2C_MOTOR.h>

LOLIN_I2C_MOTOR motor; //I2C address 0x30
// LOLIN_I2C_MOTOR motor(DEFAULT_I2C_MOTOR_ADDRESS); //I2C address 0x30
// LOLIN_I2C_MOTOR motor(your_address); //using customize I2C address

char *MotorDevice::deviceType = "motor";

// TODO: set brightness and colour set from mqtt payload
//#define BRIGHTNESS 140   // 140 is reasonably bright

MotorDevice::MotorDevice(Mqtt *m) {
    mqtt = m;
}

void MotorDevice::setup() {
  //Serial.begin(115200);
  Serial.println("Motor Shield Testing...");

  while (motor.PRODUCT_ID != PRODUCT_ID_I2C_MOTOR) //wait motor shield ready.
  {
    motor.getInfo();
  }
  motor.changeDuty(MOTOR_CH_A, 0);
  motor.changeDuty(MOTOR_CH_B, 0);
  motor.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_STANDBY);

  subscribe();
}

void MotorDevice::subscribe() {
    initialised = mqtt->subscribe(mqtt->getHostname(), deviceType, "speed");
    initialised = mqtt->subscribe("all", deviceType, "speed");
    Serial.printf("%s loop Subscription returned: %s\r\n", MotorDevice::deviceType, initialised ? "true" : "false");
}

const char *MotorDevice::IsMessageForMe(const char * topic) {
  size_t length = strlen(MotorDevice::deviceType);
  if (strncmp(MotorDevice::deviceType, topic, length) == 0) {
    return topic+length+1;
  }
  return NULL;
}

void MotorDevice::loop() {
  if (!initialised) {
    delay(1);
    setup();
  }

}

void MotorDevice::mqtt_callback_fn(const char* topic, const char* payload, unsigned int length) {
  // 10 elements in the json payload?
//   DynamicJsonBuffer jb(JSON_OBJECT_SIZE(10));
//   JsonObject& obj = jb.parseObject(payload);
  StaticJsonDocument<300> obj;
  DeserializationError error = deserializeJson(obj, payload);
  if (error) {
    Serial.printf("Failed to parse JSON: %s\n", error.c_str());
    return;
  }

  float motorbDutyCycle = obj["motorb"] | 0;
  Serial.printf("Motor speed: %f\r\n", motorbDutyCycle);
  boolean clockwise = true;
  if (motorbDutyCycle < 0) {
      motorbDutyCycle = -motorbDutyCycle;
      clockwise = false;
  }
  if (motorbDutyCycle == 0) {
    motor.changeStatus(MOTOR_CH_B, MOTOR_STATUS_STOP);
  } else {
    if (clockwise) {
      motor.changeStatus(MOTOR_CH_B, MOTOR_STATUS_CW);
    } else {
      motor.changeStatus(MOTOR_CH_B, MOTOR_STATUS_CCW);
    }
    motor.changeDuty(MOTOR_CH_B, motorbDutyCycle);
  }
}

void testloop() {
  Serial.println("Change A to CCW, B to CW, Freq: 1000Hz");
  Serial.println("Duty Testing...");

  motor.changeFreq(MOTOR_CH_BOTH, 1000); //Change A & B 's Frequency to 1000Hz.
  /*
      motor.changeFreq(MOTOR_CH_A, 1000);//Change A 's Frequency to 1000Hz.
      motor.changeFreq(MOTOR_CH_B, 2000);//Change B 's Frequency to 2000Hz.
  */
  motor.changeStatus(MOTOR_CH_A, MOTOR_STATUS_CCW);
  motor.changeStatus(MOTOR_CH_B, MOTOR_STATUS_CW);

  for (float duty = 0; duty <= 100; duty++)
  {
    motor.changeDuty(MOTOR_CH_A, duty);
    motor.changeDuty(MOTOR_CH_B, 100 - duty);

    Serial.print("Change A Duty to ");
    Serial.print(duty);
    Serial.println("%");

    Serial.print("Change B Duty to ");
    Serial.print(100 - duty);
    Serial.println("%");

    delay(100);
  }

  Serial.println("STANDBY Tesing...");
  motor.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CCW);
  motor.changeDuty(MOTOR_CH_BOTH, 100);
  for (int i = 0; i < 5; i++)
  {
    Serial.println("MOTOR_STATUS_STANDBY");
    motor.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_STANDBY);
    delay(500);
    Serial.println("MOTOR_STATUS_CW");
    motor.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CW);
    delay(500);
    Serial.println("MOTOR_STATUS_STANDBY");
    motor.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_STANDBY);
    delay(500);
    Serial.println("MOTOR_STATUS_CCW");
    motor.changeStatus(MOTOR_CH_BOTH, MOTOR_STATUS_CCW);
    delay(500);
  }

  Serial.println("MOTOR_STATUS Tesing...");
  for (int i = 0; i < 5; i++)
  {
    Serial.println("MOTOR_STATUS_STOP");
    motor.changeStatus(MOTOR_CH_A, MOTOR_STATUS_STOP);
    delay(500);
    Serial.println("MOTOR_STATUS_CCW");
    motor.changeStatus(MOTOR_CH_A, MOTOR_STATUS_CCW);
    delay(500);
    Serial.println("MOTOR_SHORT_BRAKE");
    motor.changeStatus(MOTOR_CH_A, MOTOR_STATUS_SHORT_BRAKE);
    delay(500);
    Serial.println("MOTOR_STATUS_CW");
    motor.changeStatus(MOTOR_CH_A, MOTOR_STATUS_CW);
    delay(500);
  }
}
