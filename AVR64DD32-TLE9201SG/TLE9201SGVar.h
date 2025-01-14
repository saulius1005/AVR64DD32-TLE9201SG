/**
 * @file TLE9201SGVar.h
 * @brief Initialization of the TLE9201SG global variable.
 * 
 * @details This file defines and initializes the `TLE9201SG` structure, which stores 
 *          the configuration and status of the TLE9201SG motor driver. 
 * 
 * @author Saulius
 * @date 2025-01-10
 */

#ifndef TLE9201SGVAR_H_
#define TLE9201SGVAR_H_

#include "TLE9201SG.h" ///< Include the header file for the TLE9201SG structure definition.

/**
 * @brief Global instance of TLE9201SG_DATA structure.
 * 
 * @details This variable holds the initial configuration and default values for 
 *          the TLE9201SG motor driver. These values can be modified during runtime 
 *          based on application requirements.
 */
TLE9201SG_DATA TLE9201SG = {
    .revision = 0x00, ///< Default revision value (reset state).
    .diag = 0x00,     ///< Default diagnosis register value (reset state).
    .Fault = 0x00,    ///< No faults detected (reset state).
    .OLDIS = 0,       ///< Outputs are enabled by default.
    .SIN = 0,         ///< Default SPI control (disabled).
    .SEN = 0,         ///< Default SPI (off).
    .SDIR = 0,        ///< Default direction (neutral or forward).
    .SPWM = 0,        ///< Default PWM status (disabled).
    .mode = TLE9201SG_MODE_PWMDIR ///< Default mode is PWM/DIR.
};

#endif /* TLE9201SGVAR_H_ */
