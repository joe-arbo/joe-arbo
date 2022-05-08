/*
 * LED.h
 *
 *  Created on: Dec 29, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
*/

#ifndef HAL_LED_H_
#define HAL_LED_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/**
 * Predefined LED values for EACH LED. Consult datasheets like the Launchpad
 * User Guide and the Boosterpack User Guide to determine which ports and pins
 * map to which LEDs on the board.
 */
#define LAUNCHPAD_LED1_PORT         GPIO_PORT_P1
#define LAUNCHPAD_LED1_PIN          GPIO_PIN0

#define LAUNCHPAD_LED2_RED_PORT     GPIO_PORT_P2
#define LAUNCHPAD_LED2_RED_PIN      GPIO_PIN0

#define LAUNCHPAD_LED2_GREEN_PORT   GPIO_PORT_P2
#define LAUNCHPAD_LED2_GREEN_PIN    GPIO_PIN1

#define LAUNCHPAD_LED2_BLUE_PORT    GPIO_PORT_P2
#define LAUNCHPAD_LED2_BLUE_PIN     GPIO_PIN2

#define BOOSTERPACK_LED_RED_PORT    GPIO_PORT_P2
#define BOOSTERPACK_LED_RED_PIN     GPIO_PIN6

#define BOOSTERPACK_LED_GREEN_PORT  GPIO_PORT_P2
#define BOOSTERPACK_LED_GREEN_PIN   GPIO_PIN4

#define BOOSTERPACK_LED_BLUE_PORT   GPIO_PORT_P5
#define BOOSTERPACK_LED_BLUE_PIN    GPIO_PIN6

/**=============================================================================
 * A simple LED object, implemented in the C object-oriented style. Use the
 * constructor [LED_construct()] to create an LED object. Afterwards, when
 * accessing each method of the LED object, pass in a pointer to the proper
 * LED as the first argument of the method.
 * =============================================================================
 * USAGE WARNINGS
 * =============================================================================
 * When using this LED object, DO NOT DIRECTLY ACCESS ANY MEMBER VARIABLES of
 * an LED struct. Treat all members as PRIVATE - that is, you should ONLY
 * access a member of the LED struct if your function name starts with "LED_"!
 *
 * We recommend that if you wish to determine whether an LED is lit, you use the
 * LED_isLit() method, rather than manually checking the struct's isLit member
 * directly.
 */
struct _LED
{
    uint8_t port;
    uint16_t pin;
    bool isLit;
};
typedef struct _LED LED;

// Constructs an LED object.
LED LED_construct(uint8_t port, uint16_t pin);

// Turns the provided LED on.
void LED_turnOn(LED* led);

// Turns the provided LED off.
void LED_turnOff(LED* led);

// Toggles the provided LED. Turns off lit LEDs, and turns on unlit LEDs.
void LED_toggle(LED* led);

// Returns whether the LED is currently lit or not.
bool LED_isLit(LED* led);

#endif /* HAL_LED_H_ */
