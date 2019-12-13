/*
 * Project holiday-dimmer
 * Description:
 * Author: Blake Bourque
 * Date: Dec 2019
 */

#include "SparkIntervalTimer.h"

// This is the delay-per-brightness step in microseconds.
// It is calculated based on the frequency of your voltage supply (50Hz or 60Hz)
// and the number of brightness steps you want.
// Firing angle calculation : 1 full 60Hz wave =1/60=16.7ms
// Every zerocrossing thus: (60Hz)-> 8.3ms (1/2 Cycle) For 60Hz => 8.33ms
// 8.33ms=8333us
// (8333us - 10us) / 128 = 65 (Approx)
int freqStep = 65;

volatile int counter = 0;         // Variable to use as a counter volatile as it is in an interrupt
volatile boolean zero_cross = 0;  // Boolean to store a "switch" to tell us if we have crossed zero
int AC_PIN = D7;                  // Output to Opto Triac
int PIN_ZERO_CROSS = D6;          // Interrrupt pin
// Min/Max depends on your circuit
// This is for my bedside LED lights
int DIM_MIN = 20;
int DIM_MAX = 100;
int dim = DIM_MAX;                // Dimming level (0-128)  0 = on, 128 = 0ff
int inc = 1;                      // counting up or down, 1=up, -1=down

IntervalTimer timer;

void zero_cross_detect() {
	zero_cross = true;         // set the boolean to true to tell our dimming function that a zero cross has occured
	counter = 0;
	if (dim == DIM_MIN) {
		// dont turn off (full bright)
	} else {
		digitalWrite(AC_PIN, LOW); // turn off TRIAC (and AC)
	}
  
}


// Turn on the TRIAC at the appropriate time
void dim_check() {
  if(zero_cross == true) {
	if(counter >= dim) {
		if(dim == DIM_MAX) {
			// don't turn on
		} else {
			digitalWrite(AC_PIN, HIGH); // turn on light
		}
	  counter = 0;                // reset time step counter
	  zero_cross = false;         //reset zero cross detection
	}
	else {
	  counter++; // increment time step counter
	}
  }
}

int change(String data) {
	int tmp = data.toInt();
	// tmp = 100 - tmp;
	if(tmp >= DIM_MAX) {
		return dim = DIM_MAX;
	}
	if(tmp <= DIM_MIN) {
		return dim = DIM_MIN;
	}
	if(tmp == DIM_MIN) {
		//dont turn off
	}
	return dim = tmp;
	
}






void setup()
{
	Particle.function("change", change);
	Particle.publish("ip", WiFi.localIP().toString(), PRIVATE);
	Serial.begin(9600);

	// Relay pins (ignore this)
	pinMode(D0, OUTPUT);
	pinMode(D1, OUTPUT);
	digitalWrite(D0, LOW);
	digitalWrite(D1, LOW);

	// Zero Cross setup
	pinMode(PIN_ZERO_CROSS, INPUT);
	pinMode(AC_PIN, OUTPUT);
	timer.begin(dim_check, freqStep, uSec, TIMER5);
	// Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection
	attachInterrupt(PIN_ZERO_CROSS, zero_cross_detect, RISING);

	Particle.variable("dim",dim);
}


void loop() {
  Serial.println(dim);
//   dim += inc;
//   if((dim >= DIM_MAX) || (dim <= DIM_MIN)) {
//     inc *= -1;
//   }

  delay(250);
}
