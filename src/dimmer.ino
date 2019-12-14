
#include "Dimmer.h"
#include "E131.h"

const int PIN_ZC_IN   = D5;
const int PIN_PWM_OUT = D6;

E131 e131;

void setup()
{
  Particle.publish("ip", WiFi.localIP().toString(), PRIVATE);
  ZCDimmer::getInstance()->begin(PIN_ZC_IN, PIN_PWM_OUT);
  e131.begin();
}


void loop()
{
  e131.parsePacket();
  ZCDimmer::getInstance()->setBrightness(e131.data[3]);
}
