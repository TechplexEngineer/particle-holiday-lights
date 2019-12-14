
#include "Dimmer.h"

int brightness = 0;
int inc = 10;
const int BRIGHT_MAX = 255;
const int BRIGHT_MIN = 0;

const int PIN_ZC_IN   = D5;
const int PIN_PWM_OUT = D6;


void setup()
{
  ZCDimmer::getInstance()->begin(PIN_ZC_IN, PIN_PWM_OUT);
}


void loop()
{
  brightness += inc;
  if((brightness >= BRIGHT_MAX) || (brightness <= BRIGHT_MIN)) {
    inc *= -1;
  }
  ZCDimmer::getInstance()->setBrightness(brightness);

  delay(100);
}
