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
void TLE9201SG_Sort_Control() {
    TLE9201SG.CMD = (TLE9201SG.control >> 5);       // Bits 7-5
    TLE9201SG.OLDIS = (TLE9201SG.control >> 4) & 0x01; // Bit 4
    TLE9201SG.SIN = (TLE9201SG.control >> 3) & 0x01;   // Bit 3
    TLE9201SG.SEN = (TLE9201SG.control >> 2) & 0x01;   // Bit 2
    TLE9201SG.SDIR = (TLE9201SG.control >> 1) & 0x01;  // Bit 1
    TLE9201SG.SPWM = TLE9201SG.control & 0x01;         // Bit 0
}

uint8_t TLE9201SG_Write(uint8_t command){
	 return command + (TLE9201SG.OLDIS<<4) + (TLE9201SG.SIN<<3) + (TLE9201SG.SEN<<2) + (TLE9201SG.SDIR<<1) + TLE9201SG.SPWM;
}

/**
 * @brief Initializes the TLE9201SG control mode (SPI or PWM/DIR).
 * @param mode 0 for PWM/DIR, 1 for SPI.
 */
 void TLE9201SG_Mode_init(uint8_t mode) {

    TLE9201SG.mode = mode;
    if (mode) { // SPI mode 
		SPI0_Stop();// Ensure SPI0 module is stopped
		double sig_period = 0.0; //for virtual pwm period calculation
		double sig_calc = 0.0; //for CPU clock calculation
		double sig_on = 0.0; //for virtual PWM on state signal time calculation

        SPI0_init();
        TLE9201SG.SIN = 1; // Enable SPI control
        TLE9201SG.OLDIS = 0;
        TLE9201SG.SEN = 0; // Disable outputs
		uint8_t init_cmd[3] = {TLE9201SG_Write(WR_CTRL), RD_REV, 0}; //write controll and receive control values, read revison, 0- next read will be diag
		for(uint8_t i = 0; i<3; i++){
			init_cmd[i] = SPI0_Exchange_Data(init_cmd[i]);		 
		}
		//0  (first)value is nonsense and it not revision.... as it should be... I receiving 0x5c some trash data... this is also not diag data... as datasheet says
		TLE9201SG.control = init_cmd[1];
		TLE9201SG.revision = init_cmd[2];
        TLE9201SG_Sort_Control();

		sig_calc = 1.0 / CLOCK_read() * 4; //calculating time base according current main clock value
		sig_period = (1.0 / TLE9201SG.pwm_freq) - TLE9201SG_SPI_TIME_COMPENSATION; //calculating period time for requared frequency
		sig_on = TLE9201SG.duty_cycle/100 * sig_period; //calculating pwm duty cycle
		TLE9201SG.off = ((sig_period - sig_on)/ sig_calc); //calculating pwm off time
		TLE9201SG.on =  (sig_on/sig_calc); //calculating pwm on time

    } else { // PWM/DIR mode
        PLL_init(); ///< Initialize the Phase-Locked Loop (PLL)
        TCD0_init(); ///< Initialize Timer/Counter D (TCD)
        PWM_init(TLE9201SG.pwm_freq, TLE9201SG.duty_cycle);
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

