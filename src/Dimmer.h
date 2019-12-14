#ifndef ZCDimmer_h
#define ZCDimmer_h

// This will load the definition for common Particle variable types
#include "Particle.h"

// Depends on library for precise timing
#include "SparkIntervalTimer.h"

class ZCDimmer
{
	public:

		/**
		 * Static access method
		 * @return Instance of ZCDimmer
		 */
        static ZCDimmer* getInstance();

		/**
		 * Must be called in setup()
		 */
		void begin(int PIN_ZC_IN, int PIN_PWM_OUT);

		/**
		 * Change the brightness
		 * @param  value 0=off 255=brightest
		 */
		void setBrightness(int value);


	private:

		/**
		* Constructor
		*/
		ZCDimmer();


		static void timer_dim();
		static void isr_on_zero_cross();

		int PIN_ZC_IN;
		int PIN_PWM_OUT;
		IntervalTimer timer;

		// Boolean to store a "switch" to tell us if we have crossed zero
		volatile bool isOff = false;

		// Variable to use as a counter volatile as it is in an interrupt
		volatile int counter = 0;

		int DIM_MIN = 60;
		int DIM_MAX = 117;
		// Dimming level (0-128)  0 = on, 128 = 0ff
		int dim = DIM_MIN;
};



// /**
//  * Called from the timer.
//  * Treat as an ISR, no long running things.
//  * If the requested delay (dim) has elapsed turn on the output, otherwise wait
//  */
// inline void timer_dim1()
// {
// 	dimmer.timer_dim();
// }

// /**
//  * Attatched as an interrupt when a zero cross is detected.
//  */
// inline void isr_on_zero_cross1()
// {
// 	dimmer.isr_on_zero_cross();
// }

#endif //ZCDimmer_h
