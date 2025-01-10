/*
 * AVR64DD32-TLE9201SG.c
 *
 * Created: 2025-01-10 15:50:01
 * Author : Saulius
 */ 

#include "Settings.h"

int main(void)
{
	GPIO_init();
    CLOCK_INHF_clock_init(); ///< Initialize the internal high-frequency clock
	TLE9201SG.pwm_freq = 10000; //20kHz //allways before mode init
	TLE9201SG.duty_cycle = 10.0; //10% duty cycle //allways before mode init
	TLE9201SG_Mode_init(TLE9201SG_MODE_PWMDIR);
	TLE9201SG_OFF(); // disable all outputs

    while (1) {
		if(!(PORTF.IN & PIN5_bm)){ //start TLE9201SG
			TLE9201SG_ON();
			TLE9201SG_START();
			if(!(PORTF.IN & PIN6_bm))
				TLE9201SG_DIR(1);
			else
				TLE9201SG_DIR(0);
		}
		else { //stop TLE9201SG
			TLE9201SG_STOP();
			TLE9201SG_OFF();
		}

    }
}

