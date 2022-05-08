/**
 * Starter code for Project 2. Good luck!
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* HAL and Application includes */
#include <Application.h>
#include <HAL/HAL.h>
#include <HAL/Timer.h>
#include <HAL/LcdDriver/Crystalfontz128x128_ST7735.h>

#define BUFFER_SIZE 15

// Non-blocking check. Whenever Launchpad S1 is pressed, LED1 turns on.
static void InitNonBlockingLED()
{
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
}
bool happInc, happDec, enrgInc, enrgDec = false;
// Non-blocking check. Whenever Launchpad S1 is pressed, LED1 turns on.
static void PollNonBlockingLED()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == 0) {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
}

Graphics_Context InitGraphics()
{
    Graphics_Context context;

    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&context, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);

    Graphics_setFont(&context, &g_sFontFixed6x8);

    Graphics_setForegroundColor(&context, GRAPHICS_COLOR_BLACK);
    Graphics_setBackgroundColor(&context, GRAPHICS_COLOR_WHITE);
    Graphics_clearDisplay(&context);

    return context;
}

//Circle1 Context
Graphics_Context Circle1Graphics()
{
    Graphics_Context circle1;

    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&circle1, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);

    Graphics_setFont(&circle1, &g_sFontFixed6x8);

    Graphics_setForegroundColor(&circle1, GRAPHICS_COLOR_BLUE);
    Graphics_setBackgroundColor(&circle1, GRAPHICS_COLOR_WHITE);
    Graphics_clearDisplay(&circle1);

    return circle1;
}


//Circle2 Context
Graphics_Context Circle2Graphics()
{
    Graphics_Context circle2;

    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&circle2, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);

    Graphics_setFont(&circle2, &g_sFontFixed6x8);

    Graphics_setForegroundColor(&circle2, GRAPHICS_COLOR_WHITE);
    Graphics_setBackgroundColor(&circle2, GRAPHICS_COLOR_WHITE);
    Graphics_clearDisplay(&circle2);

    return circle2;
}
int posCurrent[] = {23,32};

//Initializes the screens output
void Graphics_Initialize(Graphics_Context context, Graphics_Context circle1)
{
    char buffer[BUFFER_SIZE];


    //Baud Rate
    snprintf(buffer, BUFFER_SIZE, "BR:");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 100, 1, true); //Adds "BR:" to the screen

    snprintf(buffer, BUFFER_SIZE, "%01d", 0);
    Graphics_drawString(&context, (int8_t*) buffer, -1, 120, 1, true);


    //Happy
    snprintf(buffer, BUFFER_SIZE, "Happy: *****");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 105, true);


    //Energy
    snprintf(buffer, BUFFER_SIZE, "Energy:*****");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 115, true);


    //Age
    snprintf(buffer, BUFFER_SIZE, "Age: ");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 1, true);

    snprintf(buffer, BUFFER_SIZE, "%01d", 0);
    Graphics_drawString(&context, (int8_t*) buffer, -1, 25, 1, true);


    //Rectangle
    Graphics_Rectangle rect;
    rect.xMax = 123;
    rect.xMin = 3;
    rect.yMax = 100;
    rect.yMin = 12;
    Graphics_drawRectangle(&context, &rect);


    //Circle (Tamagotchi)
    Graphics_fillCircle(&circle1, posCurrent[0], posCurrent[1], 16);
}
int moves = 0;

//Updates the Baud Rate on Screen
void BR_Graphics_Update(int BR, Graphics_Context context)
{
    char buffer[BUFFER_SIZE];

    snprintf(buffer, BUFFER_SIZE, "%01d", BR);
    Graphics_drawString(&context, (int8_t*) buffer, -1, 120, 1, true);
}

//Decrements the Happy bar on Screen
void Happy_Graphics_Dec(int happ, Graphics_Context context)
{
    char buffer[BUFFER_SIZE];

    int i;
    int n = 0;
    for(i = 0; i<(5-happ); i++)
    {
        snprintf(buffer, BUFFER_SIZE, " ");
        Graphics_drawString(&context, (int8_t*) buffer, -1, (66-n), 105, true);
        n = n + 6;
    }
}

//Decrements the Energy bar on Screen
void Energy_Graphics_Dec(int enrg, Graphics_Context context)
{
    char buffer[BUFFER_SIZE];

    int i;
    int n = 0;
    for(i = 0; i<(5-enrg); i++)
    {
        snprintf(buffer, BUFFER_SIZE, " ");
        Graphics_drawString(&context, (int8_t*) buffer, -1, (66-n), 115, true);
        n = n + 6;
    }
}

//Increments the Happy bar on Screen
void Happy_Graphics_Inc(int happ, Graphics_Context context)
{
    char buffer[BUFFER_SIZE];

    int i;
    int n = 0;
    for(i = 0; i<happ; i++)
    {
        snprintf(buffer, BUFFER_SIZE, "*");
        Graphics_drawString(&context, (int8_t*) buffer, -1, (43+n), 105, true);
        n = n + 6;
    }
}

