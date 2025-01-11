/*
 * TLE9201SG.c
 *
 * Created: 2025-01-10 15:59:47
 *  Author: Saulius
 */ 
#include "Settings.h"
 #include "TLE9201SGVar.h"

uint8_t TLE9201SG_Read(uint8_t command, uint8_t write){
	static uint8_t firsttime = 0;
	for(uint8_t i = 0; i< 3-firsttime; i++){	//reading  3times at the first time and next time reading twice and using only last answer
		SPI0_Start();	
		if(write){ //if writing to command register
			SPI0.DATA = command+ (TLE9201SG.OLDIS<<4) + (TLE9201SG.SIN<<3) + (TLE9201SG.SEN<<2) + (TLE9201SG.SDIR<<1) + TLE9201SG.SPWM;
		}
		else // othervise just read data
			SPI0.DATA = command;
		while (!(SPI0.INTFLAGS & SPI_RXCIF_bm));
		SPI0_Stop();
	}
	firsttime = 1;
	return SPI0.DATA;;
}

void TLE9201SG_Revision(){
	uint8_t raw = TLE9201SG_Read(RD_REV, 0)/* & 7*/;
	TLE9201SG.revision =  raw;//last 3bits 			
}

void TLE9201SG_Diagnosis(){
	//TLE9201SG.diag = TLE9201SG_Read(RD_DIA, 0); //Read all data
	// Naudojame bitø maskes ir poslinkius, kad iðskirtume reikðmes
	TLE9201SG.EN = (TLE9201SG.diag >> 7) & 0x01; // 7 bitas
	TLE9201SG.OT = (TLE9201SG.diag >> 6) & 0x01; // 6 bitas
	TLE9201SG.TV = (TLE9201SG.diag >> 5) & 0x01; // 5 bitas
	TLE9201SG.CL = (TLE9201SG.diag >> 4) & 0x01; // 4 bitas
	TLE9201SG.DIA = TLE9201SG.diag & 0x0F;       // 3-0 bitai

	if(TLE9201SG.DIA != 0xF) //all ok if 0xF
	TLE9201SG.Fault = TLE9201SG.DIA;

 }

void TLE9201SG_Read_Control(){
	//TLE9201SG.control = TLE9201SG_Read(RD_CTRL, 0); //Read all data
	// Naudojame bitø maskes ir poslinkius, kad iðskirtume reikðmes
	TLE9201SG.CMD = (TLE9201SG.control >> 5); // 7-5 bitai
	TLE9201SG.OLDIS = (TLE9201SG.control >> 4) & 0x01; // 4 bitas
	TLE9201SG.SIN = (TLE9201SG.control >> 3) & 0x01; // 3 bitas
	TLE9201SG.SEN = (TLE9201SG.control >> 2) & 0x01; // 2 bitas
	TLE9201SG.SDIR= (TLE9201SG.control >> 1) & 0x01; // 1 bitas
	TLE9201SG.SPWM= TLE9201SG.control & 0x01; // 0 bitas
}

 uint8_t TLE9201SG_Write_Reg(uint8_t command){
 static uint8_t firstime = 0;
		for(uint8_t i = 0; i< 3-firstime; i++){
		SPI0_Start();
 		SPI0.DATA = command + (TLE9201SG.OLDIS<<4) + (TLE9201SG.SIN<<3) + (TLE9201SG.SEN<<2) + (TLE9201SG.SDIR<<1) + TLE9201SG.SPWM; //writing to register address
 		while (!(SPI0.INTFLAGS & SPI_RXCIF_bm));  // waiting until all data will be exchanged
		SPI0_Stop();		
	}
	firstime = 1;
	return SPI0.DATA;
 }

