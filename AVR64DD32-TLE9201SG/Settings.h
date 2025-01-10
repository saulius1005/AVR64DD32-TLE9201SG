/*
 * Settings.h
 *
 * Created: 2025-01-10 15:52:20
 *  Author: Saulius
 */ 


#ifndef SETTINGS_H_
#define SETTINGS_H_

/** @brief Defines the default CPU frequency (24 MHz). */
#define F_CPU 24000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/cpufunc.h>
#include "TLE9201SG.h"

void CLOCK_XOSCHF_crystal_init();
void CLOCK_XOSCHF_clock_init();
void CLOCK_INHF_clock_init();
void PLL_init();
void TCD0_init();
void TCD0_ON();
void TCD0_OFF();
void PWM_init(uint32_t target_freq, float duty_cycle);

void GPIO_init(); //Pins initilization

void SPI0_init();
void SPI0_Start();
void SPI0_Stop();
uint8_t SPI0_Read(uint8_t send);
void SPI0_Write (uint8_t add, uint8_t value);
uint16_t SPI0_Buffer_Read(uint16_t send);
uint8_t SPI_0_exchange_byte(uint8_t data);
void SPI_0_write_block(uint8_t block, uint8_t size);
void SPI_0_read_block(uint8_t block, uint8_t size);

uint8_t TLE9201SG_Read(uint8_t command, uint8_t write);
void TLE9201SG_Revision();
void TLE9201SG_Diagnosis();
void TLE9201SG_Read_Control();
void TLE9201SG_Write(uint8_t command);
void TLE9201SG_Mode_init(uint8_t mode);
void TLE9201SG_ON();
void TLE9201SG_OFF();
void TLE9201SG_DIR(uint8_t direction);
void TLE9201SG_START();
void TLE9201SG_STOP();

#endif /* SETTINGS_H_ */