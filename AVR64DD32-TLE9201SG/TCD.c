/**
 * @file TCD.c
 * @brief Functions for configuring and controlling Timer/Counter D (TCD) on the AVR64DD32 microcontroller.
 * 
 * @details This file includes functions to initialize TCD, control its on/off state,
 *          and configure it for PWM generation with adjustable frequency and duty cycle.
 * 
 * @author Saulius
 * @date 2025-01-09
 */

#include "Settings.h"

/**
 * @brief Turns on the TCD0 counter.
 * 
 * @details Waits until the TCD is ready to be enabled, then activates the timer.
 */
void TCD0_ON() {
    while (!(TCD0.STATUS & TCD_ENRDY_bm)); ///< Wait until the TCD is ready
    TCD0.CTRLA |= TCD_ENABLE_bm; ///< Enable the TCD0 counter
}

/**
 * @brief Turns off the TCD0 counter.
 * 
 * @details Waits until the TCD is ready to be disabled, then deactivates the timer.
 */
void TCD0_OFF() {
    while (!(TCD0.STATUS & TCD_ENRDY_bm)); ///< Wait until the TCD is ready
    TCD0.CTRLA &= ~TCD_ENABLE_bm; ///< Disable the TCD0 counter
}

/**
 * @brief Initializes PWM settings for TCD0.
 * 
 * @param target_freq The desired PWM frequency in Hz.
 * @param duty_cycle The desired duty cycle in percentage (0.1% to 99.9%).
 * 
 * @details Automatically calculates the required TCD0 settings based on the system clock,
 *          prescaler, and target frequency. The function adjusts the timer's compare registers
 *          to produce the specified frequency and duty cycle.
 * 
 * @note Ensure the system clock and PLL settings are correctly configured for the desired base frequency.
 */
void PWM_init(uint32_t target_freq, float duty_cycle) {
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

    // Calculate TCD prescaler
    uint8_t TCD_prescaler = 1;
    switch (TCD0.CTRLA & TCD_CNTPRES_gm) {
        case TCD_CNTPRES_DIV4_gc:  TCD_prescaler = 4; break;
        case TCD_CNTPRES_DIV32_gc: TCD_prescaler = 32; break;
    }

    // Calculate compare registers
    uint16_t cmpbclr = (base_freq / (TCD_prescaler * target_freq * 2)) - 1;
    uint16_t cmpaset = (uint16_t)(cmpbclr * (duty_cycle / 100.0f)) + 1;
    uint16_t cmpbset = cmpbclr - cmpaset - 1;

    // Set TCD compare registers
    TCD0.CMPBCLR = cmpbclr;
    TCD0.CMPBSET = cmpbset;
    TCD0.CMPASET = cmpaset;

}

/**
 * @brief Initializes TCD0 for PWM generation.
 * 
 * @details Configures the waveform generation mode, fault control, and clock source.
 *          This function also selects the WOC (Waveform Output Compare) pin configuration.
 */
void TCD0_init() {
    PORTMUX.TCDROUTEA = PORTMUX_TCD0_ALT4_gc; ///< Select alternative WOC pin variant 4
    ccp_write_io((uint8_t *) &TCD0.FAULTCTRL, TCD_CMPCEN_bm); ///< Enable WOC on PD4 (pin 14)

    TCD0.CTRLB = TCD_WGMODE_DS_gc; ///< Set waveform mode to double slope

    while (!(TCD0.STATUS & TCD_ENRDY_bm)); ///< Wait until TCD is ready for configuration
    TCD0.CTRLA = TCD_CLKSEL_PLL_gc | ///< Select PLL as clock source
                 TCD_CNTPRES_DIV1_gc; ///< Select prescaler
}
