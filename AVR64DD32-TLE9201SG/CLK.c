/**
 * @file CLK.c
 * @brief Clock initialization functions for AVR64DD32 microcontroller.
 * 
 * @details This file contains functions to configure the system clock using 
 *          internal, external, or crystal oscillators, as well as the PLL. 
 *          The configuration options include frequency, prescaler, and clock output.
 * 
 * @author Saulius
 * @date 2025-01-09
 */

#include "Settings.h"

/**
 * @brief Initializes the high-frequency crystal oscillator (XOSCHF).
 * 
 * @details Configures the crystal oscillator for a frequency range of 32 MHz with a 1K cycle start-up time.
 *          Sets the crystal as the clock source and optionally enables clock output on pin PA7.
 * 
 * @note Ensure the connected crystal matches the specified frequency range.
 */
void CLOCK_XOSCHF_crystal_init() {
    /* Enable crystal oscillator with frequency range 32 MHz and 1K cycles start-up time */
    ccp_write_io((uint8_t *) &CLKCTRL.XOSCHFCTRLA, CLKCTRL_RUNSTDBY_bm
        | CLKCTRL_CSUTHF_1K_gc   // Start-up time: 1K cycles
        | CLKCTRL_FRQRANGE_32M_gc // Frequency range: 16 MHz
        | CLKCTRL_SELHF_XTAL_gc  // Use crystal oscillator
        | CLKCTRL_ENABLE_bm);

    /* Wait for crystal oscillator to stabilize */
    while (!(CLKCTRL.MCLKSTATUS & CLKCTRL_EXTS_bm)) {};

    /* Clear main clock prescaler */
    ccp_write_io((uint8_t *) &CLKCTRL.MCLKCTRLB, 0x00);

    /* Set main clock to use XOSCHF as the source */
    ccp_write_io((uint8_t *) &CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_EXTCLK_gc /*| CLKCTRL_CLKOUT_bm*/); 
    // Comment | CLKCTRL_CLKOUT_bm if clock output on PA7 is not required

    /* Wait for oscillator change to complete */
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm) {};

    /* Configuration complete; main clock is now running at 16 MHz */
}

/**
 * @brief Initializes the external high-frequency clock (XOSCHF).
 * 
 * @details Configures an external clock source with a frequency of 32 MHz. 
 *          Optionally enables clock output on pin PA7 and sets a prescaler of 2.
 * 
 * @note Ensure the external clock source provides the correct frequency.
 */
void CLOCK_XOSCHF_clock_init() {
    /* Enable external clock input (32 MHz) */
    ccp_write_io((uint8_t *) &CLKCTRL.XOSCHFCTRLA, CLKCTRL_SELHF_EXTCLOCK_gc 
        | CLKCTRL_FRQRANGE_32M_gc 
        | CLKCTRL_ENABLE_bm);

    /* Set main clock prescaler */
    ccp_write_io((uint8_t *) &CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm);

    /* Set main clock to use external clock as the source */
    ccp_write_io((uint8_t *) &CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_EXTCLK_gc /*| CLKCTRL_CLKOUT_bm*/); 
    // Comment | CLKCTRL_CLKOUT_bm if clock output on PA7 is not required

    /* Wait for oscillator change to complete */
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm) {};

    /* Configuration complete; main clock is now running at 32 MHz / 2 = 16 MHz */
}

/**
 * @brief Initializes the internal high-frequency oscillator (OSCHF).
 * 
 * @details Configures the internal oscillator with a frequency of 24 MHz. 
 *          Optionally enables clock output on pin PA7. A prescaler can be configured if needed.
 */
void CLOCK_INHF_clock_init() {
    /* Enable internal oscillator with a frequency of 24 MHz */
    ccp_write_io((uint8_t *) &CLKCTRL.OSCHFCTRLA, CLKCTRL_FRQSEL_24M_gc); 

    /* Set main clock prescaler (uncomment if required) */
    // ccp_write_io((uint8_t *) &CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm);

    /* Set main clock to use the internal oscillator as the source */
    ccp_write_io((uint8_t *) &CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSCHF_gc /*| CLKCTRL_CLKOUT_bm*/); 
    // Comment | CLKCTRL_CLKOUT_bm if clock output on PA7 is not required

    /* Wait for oscillator change to complete */
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm) {};
}

/**
 * @brief Initializes the PLL (Phase-Locked Loop) for frequency multiplication.
 * 
 * @details Configures the PLL to multiply the input frequency by a factor of 2. 
 *          The maximum output frequency is limited to 48 MHz.
 * 
 * @note Ensure the input frequency does not exceed the PLL's maximum limit.
 */
void PLL_init() {
    /* Configure PLL with a multiplication factor of 2 */
    ccp_write_io((uint8_t *) &CLKCTRL.PLLCTRLA, CLKCTRL_MULFAC_2x_gc); 

    /* Wait for PLL configuration to complete */
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_PLLS_bm) {};
}
