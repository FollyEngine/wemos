#include "secrets.h"
#include <mqtt.h>
#include <neopixel.h>

//
//breif:
//change all pixels to a specified colour
//length of transition from one colour to another
//and make it twinkle for a length of time (using the specified colour as the background

// Listen to mqtt messages and change LEDs in response.  Test with a message like
// mosquitto_pub -h "mqtt" -t "all/neopixel/twinkle" -m '{"operation": "set"}'
// mosquitto_pub -h "mqtt" -t "all/neopixel/twinkle" -m '{"operation": "twinkle", "duration": 1000}'

// GO READ https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/

//BUILD with "LOLIN(WEMOS) D1 R2 & mini"
Mqtt mqtt = Mqtt(SECRET_SSID, SECRET_PASSWORD, "192.168.43.160", 1883, "multipass");

Device *devices[5];
int deviceCount = 1;

void mqtt_callback_fn(const char* topic, const byte* raw_payload, unsigned int length) {
  char payload[256];
  strncpy(payload, (char *) raw_payload, length);
  payload[length] = 0;
  Serial.printf("Callback: %s: %s\n", topic, payload);

  // THIS NEEDS TO FIGURE OUT THE RIGHT DEVICE TO call
  // AND send to multiple for "all"
  devices[0]->mqtt_callback_fn(topic, payload, length);
}


void setup() {
  //while (!Serial);
  Serial.begin(115200);
  Serial.printf("Started\n");


// D4 is the default pin for the 6 LED RBG shield
// https://wiki.wemos.cc/products:d1_mini_shields:rgb_led_shield
// https://github.com/wemos/D1_mini_Examples/blob/master/examples/04.Shields/RGB_LED_Shield/simple/simple.ino
// #define LEDPIN   D4
// #define LED_NUM 7
  devices[0] = new NeopixelString(D4, 7, &mqtt);

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
//#define LEDPIN   D5
//#define LED_NUM 50
//  devices[1] = new NeopixelString(D5, 50, &mqtt);


  for (int i = 0; i < deviceCount; i++) {
    devices[i]->setup();
  }
  mqtt.setCallback(mqtt_callback_fn);
  
  mqtt.setup();

}

void loop() {
  // StaticJsonBuffer<200> jsonBuffer;
  // JsonObject& root = jsonBuffer.createObject();
  mqtt.loop();

  for (int i = 0; i < deviceCount; i++) {
    devices[i]->loop();
  }
}