void TLE9201SG_Write(uint8_t command){
	static uint8_t fakeread = 0,
		   counter = 0;
	SPI0_Start();
		SPI0.DATA = command+ (TLE9201SG.OLDIS<<4) + (TLE9201SG.SIN<<3) + (TLE9201SG.SEN<<2) + (TLE9201SG.SDIR<<1) + TLE9201SG.SPWM;
	while (!(SPI0.INTFLAGS & SPI_RXCIF_bm));
	SPI0_Stop();
	if(counter != 3){
		if(command == WR_CTRL){
			TLE9201SG.control = SPI0.DATA;
			TLE9201SG_Read_Control();
		}
		else{
			TLE9201SG.diag = SPI0.DATA;
			TLE9201SG_Diagnosis();
		}
	}
	else{
		if(command == WR_CTRL){
			fakeread = SPI0.DATA;
			fakeread = 0x00;
			TLE9201SG.control = fakeread; //default
			TLE9201SG_Read_Control();
		}
		else{
			fakeread = SPI0.DATA;
			fakeread = 0xdf;
			TLE9201SG.diag = fakeread; //default
			TLE9201SG_Diagnosis();
		}
		counter++;			
	}
}
		double sig_period = 0.0;
		double sig_calc = 0.0;
		double sig_on = 0.0;


void TLE9201SG_Mode_init(uint8_t mode){ //This function allows selection of controll type 0- DIR/PWM or 1- SPI
	TLE9201SG.mode = mode;
	if(mode){ //SPI

		SPI0_init();
		TLE9201SG.SIN = 1; //enabling control via SPI
		TLE9201SG.OLDIS = 0;
		TLE9201SG.SEN = 0; //disable outputs
		TLE9201SG_Write(WR_CTRL);

		sig_calc = 1.0 / CLOCK_read() * 4; //calculating time base according current main clock value
		sig_period = (1.0 / TLE9201SG.pwm_freq) - TLE9201SG_SPI_TIME_COMPENSATION; //calculating period time for requared frequency
		sig_on = TLE9201SG.duty_cycle/100 * sig_period; //calculating pwm duty cycle
		TLE9201SG.off = ((sig_period - sig_on)/ sig_calc); //calculating pwm off time
		TLE9201SG.on =  (sig_on/sig_calc); //calculating pwm on time

	}
	else{ //PWM DIR
		PLL_init(); ///< Initialize the Phase-Locked Loop (PLL)
		TCD0_init(); ///< Initialize Timer/Counter D (TCD)
		PWM_init(TLE9201SG.pwm_freq, TLE9201SG.duty_cycle);
	}
}

void TLE9201SG_ON(){
	if(TLE9201SG.mode){ //Mode SPI
		TLE9201SG.SEN = 1; //enable outputs
	}
	else{
		PORTD.OUTCLR = PIN6_bm; // 
	}
}

void TLE9201SG_OFF(){ //stop and turn off
	if(TLE9201SG.mode){ //Mode SPI
		TLE9201SG.SEN = 0; //disable outputs
		SPI0_Stop();
	}
	else{
		TCD0_OFF();
		PORTD.OUTSET = PIN6_bm;
	}
}

void TLE9201SG_DIR(uint8_t direction){
	if(TLE9201SG.mode){ //Mode SPI
		TLE9201SG.SDIR = direction;
	}
	else{
		PORTD.OUT = (direction << PIN5_bp);
	}
}

void TLE9201SG_START(){
	if(TLE9201SG.mode){ //Mode SPI

		TLE9201SG.SPWM = 1;
		TLE9201SG_Write(WR_CTRL_RD_DIA);
		//_delay_loop_2(calculate_delay_loop2_value(sig_on)); //50us = 20khz //_delay can be changed with other timer exmpl.: TCA or TCB or RTC or even TCD
		_delay_loop_2(TLE9201SG.on);
		//_delay_loop_2(25);
		TLE9201SG.SPWM = 0;
		TLE9201SG_Write(WR_CTRL_RD_DIA);
		//_delay_loop_2(calculate_delay_loop2_value(sig_off));	
		_delay_loop_2(TLE9201SG.off);
		//_delay_loop_2(475);
	}
	else{
		TCD0_ON();
	}	
}

void TLE9201SG_STOP(){
	if(TLE9201SG.mode){ //Mode SPI
		SPI0_Stop();
	}
	else{
		TCD0_OFF();
	}
}