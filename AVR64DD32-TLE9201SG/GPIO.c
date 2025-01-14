/*
 * @file GPIO.c
 * @brief GPIO initialization functions for AVR64DD32 microcontroller.
 *
 * @details Configures GPIO pins for SPI communication, PWM control, and input handling 
 *          with pull-up resistors. Initializes directions and states for each pin.
 * 
 * @author Saulius
 * @date 2025-01-10
 */

#include "Settings.h"

/**
 * @brief Initializes GPIO pins for various functionalities.
 * 
 * @details 
 * - Configures PORTA for SPI communication: MOSI, SCK, SS as output; MISO as input.
 * - Stops the SPI0 module.
 * - Configures PORTD for motor control: PWM, DIR, DIS as output.
 * - Configures PORTF for input buttons with pull-up resistors: START/STOP, DIR.
 */
void GPIO_init() {
    /* Configure SPI pins on PORTA */
    PORTA.DIRSET = PIN4_bm | PIN6_bm | PIN7_bm; // Set MOSI (PA4), SCK (PA6), SS (PA7) as outputs
    PORTA.DIRCLR = PIN5_bm;                     // Set MISO (PA5) as input
    SPI0_Stop();                                // Ensure SPI0 module is stopped

    /* Configure motor control pins on PORTD */
    PORTD.DIRSET = PIN4_bm | PIN5_bm | PIN6_bm; // Set PWM (PD4), DIR (PD5), DIS (PD6) as outputs

    /* Configure input buttons on PORTF */
    PORTF.DIRCLR = PIN5_bm | PIN6_bm;           // Set START/STOP (PF5), DIR (PF6) as inputs
    PORTF.PIN5CTRL = PORT_PULLUPEN_bm;          // Enable pull-up resistor for START/STOP (PF5)
    PORTF.PIN6CTRL = PORT_PULLUPEN_bm;          // Enable pull-up resistor for DIR (PF6)
}
