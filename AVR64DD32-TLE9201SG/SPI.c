/*
 * SPI.c
 *
 * Created: 2025-01-10 16:01:05
 *  Author: Saulius
 */ 
#include "Settings.h"

void SPI0_init(){

	SPI0.CTRLA = SPI_CLK2X_bm	//Double speed
			   | SPI_MASTER_bm	//Run as Master
			   | SPI_PRESC_DIV4_gc //Speed = 24Mhz/4*2 = 12Mhz
			   | SPI_ENABLE_bm; //Enable spi

	SPI0.CTRLB =  SPI_MODE_1_gc; //SPI mode 1 for TLE9201SG
}

void SPI0_Start(){
 PORTA.OUTCLR = PIN7_bm; //Pull SS low
}

void SPI0_Stop(){
	PORTA.OUTSET = PIN7_bm; //Pull SS high
}

