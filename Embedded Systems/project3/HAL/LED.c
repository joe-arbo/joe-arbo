/*
 * LED.c
 *
 *  Created on: Dec 29, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#include <HAL/LED.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/**
 * Main constructor for an LED. This function should initialize a GPIO port-pin
 * pairing as a general-purpose output LED. The data for the initialized LED
 * should be returned as an LED struct.
 *
 * To determine which port and which pin targets which LED, consult datasheets
 * such as the Quick Reference Guide for both the Launchpad and Boosterpack, as
 * well as the User Manual for both the Launchpad and Boosterpack.
 *
 * @param port:     The GPIO port which targets the proper LED
 * @param pin:      The GPIO pin  which targets the proper LED
 */
LED LED_construct(uint8_t port, uint16_t pin)
{
    // The main LED struct from which to instantiate all other data
    LED led;

    // Initialize each member with the arguments from this function
    led.isLit = false;  // The LED starts off unlit
    led.port  = port;   // The port comes from the constructor arguments
    led.pin   = pin;    // The pin  comes from the constructor arguments

    GPIO_setAsOutputPin(led.port, led.pin);
    GPIO_setOutputLowOnPin(led.port, led.pin);

    // Returns a copy of the LED struct.
    return led;
}

/**
 * Turns on (lights up) a user-specified LED.
 *
 * @param led:  The led to turn on
 */
void LED_turnOn(LED* led)
{
    // Retrieve the port and pin target from the selected LED
    uint8_t port = led->port;
    uint16_t pin = led->pin;

    // Internal logic: when we turn on the LED, update the isLit flag
    led->isLit = true;

    // TODO: For students, replace this call with RTL calls (without driverlib)
    GPIO_setOutputHighOnPin(port, pin);
}

/**
 * Turns off a user-specified LED.
 *
 * @param led:  The led to turn off
 */
void LED_turnOff(LED* led)
{
    // Retrieve the port and pin target from the selected LED
    uint8_t port = led->port;
    uint16_t pin = led->pin;

    // Internal logic: when we turn off the LED, update the isLit flag
    led->isLit = false;

    // TODO: For students, replace this call with RTL calls (without driverlib)
    GPIO_setOutputLowOnPin(port, pin);
}

/**
 * Toggles a user-specified LED.
 *
 * @param led:  The led to toggle
 */
void LED_toggle(LED* led)
{
    // Retrieve the port and pin target from the selected LED
    uint8_t port = led->port;
    uint16_t pin = led->pin;

    // Internal logic: when we toggle the LED, toggle the isLit flag.
    led->isLit = !led->isLit;

    // TODO: For students, replace this call with RTL calls (without driverlib)
    GPIO_toggleOutputOnPin(port, pin);
}

/**
 * Determine whether the LED is lit or not.
 *
 * @param led:  The LED to check
 * @return whether the LED is lit or not
 */
bool LED_isLit(LED* led)
{
    bool isLit = led->isLit;
    return isLit;
}
