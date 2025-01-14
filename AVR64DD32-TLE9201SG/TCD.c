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


void PWM_init(uint32_t target_freq, float duty_cycle) {
    // Calculate TCD prescaler
    uint8_t TCD_prescaler = 1;
    switch (TCD0.CTRLA & TCD_CNTPRES_gm) {
        case TCD_CNTPRES_DIV4_gc:  TCD_prescaler = 4; break;
        case TCD_CNTPRES_DIV32_gc: TCD_prescaler = 32; break;
    }
    // Calculate compare registers
    uint16_t cmpbclr = (CLOCK_read() / (TCD_prescaler * target_freq * 2)) - 1;
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
    TCD0.CTRLA = TCD_CLKSEL_OSCHF_gc | ///< Select PLL as clock source
                 TCD_CNTPRES_DIV1_gc; ///< Select prescaler
}
