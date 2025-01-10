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


uint32_t CLOCK_read(){
    uint32_t base_freq = 4000000; ///< Default F_CPU. Adjust if using EXCLK or PLL clock source is EXCLK as well.

    // Determine the base clock frequency based on OSCHFCTRLA settings
    switch (CLKCTRL.OSCHFCTRLA & CLKCTRL_FRQSEL_gm) {
	    case CLKCTRL_FRQSEL_1M_gc: base_freq = 1000000; break;
	    case CLKCTRL_FRQSEL_2M_gc: base_freq = 2000000; break;
	    case CLKCTRL_FRQSEL_3M_gc: base_freq = 3000000; break;
	    case CLKCTRL_FRQSEL_8M_gc: base_freq = 8000000; break;
	    case CLKCTRL_FRQSEL_12M_gc: base_freq = 12000000; break;
	    case CLKCTRL_FRQSEL_16M_gc: base_freq = 16000000; break;
	    case CLKCTRL_FRQSEL_20M_gc: base_freq = 20000000; break;
	    case CLKCTRL_FRQSEL_24M_gc: base_freq = 24000000; break;
    }

    // Adjust base frequency for peripheral clock prescaler
    if ((TCD0.CTRLA & TCD_CLKSEL_gm) == TCD_CLKSEL_CLKPER_gc) {
	    if (CLKCTRL.MCLKCTRLB & CLKCTRL_PEN_bm) {
		    switch (CLKCTRL.MCLKCTRLB & CLKCTRL_PDIV_gm) {
			    case CLKCTRL_PDIV_2X_gc:  base_freq /= 2; break;
			    case CLKCTRL_PDIV_4X_gc:  base_freq /= 4; break;
			    case CLKCTRL_PDIV_6X_gc:  base_freq /= 6; break;
			    case CLKCTRL_PDIV_8X_gc:  base_freq /= 8; break;
			    case CLKCTRL_PDIV_10X_gc: base_freq /= 10; break;
			    case CLKCTRL_PDIV_12X_gc: base_freq /= 12; break;
			    case CLKCTRL_PDIV_16X_gc: base_freq /= 16; break;
			    case CLKCTRL_PDIV_24X_gc: base_freq /= 24; break;
			    case CLKCTRL_PDIV_32X_gc: base_freq /= 32; break;
			    case CLKCTRL_PDIV_48X_gc: base_freq /= 48; break;
			    case CLKCTRL_PDIV_64X_gc: base_freq /= 64; break;
		    }
	    }
    }

    // Adjust base frequency for PLL
    if ((TCD0.CTRLA & TCD_CLKSEL_gm) == TCD_CLKSEL_PLL_gc) {
	    if ((CLKCTRL.PLLCTRLA & CLKCTRL_PLLCTRLA) == CLKCTRL_MULFAC_2x_gc) {
		    base_freq *= 2;
		    } else if ((CLKCTRL.PLLCTRLA & CLKCTRL_PLLCTRLA) == CLKCTRL_MULFAC_3x_gc) {
		    base_freq *= 3;
	    }

	    if (base_freq > 48000000) {
		    base_freq = 48000000; ///< Cap at 48 MHz (maximum PLL frequency)
	    }
    }
	return base_freq;
};