//Increments the Energy bar on Screen
void Energy_Graphics_Inc(int enrg,  Graphics_Context context)
{
    char buffer[BUFFER_SIZE];

    int i;
    int n = 0;
    for(i = 0; i<enrg; i++)
    {
        snprintf(buffer, BUFFER_SIZE, "*");
        Graphics_drawString(&context, (int8_t*) buffer, -1, (43+n), 115, true);
        n = n + 6;
    }
}

//Updates the age on screen
void Age_Graphics_Update(int age, Graphics_Context context)
{
    char buffer[BUFFER_SIZE];

    snprintf(buffer, BUFFER_SIZE, "%01d", age);
    Graphics_drawString(&context, (int8_t*) buffer, -1, 25, 1, true);
}

int main()
{
    WDT_A_holdTimer();

    // Initialize the system clock and background hardware timer, used to enable
    // software timers to time their measurements properly.
    InitSystemTiming();

    // Initialize the main Application object and HAL object
    HAL hal = HAL_construct();
    Application app = Application_construct();

    Graphics_Context context = InitGraphics();
    Graphics_Context circle1 = Circle1Graphics();
    Graphics_Context circle2 = Circle2Graphics();

    //Tomb stone
    Graphics_Rectangle rect2;
    rect2.xMax = 100;
    rect2.xMin = 28;
    rect2.yMax = 128;
    rect2.yMin = 50;

    char buffer[BUFFER_SIZE];
    int happ = 5;
    int enrg = 5;
    int age = 0;
    bool child = true;
    bool adult = false;
    bool dead = false;

    Graphics_Initialize(context, circle1);


    SWTimer timer = SWTimer_construct(3000);
    SWTimer_start(&timer);

    // Do not remove this line. This is your non-blocking check.
    InitNonBlockingLED();
    while (1)
    {
        // Do not remove this line. This is your non-blocking check.
        PollNonBlockingLED();
        Application_loop(&app, &hal, context, circle1, circle2, happ, enrg, dead);
        HAL_refresh(&hal);

        //Sees if happ or enrg have been updated
        if(happInc)
        {
            happ++;
            happInc = false;
        }
        if(happDec)
        {
            happ--;
            happDec = false;
        }
        if(enrgInc)
        {
            enrg++;
            enrgInc = false;
        }
        if(enrgDec)
        {
           enrg--;
           enrgDec = false;
        }

        if(SWTimer_expired(&timer))
        {
            if (happ > 0) happ--;
            if (enrg > 0) enrg--;
            if (age >= 4 && enrg >= 4 && happ >=4) age++;
            if ((happ > 0 || enrg > 0) && age < 4) age++;


            if (age > 0 && age < 5 && child)
            {
                Graphics_setForegroundColor(&circle1, GRAPHICS_COLOR_PURPLE);
                Graphics_fillCircle(&circle1, posCurrent[0], posCurrent[1], 16);
                child = false;
                adult = true;
            }
            else if (age >= 5 && adult)
            {
                Graphics_setForegroundColor(&circle1, GRAPHICS_COLOR_RED);
                Graphics_fillCircle(&circle1, posCurrent[0], posCurrent[1], 16);
                adult = false;
            }

            if (happ == 0 && enrg == 0 && !dead)
            {
                dead = true;

                Graphics_clearDisplay(&circle2);

                Graphics_setFont(&context, &g_sFontCmss28b);
                Graphics_setForegroundColor(&context, GRAPHICS_COLOR_RED);
                Graphics_setBackgroundColor(&context, GRAPHICS_COLOR_BLACK);
                Graphics_clearDisplay(&context);

                Graphics_setForegroundColor(&circle1, GRAPHICS_COLOR_BLACK);
                Graphics_clearDisplay(&circle1);
                Graphics_fillRectangle(&circle1, &rect2);

                snprintf(buffer, BUFFER_SIZE, "RIP");
                Graphics_drawString(&context, (int8_t*) buffer, -1, 47, 65, true);
            }

            Happy_Graphics_Dec(happ, context);
            Energy_Graphics_Dec(enrg, context);
            if (!dead) Age_Graphics_Update(age, context);
            SWTimer_start(&timer);
        }
    }
}

/**
 * A helper function which increments a value with a maximum. If incrementing
 * the number causes the value to hit its maximum, the number wraps around
 * to 0.
 */
uint32_t CircularIncrement(uint32_t value, uint32_t maximum)
{
    return (value + 1) % maximum;
}

/**
 * The main constructor for your application. This function should initialize
 * each of the FSMs which implement the application logic of your project.
 *
 * @return a completely initialized Application object
 */
Application Application_construct()
{
    Application app;

    // Initialize local application state variables here!
    app.baudChoice = BAUD_9600;
    app.firstCall = true;

    return app;
}

   //@param app:  A pointer to the main Application object.
   //@param hal:  A pointer to the main HAL object
