#include "secrets.h"

// #ifdef ESP8266
// #include <servo.h>

// #include <neopixel.h>
// #include <motor.h>
// #include <relay.h>
// #include <switch.h>

// #else
// //esp32 only
// #include <touch.h>
// #endif

#include <SPI.h>
#include <Wire.h>
#include <Homie.h>

#include "ButtonNode.hpp"

ButtonNode buttonNode("button", "test", D3);

//
//breif:
//change all pixels to a specified colour
//length of transition from one colour to another
//and make it twinkle for a length of time (using the specified colour as the background

// Listen to mqtt messages and change LEDs in response.  Test with a message like
// mosquitto_pub -h "mqtt" -t "all/neopixel/twinkle" -m '{"operation": "set"}'
// mosquitto_pub -h "mqtt" -t "all/neopixel/twinkle" -m '{"operation": "twinkle", "duration": 1000}'

// GO READ https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/

// #define FASTLED_ESP8266_RAW_PIN_ORDER
// #define FASTLED_ALLOW_INTERRUPTS 0
// #include <FastLED.h>
#define NUM_LEDS 50
//CRGB leds[NUM_LEDS];

#define FW_NAME "folly-engine-nodes"
#define FW_VERSION "0.0.1"

void setup()
{
  // https://homieiot.github.io/homie-esp8266/docs/3.0.1/configuration/json-configuration-file/
  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie_setBrand("Folly");

  //while (!Serial);
  Serial.begin(9600); //115200
  int startDelay = random(100) * 100;
  Serial.printf("Started, delaying %d mS\r\n", startDelay);
  delay(startDelay);

#ifdef ESP8266
  // D4 is the default pin for the 6 LED RBG shield
  // https://wiki.wemos.cc/products:d1_mini_shields:rgb_led_shield
  // https://github.com/wemos/D1_mini_Examples/blob/master/examples/04.Shields/RGB_LED_Shield/simple/simple.ino
  // #define LEDPIN   D4
  // #define LED_NUM 7
  // devices[deviceCount++] = new NeopixelString(D4, 7, NEO_GRB + NEO_KHZ800, &mqtt); // GRB
  //FastLED.addLeds<WS2812, D4, GRB>(leds, 7);
  //devices[deviceCount++] = new NeopixelString(leds, 7, &mqtt); // RGB

  // When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
  // Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
  // example for more information on possible values.
  // #define LEDPIN   D5
  // FastLED.addLeds<WS2812, D5, RGB>(leds, NUM_LEDS);
  //CRGBArray<LED_NUM> left_leds;
  //FastLED.addLeds<NEOPIXEL, LEDPIN>(left_leds, LED_NUM);
  //left_leds = CRGB::HotPink;
  //devices[deviceCount++] = new NeopixelString(leds, NUM_LEDS, &mqtt); // RGB
  //devices[deviceCount++] = new NeopixelString(D5, 50, NEO_RGB + NEO_KHZ800, &mqtt); // RGB

  // pins 19&20 / D1&D2 / GPIO4 and GPOI5
  // uses I2C, defaule address 0x30
  // Don;t enable unless there is a motor board - it tries to find it, so we have to wait for an i2c timeout
  //devices[deviceCount++] = new MotorDevice(&mqtt);

  // use D3 and D6
  //devices[deviceCount++] = new ServoDevice(&mqtt);
  // use D3
  // devices[deviceCount++] = new SwitchDevice(&mqtt);
  // use D1
  //devices[deviceCount++] = new RelayDevice(&mqtt);
#else
  devices[deviceCount++] = new TouchDevice(&mqtt); //<<<<<<<<<------ for the esp32
#endif

  //buttonNode.beforeHomieSetup();

  Homie.disableLedFeedback();
  Homie.disableResetTrigger();

  Homie.setup();

  // TODO: need to redo the callback thing so there's one callback per subscription that then gets the pre-parsed json
  Serial.printf("started\r\n");
}

void loop()
{
  Homie.loop();
}
