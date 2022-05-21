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

enum _screenChoice
{
    PLAY, RULES, SCORES, NUM_SCREENS, DEATH
};
typedef enum _screenChoice screenChoice;

enum _difficulty
{
    BASE, EASY, MEDIUM, HARD, NUM_DIFF
};
typedef enum _difficulty difficulty;

struct _Application
{
    // Put your application members and FSM state variables here!
    // =========================================================================
    UART_Baudrate baudChoice;
    bool MainScreen, canRedraw, shieldStart, canDrawShield, canDrawDanger;
    double xpos, ypos, maxSpeed;
    int shieldPosX, shieldPosY, shieldPoints, dangerPosX, dangerPosY, healthPoints, score, deaths;
    int highScores[5];
    screenChoice screen;
    difficulty diff;
};
typedef struct _Application Application;

// Called only a single time - inside of main(), where the application is constructed
Application Application_construct();

void Application_reset(Application* app); //Resets App

// Called once per super-loop of the main application.
void Application_loop(Application* app, HAL* hal, Graphics_Context context, Graphics_Context image, Graphics_Context circle, Graphics_Context circle2,
                      Graphics_Context shieldVoid, Graphics_Context greenShield, bool joyStickPushedtoDown, bool joyStickPushedtoUp,
                      bool joyStickPushedtoRight, bool joyStickPushedtoLeft, int Vx, int Vy, SWTimer* shieldTimer, SWTimer* shieldSpawnTimer, SWTimer* dangerSpawnTimer);

// Called whenever the UART module needs to be updated
void Application_updateCommunications(Application* app, HAL* hal);

// Interprets an incoming character and echoes back to terminal what kind of
// character was received (number, letter, or other)
void Application_interpretIncomingChar(Application* app, HAL* hal);

//Updates the Screen
void Application_updateScreen(Application* app, HAL* hal, Graphics_Context context, Graphics_Context circle);

// Generic circular increment function
uint32_t CircularIncrement(uint32_t value, uint32_t maximum);

#endif /* APPLICATION_H_ */
