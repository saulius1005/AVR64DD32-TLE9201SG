/**
 * @file AVR64DD32-TLE9201SG.c
 * @brief This file contains the implementation for initializing and controlling the TLE9201SG with AVR64DD32 microcontroller.
 * @author Saulius
 * @date 2025-01-10
 */

#include "Settings.h"

/**
 * @brief The main function initializes peripherals and controls the TLE9201SG driver based on input pins.
 * 
 * This function performs the following steps:
 * - Initializes GPIO and the internal high-frequency clock.
 * - Configures the TLE9201SG PWM frequency and duty cycle.
 * - Monitors input pins (PF5 and PF6) to start, stop, or change the direction of the TLE9201SG.
 * 
 * @return int Always returns 0 (not used in embedded systems).
 */
int main(void)
{
    GPIO_init();
    CLOCK_INHF_clock_init(); ///< Initializes the internal high-frequency clock.

    TLE9201SG.pwm_freq = 20000; ///< Sets PWM frequency to 20 kHz. Always set this before mode initialization.
    TLE9201SG.duty_cycle = 50.0; ///< Sets duty cycle to 50%. Always set this before mode initialization.

    TLE9201SG_Mode_init(TLE9201SG_MODE_SPI); ///< Initializes the TLE9201SG in SPI mode.
    // TLE9201SG_OFF(); ///< Optionally disables all outputs.

    while (1) {
        if (!(PORTF.IN & PIN5_bm)) { ///< Starts TLE9201SG if PF5 is low.
            TLE9201SG_ON();
            TLE9201SG_START();
            if (!(PORTF.IN & PIN6_bm)) { ///< Changes direction based on PF6.
                TLE9201SG_DIR(1); ///< Sets direction to forward.
            } else {
                TLE9201SG_DIR(0); ///< Sets direction to reverse.
            }
        } else { ///< Stops TLE9201SG if PF5 is high.
            TLE9201SG_STOP();
            TLE9201SG_OFF();
        }
    }
}
