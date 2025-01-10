/*
 * SPI.c
 *
 * Created: 2025-01-10 16:01:05
 *  Author: Saulius
 */ 
#include "Settings.h"

void SPI0_init(){

	SPI0.CTRLA = /*SPI_CLK2X_bm	//Double speed
			   |*/ SPI_MASTER_bm	//Run as Master
			   | SPI_PRESC_DIV4_gc //Speed = 24Mhz/4*2 = 12Mhz
			   | SPI_ENABLE_bm; //Enable spi

	SPI0.CTRLB =  SPI_MODE_1_gc; //SPI mode 1 for TLE9201SG

	//SPI0_Stop();
}


void SPI0_Start(){
 PORTA.OUTCLR = PIN7_bm; //Pull SS low
}

void SPI0_Stop(){
	PORTA.OUTSET = PIN7_bm; //Pull SS high
}

uint8_t SPI0_Read(uint8_t send){//universal function for spi
	
	SPI0.DATA = send | 0x80; //sending data
	while (!(SPI0.INTFLAGS & SPI_IF_bm));  // waiting until all data will be exchanged
	return SPI0.DATA;
}

void SPI0_Write (uint8_t add, uint8_t value){//universal function for spi
	
	SPI0.DATA = add & 0x7f; //writing to register address
	while (!(SPI0.INTFLAGS & SPI_IF_bm));  // waiting until all data will be exchanged
	SPI0.DATA = value;
	while (!(SPI0.INTFLAGS & SPI_IF_bm));
}


uint16_t SPI0_Buffer_Read(uint16_t send){//this function is faster than SPI0_MAX6675_Read() betwean 8bytes data is only 0.1uS delay (8byt 0.14us pause 8byt)
	uint16_t data = 0;
		PORTA.OUTCLR = PIN7_bm; //Pull SS low
	for (uint8_t i = 0; i<2; i++){ //send 16b

		SPI0.DATA = send; //send dummy data
		while(!(SPI0.INTFLAGS & SPI_DREIF_bm));
	}
	for (uint8_t i = 0; i<2; i++){ // read 16b
		SPI0.INTFLAGS = SPI_IF_bm;//clear buffer before reading
		if(i)
		data += SPI0.DATA;
		else
		data = SPI0.DATA<<8;
		SPI0.INTFLAGS = SPI_TXCIF_bm;//clear all transsmmit
	}
		PORTA.OUTSET = PIN7_bm; //Pull SS high
	//_delay_ms(170);//conversion time according MAX6675 (0.17-0.22 s)
	return data;
}

uint8_t SPI_0_exchange_byte(uint8_t data)
{
	// Blocking wait for SPI free makes the function work
	// seamlessly also with IRQ drivers.
	while (SPI0.INTFLAGS & SPI_IF_bm)
	;
	SPI0.DATA = data;
	while (!(SPI0.INTFLAGS & SPI_RXCIF_bm))
	;
	return SPI0.DATA;
}

void SPI_0_write_block(uint8_t block, uint8_t size)
{
	uint8_t b = block;
	while (size--) {
		SPI0.DATA = b;
		while (!(SPI0.INTFLAGS & SPI_RXCIF_bm))
		;
		b++;
	}
}

void SPI_0_read_block(uint8_t block, uint8_t size)
{
	uint8_t b = block;
	while (size--) {
		SPI0.DATA = 0;
		while (!(SPI0.INTFLAGS & SPI_RXCIF_bm))
		;
		b = SPI0.DATA;
		b++;
	}
}