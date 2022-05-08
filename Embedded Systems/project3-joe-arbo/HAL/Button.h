/*
 * Button.h
 *
 *  Created on: Dec 30, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#ifndef HAL_BUTTON_H_
#define HAL_BUTTON_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <HAL/Timer.h>

#define DEBOUNCE_TIME_MS    5
#define PRESSED             0
#define RELEASED            1

/**
 * Predefined Button ports and pins for EACH BUTTON. Consult datasheets like
 * the Launchpad User Guide and the Boostepack User Guide to determine which
 * ports and pins map to which LEDs on the board.
 */
#define LAUNCHPAD_S1_PORT       GPIO_PORT_P1
#define LAUNCHPAD_S1_PIN        GPIO_PIN1

#define LAUNCHPAD_S2_PORT       GPIO_PORT_P1
#define LAUNCHPAD_S2_PIN        GPIO_PIN4

#define BOOSTERPACK_S1_PORT     GPIO_PORT_P5
#define BOOSTERPACK_S1_PIN      GPIO_PIN1

#define BOOSTERPACK_S2_PORT     GPIO_PORT_P3
#define BOOSTERPACK_S2_PIN      GPIO_PIN5

#define BOOSTERPACK_JS_PORT     GPIO_PORT_P4
#define BOOSTERPACK_JS_PIN      GPIO_PIN1

/**
 * A simple enum to keep track of which states of debouncing a button is in.
 */
enum _DebounceState { StableP, TransitionPR, TransitionRP, StableR };
typedef enum _DebounceState DebounceState;

/**=============================================================================
 * A simple Button object, implemented in the C object-oriented style. Use the
 * constructor [Button_construct()] to create a Button object. Afterwards, when
 * accessing each method of the Button object, pass in a pointer to the proper
 * Button as the first argument of the method.
 * =============================================================================
 * USAGE WARNINGS
 * =============================================================================
 * When using this Button object, DO NOT DIRECTLY ACCESS ANY MEMBERS VARIABLES
 * of a Button struct. Treat all members as PRIVATE - that is, you should ONLY
 * access a member of the Button struct if your function name starts with
 * "Button_"!
 *
 * In order to retrieve new data for the button, you MUST use the
 * [Button_refresh()] method ONE TIME at either the beginning or the end of each
 * cycle of the [while (true)] loop in [main()]. In this way, we poll for button
 * inputs exactly ONE TIME per cyclic execution. If your button doesn't seem to
 * work, this is probably why.
 *
 * Since the button works based off a DebounceState FSM, calling
 * [Button_refresh()] only once per cycle ensures that the FSM is only advanced
 * a single time per loop. Recall that the FSMs for debouncing and for
 * determining whether the button has been tapped or not both have outputs at
 * each edge of the FSM. If you accidentally call [Button_refresh()] more than
 * once per button per cycle, you WILL accidentally increment the FSMs more than
 * once, and so you will MISS edge transitions and outputs for the Button FSMs.
 */
struct _Button
{
    uint8_t port;   // The port on the Launchpad to which this Button is mapped
    uint16_t pin;   // The pin  on the Launchpad to which this Button is mapped

    // Keeps track of FSM progress in the Debouncing FSM
    DebounceState debounceState;

    // The timer which is used to wait out a bouncy input
    SWTimer timer;

    // The outputs of the FSM.
    int pushState;  // The output of the debouncing FSM (PRESSED or RELEASED)
    bool isTapped;  // The output of the buttonpushed FSM (true or false).
};
typedef struct _Button Button;

/** Constructs a new button object, given a valid port and pin. */
Button Button_construct(uint8_t port, uint16_t pin);

/** Given a button, determines if the switch is currently pushed down */
bool Button_isPressed(Button* button);

/** Given a button, determines if it was "tapped" - pressed down and released */
bool Button_isTapped(Button* button);

/** Refreshes this button so the Button FSM now has new outputs to interpret */
void Button_refresh(Button* button);

#endif /* HAL_BUTTON_H_ */
