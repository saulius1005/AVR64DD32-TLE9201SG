/*
 * GPIO.c
 *
 * Created: 2025-01-10 16:03:22
 *  Author: Saulius
 */ 
#include "Settings.h"

void GPIO_init(){
	PORTA.DIRSET = PIN4_bm |PIN6_bm | PIN7_bm; // MOSI, SCK, SS
	PORTA.DIRCLR = PIN5_bm; //MISO

	PORTD.DIRSET = PIN4_bm | PIN5_bm | PIN6_bm; //PWM, DIR, DIS

	PORTF.DIRCLR = PIN5_bm | PIN6_bm; // START/STOP, DIR
	PORTF.PIN5CTRL = PORT_PULLUPEN_bm;
	PORTF.PIN6CTRL = PORT_PULLUPEN_bm;
	//SPI0_Stop();
	
}