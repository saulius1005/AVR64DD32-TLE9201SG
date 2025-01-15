/**
 * @file TLE9201SG.c
 * @brief Implementation of TLE9201SG motor driver functions, including SPI and PWM control.
 * @author Saulius
 * @date 2025-01-10
 */

#include "Settings.h"
#include "TLE9201SGVar.h"

/**
 * @brief Parses and updates the diagnosis data from the TLE9201SG.
 */
void TLE9201SG_Sort_Diagnosis() {
	TLE9201SG.EN = GET_BIT(TLE9201SG.diag, 7);
	TLE9201SG.OT = GET_BIT(TLE9201SG.diag, 6);
	TLE9201SG.TV = GET_BIT(TLE9201SG.diag, 5);
	TLE9201SG.CL = GET_BIT(TLE9201SG.diag, 4);
	TLE9201SG.DIA = GET_BITS(TLE9201SG.diag, 0x0F);

	TLE9201SG.Fault = (TLE9201SG.DIA != 0xF) ? TLE9201SG.DIA : 0;
}

/**
 * @brief Parses and updates the control data from the TLE9201SG.
 */
void TLE9201SG_Sort_Control() {
    TLE9201SG.CMD = (TLE9201SG.control >> 5);         // Extract Bits 7-5
    TLE9201SG.OLDIS = GET_BIT(TLE9201SG.control, 4);  // Extract Bit 4
    TLE9201SG.SIN = GET_BIT(TLE9201SG.control, 3);    // Extract Bit 3
    TLE9201SG.SEN = GET_BIT(TLE9201SG.control, 2);    // Extract Bit 2
    TLE9201SG.SDIR = GET_BIT(TLE9201SG.control, 1);   // Extract Bit 1
    TLE9201SG.SPWM = GET_BIT(TLE9201SG.control, 0);   // Extract Bit 0
}

/**
 * @brief Constructs the control command for the TLE9201SG motor driver.
 *
 * This function generates a control command for the TLE9201SG motor driver by 
 * combining the base command with various control flags.
 *
 * @param command The base command to be sent to the motor driver.
 * @return The constructed control command.
 */
uint8_t TLE9201SG_Write(uint8_t command) {
    return command |
           (TLE9201SG.OLDIS << 4) |
           (TLE9201SG.SIN << 3) |
           (TLE9201SG.SEN << 2) |
           (TLE9201SG.SDIR << 1) |
           TLE9201SG.SPWM;
}

/**
 * @brief Initializes the TLE9201SG motor driver in SPI mode.
 *
 * This function stops any ongoing SPI communication, initializes the SPI module, 
 * and sets up the TLE9201SG for SPI mode operation. It also calculates the virtual 
 * PWM signal timing for simulating PWM behavior through SPI.
 */
void TLE9201SG_SPI_Mode_Init() {
    SPI0_Stop(); // Ensure SPI0 module is stopped
    SPI0_init();

    // Enable SPI control and disable outputs
    TLE9201SG.SIN = 1;
    TLE9201SG.OLDIS = 0;
    TLE9201SG.SEN = 0;

    uint8_t init_cmd[3] = {TLE9201SG_Write(WR_CTRL), RD_REV, 0};
    for (uint8_t i = 0; i < 3; i++) {
        init_cmd[i] = SPI0_Exchange_Data(init_cmd[i]);
    }

    // Update control and revision values
    TLE9201SG.control = init_cmd[1];
    TLE9201SG.revision = init_cmd[2];
    TLE9201SG_Sort_Control();

    // PWM signal timing calculations
    float sig_calc = 1.0f / CLOCK_read() * 4.0f; // Calculate the time base based on the current main clock
    float sig_period = (1.0f / TLE9201SG.pwm_freq) - TLE9201SG_SPI_TIME_COMPENSATION; // Calculate period for required frequency
    float sig_on = (TLE9201SG.duty_cycle / 100.0f) * sig_period; // Calculate PWM duty cycle

    TLE9201SG.off = (sig_period - sig_on) / sig_calc; // Calculate PWM off time
    TLE9201SG.on = sig_on / sig_calc;                // Calculate PWM on time
}

/**
 * @brief Initializes the TLE9201SG motor driver in PWM/DIR mode.
 *
 * This function initializes the hardware components required for the PWM/DIR 
 * control mode, including the Phase-Locked Loop (PLL), Timer/Counter D (TCD), 
 * and the PWM generation module.
 */
void TLE9201SG_PWM_Mode_Init() {
    PLL_init();  // Initialize Phase-Locked Loop (PLL)
    TCD0_init(); // Initialize Timer/Counter D (TCD)
    PWM_init(TLE9201SG.pwm_freq, TLE9201SG.duty_cycle);
}

/**
 * @brief Initializes the TLE9201SG motor driver in the selected control mode.
 *
 * This function sets up the TLE9201SG motor driver for operation in either SPI 
 * mode or PWM/DIR mode based on the provided mode parameter. 
 *
 * @param mode The desired control mode:
 *             - 0: PWM/DIR mode
 *             - 1: SPI mode
 */
void TLE9201SG_Mode_init(uint8_t mode) {
    TLE9201SG.mode = mode;

    if (mode) {
        TLE9201SG_SPI_Mode_Init(); // SPI mode initialization
    } else {
        TLE9201SG_PWM_Mode_Init(); // PWM/DIR mode initialization
    }
}

/**
 * @brief Turns off the TLE9201SG outputs.
 * 
 * This function disables the TLE9201SG outputs, either via SPI or by controlling 
 * the hardware pin directly, depending on the current control mode.
 */
void TLE9201SG_STOP() {
    if (TLE9201SG.mode) { // SPI mode
        TLE9201SG.SEN = 0; // Disable outputs
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
		// Update only the direction bit (PIN5_bp)
		PORTD.OUT = (PORTD.OUT & ~(1 << PIN5_bp)) | (direction << PIN5_bp);
	}
}

/**
 * @brief Starts the motor driver outputs.
 * 
 * This function starts the motor driver outputs, either by toggling the SPWM bit
 * via SPI or by enabling the timer/counter in PWM/DIR mode.
 */
void TLE9201SG_START() {
    if (TLE9201SG.mode) { // SPI mode imitating pwm...
		TLE9201SG.SEN = 1; // Enable outputs
        TLE9201SG.SPWM = 1;
		TLE9201SG.diag = SPI0_Exchange_Data(TLE9201SG_Write(WR_CTRL_RD_DIA)); //returning not curent value but value from the past... I hate SPI on TLE9201SG!!!
        _delay_loop_2(TLE9201SG.on); // Wait for the on-time duration
		TLE9201SG_Sort_Diagnosis(); //but spi have good diagnosis...
        TLE9201SG.SPWM = 0;
	   SPI0_Exchange_Data(TLE9201SG_Write(WR_CTRL_RD_DIA)); //same...
        _delay_loop_2(TLE9201SG.off); // Wait for the off-time duration

    } else { // PWM/DIR mode
        TCD0_ON(); // Enable the timer/counter for easy pwm generation 
		PORTD.OUTCLR = PIN6_bm; // Clear the pin to enable outputs
		if(PORTA.IN & PIN5_bm)
		TLE9201SG.Fault = (PORTA.IN & PIN5_bm) ?  1 : 0; //checking fault flag only...
    }
}

