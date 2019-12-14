#include "Dimmer.h"

// This is the delay-per-brightness step in microseconds.
// It is calculated based on the frequency of your voltage supply (50Hz or 60Hz)
// and the number of brightness steps you want.
// Firing angle calculation : 1 full 60Hz wave =1/60=16.7ms
// Every zerocrossing thus: (60Hz)-> 8.3ms (1/2 Cycle) For 60Hz => 8.33ms
// 8.33ms=8333us
// 128 = brightness steps
// (8333us - 10us) / 128 = 65 (Approx)
const int freqStep = 65;


/**
 * Constructor.
 */
ZCDimmer::ZCDimmer()
{
	// be sure not to call anything that requires hardware be initialized here,
	// put those in begin()
}

ZCDimmer* ZCDimmer::getInstance()
{
    static ZCDimmer instance;

    return &instance;
}

/**
 * Init the dimmer
 */
void ZCDimmer::begin(int PIN_ZC_IN, int PIN_PWM_OUT)
{
	this->PIN_ZC_IN = PIN_ZC_IN;
	this->PIN_PWM_OUT = PIN_PWM_OUT;

	// Zero Cross
	pinMode(this->PIN_ZC_IN, INPUT);
	// TRIAC control
	pinMode(this->PIN_PWM_OUT, OUTPUT);

	// Setup the timer
	timer.begin(ZCDimmer::timer_dim, freqStep, uSec, TIMER5);
	// Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection
	attachInterrupt(this->PIN_ZC_IN, ZCDimmer::isr_on_zero_cross, RISING);
}

/**
 * Change the brightness
 * @param  value 0=off 255=brightest
 */
void ZCDimmer::setBrightness(int value)
{
	// DMX/sACN/E1.31 sends values in the range [0,255]
	// Convert to our known good range
	value = map(value, 0, 255, DIM_MAX, DIM_MIN);
	if(value >= DIM_MAX) {
		this->dim = DIM_MAX;
		return;
	}
	if(value <= DIM_MIN) {
		this->dim = DIM_MIN;
		return;
	}
	this->dim = value;
}

/**
 * Called from the timer.
 * Treat as an ISR, no long running things.
 * If the requested delay (dim) has elapsed turn on the output, otherwise wait
 */
void ZCDimmer::timer_dim()
{
	if(ZCDimmer::getInstance()->isOff == true)
	{
		if(ZCDimmer::getInstance()->counter >= ZCDimmer::getInstance()->dim)
		{
			// turn on output
			digitalWrite(ZCDimmer::getInstance()->PIN_PWM_OUT, HIGH);
			// reset time step counter
			ZCDimmer::getInstance()->counter = 0;
			//reset zero cross detection
			ZCDimmer::getInstance()->isOff = false;
		}
		else
		{
			// increment time step counter
			ZCDimmer::getInstance()->counter++;
		}
	}
}

/**
 * Attatched as an interrupt when a zero cross is detected.
 */
void ZCDimmer::isr_on_zero_cross()
{
	// keep track of when we turn off the output, ie: at a ZC
	ZCDimmer::getInstance()->isOff = true;
	// begin counting the (dim) amount before turning on
	ZCDimmer::getInstance()->counter = 0;

	// turn off output at the zero crossing
	// Which is the only place we can turn off the TRIAC
	digitalWrite(ZCDimmer::getInstance()->PIN_PWM_OUT, LOW);
}

