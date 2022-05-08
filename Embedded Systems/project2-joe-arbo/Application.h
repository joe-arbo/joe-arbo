/*
 * Application.h
 *
 *  Created on: Dec 29, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <HAL/HAL.h>

struct _Application
{
    // Put your application members and FSM state variables here!
    // =========================================================================
    UART_Baudrate baudChoice;
    bool firstCall;
};
typedef struct _Application Application;

// Called only a single time - inside of main(), where the application is constructed
Application Application_construct();

// Called once per super-loop of the main application.
void Application_loop(Application* app, HAL* hal, Graphics_Context context, Graphics_Context circle1, Graphics_Context circle2, int happ, int enrg, bool dead);

// Called whenever the UART module needs to be updated
void Application_updateCommunications(Application* app, HAL* hal, Graphics_Context context);

// Interprets an incoming character and echoes back to terminal what kind of
// character was received (number, letter, or other)
void Application_interpretIncomingChar(Application* app, HAL* hal, Graphics_Context context, Graphics_Context circle1, Graphics_Context circle2, int happ, int enrg, bool dead);

// Generic circular increment function
uint32_t CircularIncrement(uint32_t value, uint32_t maximum);

#endif /* APPLICATION_H_ */
