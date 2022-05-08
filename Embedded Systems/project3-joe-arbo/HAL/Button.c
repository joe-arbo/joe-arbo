/*
 * Button.c
 *
 *  Created on: Dec 30, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#include <HAL/Button.h>

/**
 * Constructs a button as a GPIO pushbutton, given a proper port and pin.
 * Initializes the debouncing and output FSMs.
 *
 * @param port:     The GPIO port used to initialize this button
 * @param pin:      The GPIO pin  used to initialize this button
 *
 * @return a constructed button with debouncing and output FSMs initialized
 */
Button Button_construct(uint8_t port, uint16_t pin)
{
    // The button object which will be returned at the end of construction
    Button button;

    // Initialize the member variables for port and pin of the button.
    button.port = port;
    button.pin = pin;

    // Here's a trick: All buttons on the board can be initialized with a
    // pullup resistor, since a double pullup resistor has no impact on the
    // input voltage of the button.
    GPIO_setAsInputPinWithPullUpResistor(port, pin);

    // Initialize all FSM variables for the button to their RELEASED states
    button.debounceState = StableR;
    button.timer = SWTimer_construct(DEBOUNCE_TIME_MS);
    SWTimer_start(&button.timer);

    // Initialize all buffered outputs of the button
    button.pushState = RELEASED;
    button.isTapped = false;

    // Return the constructed Button object to the user
    return button;
}

/**
 * A getter method which should just return whether the user currently has held
 * down the button. This should be determined using the pushState which was
 * computed last time the Button object was refreshed. It does NOT update the
 * internal FSM and does NOT directly check the GPIO signal directly - that is,
 * do NOT use [GPIO_*()] functions here.
 *
 * @param button:   The Button object from which to retrieve the push state
 *
 * @return true if the button is depressed, and false if it is not
 */
bool Button_isPressed(Button* button)
{
    return button->pushState == PRESSED;
}

/**
 * A getter method which should just return whether the user currently has
 * tapped the button. This should NOT update the internal FSM for debouncing
 * and instead should simply fetch the result which was determined last time
 * the button was refreshed. A tap is defined to be true when the button was
 * not held down two refreshes ago but was held down one refresh ago.
 *
 * @param button:   The Button object from which to retrieve the tapped state
 *
 * @return true if the button was tapped, and false otherwise
 */
bool Button_isTapped(Button* button)
{
    return button->isTapped;
}

/**
 * Refreshes the input of the provided Button by polling for the new GPIO input
 * pin value and advancing the debouncing FSM by one step.
 *
 * @param button:   The Button object to refresh
 */
void Button_refresh(Button* button)
{
    // Retrieve the port and pin targets
    uint8_t port = button->port;
    uint16_t pin = button->pin;

    // Poll for updated values from port and pin status through GPIO directly
    uint16_t rawButtonStatus = GPIO_getInputPinValue(port, pin);
    int newPushState = RELEASED;

    // Main debouncing FSM
    switch (button->debounceState)
    {
        // Released State - transition only if the new raw state is pressed
        case StableR:
            if (rawButtonStatus == PRESSED) {
                SWTimer_start(&button->timer);
                button->debounceState = TransitionRP;
            }
            newPushState = RELEASED;
            break;

        // Pressed State - transition only if the new raw state is released
        case StableP:
            if (rawButtonStatus == RELEASED) {
                SWTimer_start(&button->timer);
                button->debounceState = TransitionPR;
            }
            newPushState = PRESSED;
            break;

        // Transition State - transition if either the timer expires OR if
        //                    the input becomes polluted with an erroneous
        //                    RELEASED input.
        case TransitionRP:
            if (rawButtonStatus == RELEASED) {
                button->debounceState = StableR;
            }
            else if (SWTimer_expired(&button->timer)) {
                button->debounceState = StableP;
            }
            newPushState = RELEASED;
            break;

        // Transition State - transition if either the timer expires OR if
        //                    the input becomes polluted with an erroneous
        //                    PRESSED input.
        case TransitionPR:
            if (rawButtonStatus == PRESSED) {
                button->debounceState = StableP;
            }
            else if (SWTimer_expired(&button->timer)) {
                button->debounceState = StableR;
            }
            newPushState = PRESSED;
    }

    // Outputs of the FSM: The button is tapped if the old debounced state was
    // RELEASED and the new state is PRESSED.
    button->isTapped = newPushState == PRESSED && button->pushState == RELEASED;
    button->pushState = newPushState;
}
