/*
 * Timer.h
 *
 *  Created on: Dec 29, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#ifndef HAL_TIMER_H_
#define HAL_TIMER_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#define MS_DIVISION_FACTOR  1000        // Number of milliseconds in one second
#define US_DIVISION_FACTOR  1000000     // Number of microseconds in one second

// A globally-defined system clock variable. Changing this variable will change the system clock
// across the ENTIRE BOARD. Any API calls which use the system clock as part of its timing therefore
// should parameterize their variables to this #define and thus #include <API/Timer.h>.
#define SYSTEM_CLOCK        48000000

#define LOADVALUE           0xFFFFFFFF
#define PRESCALER           1

/**=================================================================================================
 * A Software timer object, implemented in the C object-oriented style. Use the constructor
 * [SWTimer_construct()] to create a software timer. The only method which works after a timer is
 * constructed is the [SWTimer_start()] method. All other methods only work AFTER [SWTimer_start()]
 * is called on a timer object. If you wish to restart a constructed timer, simply call
 * [SWTimer_start()] a second time.
 * =================================================================================================
 * USAGE WARNINGS
 * =================================================================================================
 * When using this object, DO NOT DIRECTLY ACCESS ANY MEMBER VARIABLES of a SWTimer struct. Treat
 * all members as PRIVATE - that is, you should only access a member of the SWTimer struct if your
 * function name starts with "SWTimer_*"!
 */
struct _SWTimer
{
    // The number of hardware timer cycles which must elapse before the timer expires
    uint64_t cyclesToWait;

    // The starting counter value of the hardware timer, set when the timer is started
    uint32_t startCounter;

    // The starting rollover value of the hardware timer, set when the timer is started
    uint32_t startRollovers;
};
typedef struct _SWTimer SWTimer;

// Constructs a Software timer. All timers must be constructed before starting them.
SWTimer SWTimer_construct(uint64_t waitTime_ms);

// Starts a software timer. All constructed timers must be started before use.
void SWTimer_start(SWTimer* timer);

// A helper function used for determining how many cycles have elapsed since the
// timer was started. You do not need to call this function outside of Timer.c.
uint64_t SWTimer_elapsedCycles(SWTimer* timer);

// Returns true if the timer has expired, and false otherwise
bool SWTimer_expired(SWTimer* timer);

// Initializes the global clock system for the MSP432, as well as a hardware
// timer under which all of the software timers are based.
void InitSystemTiming();

#endif /* HAL_TIMER_H_ */
