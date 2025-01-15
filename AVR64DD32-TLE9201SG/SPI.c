/*
 * @file SPI.c
 * @brief SPI initialization and control functions for AVR64DD32 microcontroller.
 *
 * @details This file contains functions for initializing the SPI0 module, 
 *          starting and stopping communication with the SPI slave device.
 * 
 * @author Saulius
 * @date 2025-01-10
 */

#include "Settings.h"

/**
 * @brief Initializes the SPI0 module for communication.
 * 
 * @details 
 * - Configures SPI0 as a Master with a clock speed of 6 MHz (F_CPU/4).
 * - Sets SPI mode 1 for communication with TLE9201SG.
 * - Enables the SPI0 module.
 */
void SPI0_init() {
    SPI0.CTRLA = SPI_MASTER_bm          // Configure as Master
               | SPI_PRESC_DIV4_gc      // Clock speed = F_CPU / 4 = 24 MHz / 4 = 6 MHz
               | SPI_ENABLE_bm;         // Enable SPI
    SPI0.CTRLB = SPI_MODE_1_gc;         // Set SPI mode 1 for TLE9201SG
}

/**
 * @brief Starts SPI communication by pulling the SS pin low.
 * 
 * @details 
 * - Pulls the Slave Select (SS) line low to indicate the start of communication 
 *   with the SPI slave device.
 */
void SPI0_Start() {
    PORTA.OUTCLR = PIN7_bm; // Set SS (PA7) low
}

/**
 * @brief Stops SPI communication by pulling the SS pin high.
 * 
 * @details 
 * - Pulls the Slave Select (SS) line high to indicate the end of communication 
 *   with the SPI slave device.
 */
void SPI0_Stop() {
    PORTA.OUTSET = PIN7_bm; // Set SS (PA7) high
}

/**
 * @brief Exchanges a byte of data via SPI0.
 *
 * This function transmits a single byte of data to an SPI slave device and 
 * simultaneously receives a byte of data from the slave device.
 * It ensures the SPI communication is correctly initiated and terminated by 
 * managing the slave select (SS) line.
 *
 * @param data_storage The byte of data to send to the SPI slave.
 * @return The byte of data received from the SPI slave.
 */
uint8_t SPI0_Exchange_Data(uint8_t data_storage) {
    SPI0_Start(); // Pull SS low to initiate communication
    SPI0.DATA = data_storage; // Send the data
    while (!(SPI0.INTFLAGS & SPI_IF_bm)) {} // Wait until data is exchanged
    SPI0_Stop(); // Pull SS high to terminate communication
    return SPI0.DATA; // Return the received data
}
