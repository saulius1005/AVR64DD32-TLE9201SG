/**
 * @file TLE9201SG.c
 * @brief Implementation of TLE9201SG motor driver functions, including SPI and PWM control.
 * @author Saulius
 * @date 2025-01-10
 */

#include "Settings.h"
#include "TLE9201SGVar.h"

/**
 * @brief Reads data from or writes data to the TLE9201SG via SPI.
 * @param command The SPI command to execute.
 * @param write If non-zero, writes data to the command register; otherwise, reads data.
 * @return The data received via SPI.
 */
uint8_t TLE9201SG_Read(uint8_t command, uint8_t write) {
    static uint8_t firsttime = 0;
    for (uint8_t i = 0; i < 3 - firsttime; i++) { // Read 3 times initially, then twice subsequently, using the last answer.
        SPI0_Start();
        if (write) { // If writing to the command register
            SPI0.DATA = command + (TLE9201SG.OLDIS << 4) + (TLE9201SG.SIN << 3) +
                        (TLE9201SG.SEN << 2) + (TLE9201SG.SDIR << 1) + TLE9201SG.SPWM;
        } else { // Otherwise, just read data
            SPI0.DATA = command;
        }
        while (!(SPI0.INTFLAGS & SPI_RXCIF_bm)); // Wait until data is exchanged
        SPI0_Stop();
    }
    firsttime = 1;
    return SPI0.DATA;
}

/**
 * @brief Reads and updates the TLE9201SG revision register.
 */
void TLE9201SG_Revision() {
    uint8_t raw = TLE9201SG_Read(RD_REV, 0);
    TLE9201SG.revision = raw; // Store the last 3 bits of the revision register
}

/**
 * @brief Parses and updates the diagnosis data from the TLE9201SG.
 */
void TLE9201SG_Sort_Diagnosis() {
    TLE9201SG.EN = (TLE9201SG.diag >> 7) & 0x01;  // Bit 7
    TLE9201SG.OT = (TLE9201SG.diag >> 6) & 0x01;  // Bit 6
    TLE9201SG.TV = (TLE9201SG.diag >> 5) & 0x01;  // Bit 5
    TLE9201SG.CL = (TLE9201SG.diag >> 4) & 0x01;  // Bit 4
    TLE9201SG.DIA = TLE9201SG.diag & 0x0F;        // Bits 3-0

    // If DIA is not 0xF (all OK), set the Fault field; otherwise, clear it.
    TLE9201SG.Fault = (TLE9201SG.DIA != 0xF) ? TLE9201SG.DIA : 0;
}

/**
 * @brief Parses and updates the control data from the TLE9201SG.
 */
void TLE9201SG_Sort_Control_() {
    TLE9201SG.CMD = (TLE9201SG.control >> 5);       // Bits 7-5
    TLE9201SG.OLDIS = (TLE9201SG.control >> 4) & 0x01; // Bit 4
    TLE9201SG.SIN = (TLE9201SG.control >> 3) & 0x01;   // Bit 3
    TLE9201SG.SEN = (TLE9201SG.control >> 2) & 0x01;   // Bit 2
    TLE9201SG.SDIR = (TLE9201SG.control >> 1) & 0x01;  // Bit 1
    TLE9201SG.SPWM = TLE9201SG.control & 0x01;         // Bit 0
}

/**
 * @brief Writes data to a TLE9201SG register.
 * @param command The SPI command to execute.
 * @return The data received via SPI.
 */
uint8_t TLE9201SG_Write_Reg(uint8_t command) {
    static uint8_t firsttime = 0;
    for (uint8_t i = 0; i < 3 - firsttime; i++) {
        SPI0_Start();
        SPI0.DATA = command + (TLE9201SG.OLDIS << 4) + (TLE9201SG.SIN << 3) +
                    (TLE9201SG.SEN << 2) + (TLE9201SG.SDIR << 1) + TLE9201SG.SPWM;
        while (!(SPI0.INTFLAGS & SPI_RXCIF_bm)); // Wait until data is exchanged
        SPI0_Stop();
    }
    firsttime = 1;
    return SPI0.DATA;
}

/**
 * @brief Writes a command to the TLE9201SG and updates internal control or diagnostic data.
 * @param command The SPI command to execute.
 */
