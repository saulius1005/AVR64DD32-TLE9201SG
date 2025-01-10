/*
 * TLE9201SGVar.h
 *
 * Created: 2025-01-10 15:59:23
 *  Author: Saulius
 */ 


#ifndef TLE9201SGVAR_H_
#define TLE9201SGVAR_H_

TLE9201SG_DATA TLE9201SG ={
	.revision = 0x00, //default  reset values
	.diag = 0x00,
	.Fault = 0x00,
	.OLDIS = 0,
	.SIN = 0,
	.SEN = 0,
	.SDIR = 0,
	.SPWM = 0,
	.mode = 0 //default work mode PWM/DIR
};

#endif /* TLE9201SGVAR_H_ */