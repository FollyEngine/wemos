
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Homie.h>

#ifdef BUTTONBOARD
#include "ButtonNode.hpp" //https://github.com/luebbe/homie-node-collection#buttonnode
// default wemos board: https://www.wemos.cc/en/latest/d1_mini_shield/1_button.html
ButtonNode button("button", "test", D3); // GPIO0
// TODO: Get the battery level (need resistor&Analog pin) - https://hutscape.com/tutorials/measure-battery-voltage
#endif
#include "RelayNode.hpp" // https://github.com/luebbe/homie-node-collection#relaynode
// default wemos board: https://www.wemos.cc/en/latest/d1_mini_shield/relay.html
RelayNode relay("relay", "test", D1); // GPIO5
// Must be paired with:  "settings": { "relay.maxTimeout": 0 }
// mosquitto_pub -h sven-screen1 -t homie/4c11ae0dce0c/relay/on/set -m toggle

//include "WS2812Node.h" // https://github.com/euphi/ESP_Homie_WS2812FX?utm_source=platformio&utm_medium=piohome
#include <WS2812Node.h>
// default wemos board: https://www.wemos.cc/en/latest/d1_mini_shield/rgb_led.html
WS2812Node neopixels("neopixel", "seven", FX_MODE_FIRE_FLICKER_SOFT, NEO_GRB + NEO_KHZ800, D4, 7); // RGB D4, GPIO2
#include <LoggerNode.h>
LoggerNode LN; // looks like the WS2812Node needs this..

// PIR D3, GPIO0

#define FW_NAME "folly-engine-nodes"
#define FW_VERSION "0.0.1"
#define FW_BRAND "Folly"

void setup()
{
  Serial.begin(9600); //115200

  // Avoid all the devices hammering at the same power-on instant
  int startDelay = random(100) * 100;
  Serial.printf("Started, delaying %d mS\r\n", startDelay);
  delay(startDelay);

  // https://homieiot.github.io/homie-esp8266/docs/3.0.1/configuration/json-configuration-file/
  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie_setBrand(FW_BRAND);

  //  Homie.disableLedFeedback();
  Homie.disableResetTrigger();

  Homie.setup();

  // TODO: need to redo the callback thing so there's one callback per subscription that then gets the pre-parsed json
  Serial.printf("started \r\n");
}

void loop()
{
  Homie.loop();
}
