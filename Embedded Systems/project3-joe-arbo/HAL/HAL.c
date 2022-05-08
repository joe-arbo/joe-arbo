/*
 * API.c
 *
 *  Created on: Dec 29, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#include <HAL/HAL.h>

/**
 * Constructs a new API object. The API constructor should simply call the constructors of each
 * of its sub-members with the proper inputs.
 *
 * @return a properly constructed API object.
 */
HAL HAL_construct()
{
    // The API object which will be returned at the end of construction
    HAL hal;

    // Initialize all LEDs by calling their constructors with correctly-defined arguments.
    hal.launchpadLED1 = LED_construct(LAUNCHPAD_LED1_PORT, LAUNCHPAD_LED1_PIN);

    hal.launchpadLED2Red   = LED_construct(LAUNCHPAD_LED2_RED_PORT  , LAUNCHPAD_LED2_RED_PIN  );
    hal.launchpadLED2Green = LED_construct(LAUNCHPAD_LED2_GREEN_PORT, LAUNCHPAD_LED2_GREEN_PIN);
    hal.launchpadLED2Blue  = LED_construct(LAUNCHPAD_LED2_BLUE_PORT , LAUNCHPAD_LED2_BLUE_PIN );

    hal.boosterpackRed   = LED_construct(BOOSTERPACK_LED_RED_PORT  , BOOSTERPACK_LED_RED_PIN  );
    hal.boosterpackGreen = LED_construct(BOOSTERPACK_LED_GREEN_PORT, BOOSTERPACK_LED_GREEN_PIN);
    hal.boosterpackBlue  = LED_construct(BOOSTERPACK_LED_BLUE_PORT , BOOSTERPACK_LED_BLUE_PIN );

    // Initialize all Buttons by calling their constructors with correctly-defined arguments.
    hal.launchpadS1 = Button_construct(LAUNCHPAD_S1_PORT, LAUNCHPAD_S1_PIN);        // Launchpad S1
    hal.launchpadS2 = Button_construct(LAUNCHPAD_S2_PORT, LAUNCHPAD_S2_PIN);        // Launchpad S2

    hal.boosterpackS1 = Button_construct(BOOSTERPACK_S1_PORT, BOOSTERPACK_S1_PIN);  // Boosterpack S1
    hal.boosterpackS2 = Button_construct(BOOSTERPACK_S2_PORT, BOOSTERPACK_S2_PIN);  // Boosterpack S2
    hal.boosterpackJS = Button_construct(BOOSTERPACK_JS_PORT, BOOSTERPACK_JS_PIN);  // Joystick Button

    // Initialize the UART module inside of this HAL struct
    hal.uart = UART_construct(BAUD_9600);

    // Once we have finished building the API, return the completed struct.
    return hal;
}

/**
 * Upon every new cycle of the main super-loop, we MUST UPDATE the status of
 * all inputs. In this program, this function is called only once in the
 * Application_loop() function. Since the Application_loop() function is called
 * once per loop of the while (true) loop in main, we are effectively polling
 * all inputs once per loop.
 *
 * @param hal:  The API whose input modules we wish to refresh
 */
void HAL_refresh(HAL* hal)
{
    // Refresh Launchpad buttons
    Button_refresh(&hal->launchpadS1);
    Button_refresh(&hal->launchpadS2);

    // Refresh Boosterpack buttons
    Button_refresh(&hal->boosterpackS1);
    Button_refresh(&hal->boosterpackS2);
    Button_refresh(&hal->boosterpackJS);
}