void TLE9201SG_Write(uint8_t command) {
    SPI0_Start();
    SPI0.DATA = command + (TLE9201SG.OLDIS << 4) + (TLE9201SG.SIN << 3) +
                (TLE9201SG.SEN << 2) + (TLE9201SG.SDIR << 1) + TLE9201SG.SPWM;
    while (!(SPI0.INTFLAGS & SPI_RXCIF_bm)); // Wait for data exchange

    if (command == WR_CTRL) {
        TLE9201SG.control = SPI0.DATA; // Update control data
        TLE9201SG_Sort_Control_();
    } else {
        TLE9201SG.diag = SPI0.DATA; // Update diagnostic data
        TLE9201SG_Sort_Diagnosis();
    }
    SPI0_Stop();
}

/**
 * @brief Initializes the TLE9201SG control mode (SPI or PWM/DIR).
 * @param mode 0 for PWM/DIR, 1 for SPI.
 */
void TLE9201SG_Mode_init(uint8_t mode) {
    TLE9201SG.mode = mode;
    if (mode) { // SPI mode
        SPI0_init();
        TLE9201SG.SIN = 1; // Enable SPI control
        TLE9201SG.OLDIS = 0;
        TLE9201SG.SEN = 0; // Disable outputs
        TLE9201SG_Write(WR_CTRL);
        TLE9201SG_Revision();
        TLE9201SG.control = TLE9201SG_Read(RD_CTRL, 0); // Read all data
        TLE9201SG_Sort_Control_();
        SPI0_Stop();
    } else { // PWM/DIR mode
        PLL_init(); ///< Initialize the Phase-Locked Loop (PLL)
        TCD0_init(); ///< Initialize Timer/Counter D (TCD)
        PWM_init(TLE9201SG.pwm_freq, TLE9201SG.duty_cycle);
    }
}

/**
 * @brief Turns on the TLE9201SG outputs.
 * 
 * This function enables the TLE9201SG outputs, either via SPI or by controlling 
 * the hardware pin directly, depending on the current control mode.
 */
void TLE9201SG_ON() {
    if (TLE9201SG.mode) { // SPI mode
        TLE9201SG.SEN = 1; // Enable outputs
    } else { // PWM/DIR mode
        PORTD.OUTCLR = PIN6_bm; // Clear the pin to enable outputs
    }
}

/**
 * @brief Turns off the TLE9201SG outputs.
 * 
 * This function disables the TLE9201SG outputs, either via SPI or by controlling 
 * the hardware pin directly, depending on the current control mode.
 */
void TLE9201SG_OFF() {
    if (TLE9201SG.mode) { // SPI mode
        TLE9201SG.SEN = 0; // Disable outputs
        SPI0_Stop();
    } else { // PWM/DIR mode
        TCD0_OFF(); // Turn off the timer/counter
        PORTD.OUTSET = PIN6_bm; // Set the pin to disable outputs
    }
}

/**
 * @brief Sets the direction of the TLE9201SG motor driver.
 * @param direction The desired direction (0 or 1).
 * 
 * This function sets the direction of the motor driver outputs, either via SPI or 
 * by controlling the hardware pin directly, depending on the current control mode.
 */
void TLE9201SG_DIR(uint8_t direction) {
    if (TLE9201SG.mode) { // SPI mode
        TLE9201SG.SDIR = direction;
    } else { // PWM/DIR mode
        PORTD.OUT = (direction << PIN5_bp); // Set the direction pin
    }
}

/**
 * @brief Starts the motor driver outputs.
 * 
 * This function starts the motor driver outputs, either by toggling the SPWM bit
 * via SPI or by enabling the timer/counter in PWM/DIR mode.
 */
void TLE9201SG_START() {
    if (TLE9201SG.mode) { // SPI mode
        TLE9201SG.SPWM = 1;
        TLE9201SG_Write(WR_CTRL_RD_DIA);
        _delay_loop_2(TLE9201SG.on); // Wait for the on-time duration
        TLE9201SG.SPWM = 0;
        TLE9201SG_Write(WR_CTRL_RD_DIA);
        _delay_loop_2(TLE9201SG.off); // Wait for the off-time duration
    } else { // PWM/DIR mode
        TCD0_ON(); // Enable the timer/counter
    }
}

/**
 * @brief Stops the motor driver outputs.
 * 
 * This function stops the motor driver outputs, either via SPI or by disabling
 * the timer/counter in PWM/DIR mode.
 */
void TLE9201SG_STOP() {
    if (TLE9201SG.mode) { // SPI mode
        SPI0_Stop();
    } else { // PWM/DIR mode
        TCD0_OFF(); // Disable the timer/counter
    }
}
