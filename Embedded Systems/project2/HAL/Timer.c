/*
 * Timer.c
 *
 *  Created on: Dec 29, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#include <HAL/Timer.h>

/** The reference counter which tracks how many rollovers have occurred. Used in timing SWTimers. */
static volatile uint64_t hwTimerRollovers = 0;

/**
 * The ISR used to increment the total number of rollovers which have passed. When the
 * TIMER32_0_BASE timer expires, this ISR is automatically called. DO NOT DIRECTLY INVOKE THIS
 * FUNCTION FROM YOUR CODE, or you WILL destroy the accuracy of ALL software timers in your code.
 */
void T32_INT1_IRQHandler()
{
    hwTimerRollovers++;
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
}

/**
 * Initializes the global system timing. This function should be called immediately after the
 * Watchdog timer is reset, so that the system clock is set appropriately.
 *
 * To change the system clock to different frequencies, use the #define on the SYSTEM_CLOCK in
 * Timer.h. DO NOT MODIFY THIS FUNCTION UNLESS YOU KNOW WHAT YOU ARE DOING. You can potentially
 * brick your board, which requires a factory reset to fix.
 */
void InitSystemTiming()
{
    // Before initializing anything else, disable all interrupts
    Interrupt_disableMaster();

    // Before changing the clock frequency, we need to change the flash control to use 2 wait
    // states (2 delayed cycles per flash read). IF YOU DO NOT CHANGE YOUR FLASH CONTROL BEFORE
    // CALLING CS_setDCOFrequency(), YOU WILL BRICK YOUR BOARD AND WILL NEED TO PERFORM A
    // FACTORY RESET.
    //
    // The reason why we need to change the Flash Control settings is because although calling
    // CS_setDCOFrequency() may change the system clock frequency itself, the flash memory has NOT
    // been configured for instruction fetches at a higher frequency. Omitting these two lines WILL
    // cause your board to attempt to fetch instructions before flash memory has properly fetched
    // them, meaning your board will begin to read garbage instructions and will no longer be
    // flashable without a factory reset.
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);

    // Set the system clock frequency to user-specified frequency
    CS_setDCOFrequency(SYSTEM_CLOCK);

    // After DCO is set, configure all other clock signals to use a source from DCO.
    CS_initClockSignal(CS_MCLK  , CS_DCOCLK_SELECT , CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT , CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK , CS_DCOCLK_SELECT , CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK  , CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // Initialize the main hardware timer under which all other software timers are based. This
    // should be a periodic timer with the maximum load value supported and a prescaler of 1 in
    // order to minimize the frequency of interrupts while keeping a high timer resolution.
    Timer32_initModule(TIMER32_0_BASE, PRESCALER, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_setCount(TIMER32_0_BASE, LOADVALUE);

    // Starts the main reference hardware timer and enables an interrupt which counts rollovers
    Timer32_startTimer(TIMER32_0_BASE, false);

    // Enable interrupts again, after all system timing has been set up properly
    Interrupt_enableMaster();
    Interrupt_enableInterrupt(INT_T32_INT1);
}

/**
 * Constructs a new Software Timer, using a wait time in milliseconds. The timer uses the
 * hwTimerRollovers variable to keep track of its reference time, and is based off of time passing
 * under the TIMER32_0_BASE. When first constructed, this timer is NOT conditioned to start. Before
 * any calls to SWTimer_expired(), SWTimer_elapsedTimeUS(), or SWTimer_percentElapsed(), you MUST
 * FIRST CALL the SWTimer_start() method.
 *
 * @param waitTime_ms:  The amount of time this timer measures before expiration
 * @return a SWTimer object
 */
SWTimer SWTimer_construct(uint64_t waitTime_ms)
{
    SWTimer timer;

    timer.startCounter = 0;
    timer.startRollovers = 0;

    uint64_t counterClock = SYSTEM_CLOCK / PRESCALER;
    uint64_t cyclesPerMillisecond = counterClock / MS_DIVISION_FACTOR;
    timer.cyclesToWait = cyclesPerMillisecond * waitTime_ms;

    return timer;
}

/**
 * Starts a constructed timer by reading the current number of rollovers and current load value in
 * TIMER32_0_BASE.
 *
 * @param timer:    The SWTimer to start
 */
void SWTimer_start(SWTimer* timer)
{
    timer->startCounter = Timer32_getValue(TIMER32_0_BASE);
    timer->startRollovers = hwTimerRollovers;
}

/**
 * A helper method to determine how many cycles have elapsed since the SWTimer started. As the user,
 * you most likely do NOT need to call this method outside of the Timer.c file. This method is used
 * in calculating how much time has elapsed for each of the methods below. If the timer was never
 * started, this function instead will return the number of cycles from the start of the program's
 * execution.
 *
 * @param timer:    The SWTimer with which we measure the number of cycles elapsed
 * @return the number of cycles elapsed since the timer started.
 */
uint64_t SWTimer_elapsedCycles(SWTimer* timer)
{
    uint64_t rollovers = hwTimerRollovers - timer->startRollovers;
    uint64_t startCounter = timer->startCounter;
    uint64_t currentCounter = Timer32_getValue(TIMER32_0_BASE);
    uint64_t elapsedCycles = (rollovers * LOADVALUE) + startCounter - currentCounter;

    return elapsedCycles;
}

/**
 * Determines whether the proper amount of time has elapsed on this timer.
 *
 * @param timer:    The target timer used in determining expiration
 * @return true if the timer is expired and false otherwise
 */
bool SWTimer_expired(SWTimer* timer)
{
    uint64_t elapsedCycles = SWTimer_elapsedCycles(timer);
    return elapsedCycles >= timer->cyclesToWait;
}

