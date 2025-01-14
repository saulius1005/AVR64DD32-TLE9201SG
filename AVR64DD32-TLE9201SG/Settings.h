/**
 * @file Settings.h
 * @brief Header file for system and peripheral initialization for AVR64DD32 and TLE9201SG control.
 * @author Saulius
 * @date 2025-01-10
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

/** @brief Defines the default CPU frequency (24 MHz). */
#define F_CPU 24000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/cpufunc.h>
#include "TLE9201SG.h"

/** @brief Initializes the crystal oscillator in high-frequency mode. */
void CLOCK_XOSCHF_crystal_init();

/** @brief Switches the clock source to the external high-frequency crystal. */
void CLOCK_XOSCHF_clock_init();

/** @brief Initializes the internal high-frequency clock. */
void CLOCK_INHF_clock_init();

/** @brief Configures the Phase-Locked Loop (PLL). */
void PLL_init();

/** 
 * @brief Reads the current clock configuration.
 * @return The current clock frequency in Hz.
 */
uint32_t CLOCK_read();

/** @brief Initializes Timer/Counter D0 (TCD0) for PWM generation. */
void TCD0_init();

/** @brief Enables Timer/Counter D0. */
void TCD0_ON();

/** @brief Disables Timer/Counter D0. */
void TCD0_OFF();

/**
 * @brief Configures PWM with a specified frequency and duty cycle.
 * @param target_freq Desired PWM frequency in Hz.
 * @param duty_cycle Desired duty cycle as a percentage (0.0 to 100.0).
 */
void PWM_init(uint32_t target_freq, float duty_cycle);

/** @brief Initializes GPIO pins. */
void GPIO_init();

/** @brief Initializes the SPI0 interface. */
void SPI0_init();

/** @brief Starts the SPI0 communication. */
void SPI0_Start();

/** @brief Stops the SPI0 communication. */
void SPI0_Stop();

/**
 * @brief Reads data from the TLE9201SG.
 * @param command Command byte to send.
 * @param write Data byte to write.
 * @return Response byte from the TLE9201SG.
 */
uint8_t TLE9201SG_Read(uint8_t command, uint8_t write);

/** @brief Reads and displays the TLE9201SG revision information. */
void TLE9201SG_Revision();

/** @brief Sorts and processes the diagnostic data from the TLE9201SG. */
void TLE9201SG_Sort_Diagnosis();

/** @brief Performs control-related actions for the TLE9201SG. */
void TLE9201SG_Sort_Control_();

/**
 * @brief Writes a command to the TLE9201SG.
 * @param command Command byte to send.
 */
void TLE9201SG_Write(uint8_t command);

/**
 * @brief Initializes the TLE9201SG with the specified mode.
 * @param mode Mode to initialize (e.g., SPI mode).
 */
void TLE9201SG_Mode_init(uint8_t mode);

/** @brief Turns on the TLE9201SG. */
void TLE9201SG_ON();

/** @brief Turns off the TLE9201SG. */
void TLE9201SG_OFF();

/**
 * @brief Sets the direction of the TLE9201SG.
 * @param direction 1 for forward, 0 for reverse.
 */
void TLE9201SG_DIR(uint8_t direction);

/** @brief Starts the TLE9201SG operation. */
void TLE9201SG_START();

/** @brief Stops the TLE9201SG operation. */
void TLE9201SG_STOP();

#endif /* SETTINGS_H_ */
