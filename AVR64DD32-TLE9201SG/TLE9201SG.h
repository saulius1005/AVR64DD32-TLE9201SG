/**
 * @file TLE9201SG.h
 * @brief Header file for TLE9201SG driver configuration and control.
 *
 * @details This file contains definitions, macros, and data structures 
 *          used for controlling the TLE9201SG motor driver. It supports 
 *          SPI and PWM-DIR control modes, along with diagnostic and control register operations.
 * 
 * @author Saulius
 * @date 2025-01-10
 */

#ifndef TLE9201SG_H_
#define TLE9201SG_H_

/**
 * @brief Extracts the value of a specific bit from a given variable.
 *
 * This macro shifts the specified bit of the input value to the least significant bit (LSB)
 * position and masks it to extract its value (0 or 1).
 *
 * @param value The variable from which to extract the bit.
 * @param bit The bit position to extract (0 for LSB, increasing towards MSB).
 * @return The value of the specified bit (0 or 1).
 *
 * @note Used in functions like TLE9201SG_Sort_Diagnosis() and TLE9201SG_Sort_Control() 
 *       to extract individual data bits for processing.
 */
#define GET_BIT(value, bit) (((value) >> (bit)) & 0x01)

/**
 * @brief Extracts a group of bits from a given variable using a mask.
 *
 * This macro applies a bitwise AND operation between the input value and the mask to extract 
 * the bits of interest, leaving all other bits cleared.
 *
 * @param value The variable from which to extract the bits.
 * @param mask A mask specifying which bits to extract (1 for bits of interest, 0 otherwise).
 * @return The value of the bits of interest, with all other bits cleared.
 *
 * @note Used in functions like TLE9201SG_Sort_Diagnosis() to extract groups of data bits.
 */
#define GET_BITS(value, mask) ((value) & (mask))


/** @brief TLE9201SG mode: SPI control */
#define TLE9201SG_MODE_SPI 1

/** @brief TLE9201SG mode: PWM-DIR control */
#define TLE9201SG_MODE_PWMDIR 0

/** @brief SPI time compensation for sending and receiving data (16 µs). */
#define TLE9201SG_SPI_TIME_COMPENSATION 0.000014 

/** @brief Command to read the Diagnosis Register. */
#define RD_DIA 0b00000000

/** @brief Command to reset the Diagnosis Register. */
#define RES_DIA 0b10000000

/** @brief Command to read the Device Revision Number. */
#define RD_REV 0b00100000

/** @brief Command to read the Control Register. */
#define RD_CTRL 0b01100000

/** @brief Command to write and return Control Register values. */
#define WR_CTRL 0b11100000

/** @brief Command to write Control values and read Diagnosis Register values. */
#define WR_CTRL_RD_DIA 0b11000000

/**
 * @struct TLE9201SG_DATA
 * @brief Structure for storing TLE9201SG configuration and status.
 */
typedef struct {
    uint8_t revision;    ///< Device revision number.
    uint8_t diag;        ///< Diagnosis register value.
    uint8_t control;     ///< Control register value.
    uint8_t EN;          ///< Enable status.
    uint8_t OT;          ///< Over-temperature status.
    uint8_t TV;          ///< Thermal warning status.
    uint8_t CL;          ///< Current limit status.
    uint8_t DIA;         ///< Diagnosis error status.
    uint8_t Fault;       ///< Fault status.
    uint8_t CMD;         ///< Last command sent to the device.
    uint8_t OLDIS;       ///< Output disable status.
    uint8_t SIN;         ///< SPI control.
    uint8_t SEN;         ///< SPI on and off.
    uint8_t SDIR;        ///< Direction status.
    uint8_t SPWM;        ///< PWM status.
    uint8_t back;        ///< Backup register.
    uint8_t mode;        ///< Current operating mode (SPI or PWM-DIR).
    uint16_t pwm_freq;   ///< PWM frequency in Hz.
    float duty_cycle;    ///< Duty cycle percentage (0-100%).
    uint16_t on;         ///< PWM on time using `_delay_loop2()`.
    uint16_t off;        ///< PWM off time using `_delay_loop2()`.
} TLE9201SG_DATA;

/** @brief Global variable for storing TLE9201SG data and configuration. */
extern TLE9201SG_DATA TLE9201SG;

#endif /* TLE9201SG_H_ */
