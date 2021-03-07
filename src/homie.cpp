#include <SPI.h>
#include <Wire.h>
#include <Homie.h>

#include "ButtonNode.hpp"
ButtonNode button("button", "test", D3);

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