void Application_loop(Application* app, HAL* hal, Graphics_Context context, Graphics_Context circle1, Graphics_Context circle2, int happ, int enrg, bool dead)
{
    // Update communications if either this is the first time the application is
    // run.
    if (Button_isTapped(&hal->boosterpackS2) || app->firstCall) {
        Application_updateCommunications(app, hal, context);
    }

    // Interpret a new character if one is received.
    if (UART_hasChar(&hal->uart)) {
        Application_interpretIncomingChar(app, hal, context, circle1, circle2, happ, enrg, dead);
    }
}

/**
 * Updates which LEDs are lit and what baud rate the UART module communicates
 * with, based on what the application's baud choice is at the time this
 * function is called.
 *
 * @param app:  A pointer to the main Application object.
 * @param hal:  A pointer to the main HAL object
 */
void Application_updateCommunications(Application* app, HAL* hal, Graphics_Context context)
{
    // When this application first loops, the proper LEDs aren't lit. The
    // firstCall flag is used to ensure that the
    if (app->firstCall) {
        app->firstCall = false;
    }

    // When Boosterpack S2 is tapped, circularly increment which baud rate is used.
    if (Button_isTapped(&hal->boosterpackS2))
    {
        uint32_t newBaudNumber = CircularIncrement((uint32_t) app->baudChoice, NUM_BAUD_CHOICES);
        app->baudChoice = (UART_Baudrate) newBaudNumber;
    }

    // Update the baud rate according to the one set above.
    UART_updateBaud(&hal->uart, app->baudChoice);

    LED_turnOff(&hal->boosterpackRed);
    LED_turnOff(&hal->boosterpackGreen);
    LED_turnOff(&hal->boosterpackBlue);

    switch (app->baudChoice)
    {
        // When the baud rate is 9600, turn on Boosterpack LED Red
        case BAUD_9600:
            BR_Graphics_Update(0, context);
            break;

        // When the baud rate is 19200, turn on Boosterpack LED Green
        case BAUD_19200:
            BR_Graphics_Update(1, context);
            break;

        // When the baud rate is 38400, turn on Boosterpack LED Blue
        case BAUD_38400:
            BR_Graphics_Update(2, context);
            break;

        // When the baud rate is 57600, turn on all Boosterpack LEDs (illuminates white)
        case BAUD_57600:
            BR_Graphics_Update(3, context);
            break;

        // In the default case, this program will do nothing.
        default:
            break;
    }
}

/**
 * Interprets a character which was incoming and sends an interpreted character
 * back if the system USB UART module can accept a new character.
 *
 * @param app:  A pointer to the main Application object.
 * @param hal:  A pointer to the main HAL object
 */
void Application_interpretIncomingChar(Application* app, HAL* hal, Graphics_Context context, Graphics_Context circle1, Graphics_Context circle2, int happ, int enrg, bool dead)
{
    // The character received from your serial terminal
    char rxChar = UART_getChar(&hal->uart);

    if (!dead)
    {
        if ((rxChar == 'w' || rxChar == 'W') && (posCurrent[1] != 32 && enrg > 0))
        {
            Graphics_fillCircle(&circle2, posCurrent[0], posCurrent[1], 16);
            posCurrent[1] = 32;
            Graphics_fillCircle(&circle1, posCurrent[0], posCurrent[1], 16);
            moves++;
        }

        if ((rxChar == 'a' || rxChar == 'A') && (posCurrent[0] > 23 && enrg > 0))
        {
            Graphics_fillCircle(&circle2, posCurrent[0], posCurrent[1], 16);
            posCurrent[0] = posCurrent[0] - 40;
            Graphics_fillCircle(&circle1, posCurrent[0], posCurrent[1], 16);
            moves++;
        }

        if ((rxChar == 's' || rxChar == 'S') && (posCurrent[1] != 78 && enrg > 0))
        {
            Graphics_fillCircle(&circle2, posCurrent[0], posCurrent[1], 16);
            posCurrent[1] = 78;
            Graphics_fillCircle(&circle1, posCurrent[0], posCurrent[1], 16);
            moves++;
        }

        if ((rxChar == 'd' || rxChar == 'D') && (posCurrent[0] < 103 && enrg > 0))
        {
            Graphics_fillCircle(&circle2, posCurrent[0], posCurrent[1], 16);
            posCurrent[0] = posCurrent[0] + 40;
            Graphics_fillCircle(&circle1, posCurrent[0], posCurrent[1], 16);
            moves++;
        }

        if ((rxChar == 'f' || rxChar == 'F') && (enrg < 5))
        {
            Energy_Graphics_Inc(enrg+1, context);
            enrgInc = true;
        }

        if (moves >= 4)
        {
            if (happ < 5 && enrg > 0)
            {
                Happy_Graphics_Inc(happ+1, context);
                happInc = true;
                Energy_Graphics_Dec(enrg-1, context);
                enrgDec = true;
            }
            moves = 0;
        }
    }
}
