/*
 * TLE9201SG.h
 *
 * Created: 2025-01-10 15:57:42
 *  Author: Saulius
 */ 


#ifndef TLE9201SG_H_
#define TLE9201SG_H_

#define TLE9201SG_MODE_SPI 1
#define TLE9201SG_MODE_PWMDIR 0
#define TLE9201SG_SPI_TIME_COMPENSATION 0.5
#define RD_DIA 0b00000000 // Read Diagnosis Register
#define RES_DIA 0b10000000 // Reset Diagnosis Register
#define RD_REV 0b00100000 // Read Device Revision Number
#define RD_CTRL 0b01100000 // Read Control Register
#define WR_CTRL 0b11100000 // Write Control - sets and returns Control Register values
#define WR_CTRL_RD_DIA 0b11000000 // Write Control and Read Diagnosis- sets Control Register values and returns Diagnosis Register values

typedef struct { //creating structure for data storage for each row
	uint8_t revision; //index for moving average
	uint8_t diag;
	uint8_t control;
	uint8_t EN;
	uint8_t OT;
	uint8_t TV;
	uint8_t CL;
	uint8_t DIA;
	uint8_t Fault;
	uint8_t CMD;
	uint8_t OLDIS;
	uint8_t SIN;
	uint8_t SEN;
	uint8_t SDIR;
	uint8_t SPWM;
	uint8_t back;
	uint8_t mode;
	uint16_t pwm_freq;
	float duty_cycle;
	uint16_t on; //pwm on time using _delay_loop2()
	uint16_t off; //pwm off time

} TLE9201SG_DATA;

extern TLE9201SG_DATA TLE9201SG;

#endif /* TLE9201SG_H_ */