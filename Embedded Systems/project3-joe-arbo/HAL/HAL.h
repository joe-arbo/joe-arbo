/*
 * HAL.h
 *
 *  Created on: Dec 29, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#ifndef HAL_HAL_H_
#define HAL_HAL_H_

#include <HAL/Button.h>
#include <HAL/LED.h>
#include <HAL/Timer.h>
#include <HAL/UART.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/**============================================================================
 * The main HAL struct. This struct encapsulates all of the other input structs
 * in this application as individual members. This includes all LEDs, all
 * Buttons, one HWTimer from which all software timers should reference, the
 * Joystick, and any other peripherals with which you wish to interface.
 * ============================================================================
 * USAGE WARNINGS
 * ============================================================================
 * YOU SHOULD HAVE EXACTLY ONE HAL STRUCT IN YOUR ENTIRE PROJECT. We recommend
 * you put this struct inside of a main [Application] object so that every
 * single function in your application has access to the main inputs and outputs
 * which interface with the hardware on the MSP432.
 */
struct _HAL
{
    // LEDs - Left Launchpad LED (the LED is only a single color - red)
    LED launchpadLED1;

    // LEDs - Right Launchpad LEDs (one LED struct for red, one for blue, etc.)
    LED launchpadLED2Red;
    LED launchpadLED2Blue;
    LED launchpadLED2Green;

    // LEDs - Boosterpack LED (one LED struct for red, one for blue, etc.)
    LED boosterpackRed;
    LED boosterpackBlue;
    LED boosterpackGreen;

    // Buttons - Launchpad S1 and S2
    Button launchpadS1;
    Button launchpadS2;

    // Buttons - Boosterpack S1, S2, and JS (press down on the joystick)
    Button boosterpackS1;
    Button boosterpackS2;
    Button boosterpackJS;

    // UART - Construct a new UART instance
    UART uart;
};
typedef struct _HAL HAL;

// Constructs an HAL object by calling the constructor of each individual member
HAL HAL_construct();

// Refreshes all necessary inputs in the HAL
void HAL_refresh(HAL* api);

#endif /* HAL_HAL_H_ */
