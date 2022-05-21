/**
 * Starter code for Project 3. Good luck!
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include <Application.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* HAL and Application includes */
#include <HAL/HAL.h>
#include <HAL/Timer.h>
#include <HAL/LcdDriver/Crystalfontz128x128_ST7735.h>

#define BUFFER_SIZE 25
#define UP_THRESHOLD  0x3000
#define DOWN_THRESHOLD 0x1000
#define RIGHT_THRESHOLD 0x3000
#define LEFT_THRESHOLD  0x1000
#define FIGUREX 1
#define FIGUREY 1

// Non-blocking check. Whenever Launchpad S1 is pressed, LED1 turns on.
static void InitNonBlockingLED()
{
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
}

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

    Graphics_setFont(&context, &g_sFontCmss14b);

    Graphics_setForegroundColor(&context, GRAPHICS_COLOR_WHITE);
    Graphics_setBackgroundColor(&context, GRAPHICS_COLOR_DARK_RED);
    Graphics_clearDisplay(&context);

    return context;
}


Graphics_Context ImageGraphics()
{
    Graphics_Context context;

    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&context, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);

    Graphics_setFont(&context, &g_sFontCmss14b);

    Graphics_setForegroundColor(&context, GRAPHICS_COLOR_WHITE);
    Graphics_setBackgroundColor(&context, GRAPHICS_COLOR_WHITE);
    Graphics_clearDisplay(&context);

    return context;
}


Graphics_Context CircleGraphics()
{
    Graphics_Context context;

    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&context, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);

    Graphics_setFont(&context, &g_sFontFixed6x8);

    Graphics_setForegroundColor(&context, GRAPHICS_COLOR_WHITE);
    Graphics_setBackgroundColor(&context, GRAPHICS_COLOR_DARK_RED);
    Graphics_clearDisplay(&context);

    return context;
}

Graphics_Context Circle2Graphics()
{
    Graphics_Context context;

    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&context, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);

    Graphics_setFont(&context, &g_sFontFixed6x8);

    Graphics_setForegroundColor(&context, GRAPHICS_COLOR_DARK_RED);
    Graphics_setBackgroundColor(&context, GRAPHICS_COLOR_DARK_RED);
    Graphics_clearDisplay(&context);

    return context;
}


Graphics_Context ShieldGraphics()
{
    Graphics_Context context;

    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&context, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);

    Graphics_setFont(&context, &g_sFontFixed6x8);

    Graphics_setForegroundColor(&context, GRAPHICS_COLOR_GREEN);
    Graphics_setBackgroundColor(&context, GRAPHICS_COLOR_DARK_RED);
    Graphics_clearDisplay(&context);

    return context;
}


void drawImage(Graphics_Context *g_sContext_p){
    uint32_t xpos;
    uint32_t ypos;
    extern const tImage triangle4BPP_UNCOMP;

    xpos = FIGUREX;
    ypos = FIGUREY;

    Graphics_drawImage(g_sContext_p, &triangle4BPP_UNCOMP, xpos, ypos);
}

//Initializes the screens output
void Graphics_Initialize(Graphics_Context context, Graphics_Context image)
{
    char buffer[BUFFER_SIZE];

    Graphics_clearDisplay(&image);
    Graphics_clearDisplay(&context);

    snprintf(buffer, BUFFER_SIZE, "Main Menu");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 1, true);

    snprintf(buffer, BUFFER_SIZE, "---------------------------");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 12, true);

    snprintf(buffer, BUFFER_SIZE, "Play Game");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 25, 30, true);

    snprintf(buffer, BUFFER_SIZE, "How to Play");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 25, 45, true);

    snprintf(buffer, BUFFER_SIZE, "High Scores");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 25, 60, true);

    snprintf(buffer, BUFFER_SIZE, "-");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 15, 30, true);
}

void updateCursor(Graphics_Context context, Application* app)
{
    char buffer[BUFFER_SIZE];
    int pos1, pos2, pos3 = 0;

    switch(app->screen)
    {
    case PLAY:
        pos1 = 30;
        pos2 = 60;
        pos3 = 45;
        break;

    case RULES:
        pos1 = 45;
        pos2 = 30;
        pos3 = 60;
        break;

    case SCORES:
        pos1 = 60;
        pos2 = 45;
        pos3 = 30;
        break;
    }

    snprintf(buffer, BUFFER_SIZE, " ");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 15, pos2, true);

    snprintf(buffer, BUFFER_SIZE, " ");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 15, pos3, true);

    snprintf(buffer, BUFFER_SIZE, "-");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 15, pos1, true);
}


void playScreenRedraw(Application* app, Graphics_Context context, Graphics_Context circle, Graphics_Context circle2)
{
    char buffer[BUFFER_SIZE];

    snprintf(buffer, BUFFER_SIZE, "Avatar vs. Dangers");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 1, true);

    snprintf(buffer, BUFFER_SIZE, "---------------------------");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 12, true);

    snprintf(buffer, BUFFER_SIZE, "H:");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 115, true);

    snprintf(buffer, BUFFER_SIZE, "%01d", app->healthPoints);
    Graphics_drawString(&context, (int8_t*) buffer, -1, 15, 115, true);

    snprintf(buffer, BUFFER_SIZE, "D:");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 28, 115, true);

    snprintf(buffer, BUFFER_SIZE, "%01d", app->diff);
    Graphics_drawString(&context, (int8_t*) buffer, -1, 43, 115, true);

    snprintf(buffer, BUFFER_SIZE, "S:");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 56, 115, true);

    snprintf(buffer, BUFFER_SIZE, "%03d", app->score);
    Graphics_drawString(&context, (int8_t*) buffer, -1, 68, 115, true);

    snprintf(buffer, BUFFER_SIZE, "P:");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 95, 115, true);

    snprintf(buffer, BUFFER_SIZE, "%02d", app->shieldPoints);
    Graphics_drawString(&context, (int8_t*) buffer, -1, 110, 115, true);

    Graphics_Rectangle rect;
    rect.xMax = 119;
    rect.xMin = 9;
    rect.yMax = 112;
    rect.yMin = 22;
    Graphics_drawRectangle(&context, &rect);

    Graphics_fillCircle(&circle, app->xpos, app->ypos, 7);

    if(app->canDrawShield == false)
    {
        Graphics_setForegroundColor(&circle2, GRAPHICS_COLOR_SLATE_BLUE);
        Graphics_fillCircle(&circle2, app->shieldPosX, app->shieldPosY, 7);
        Graphics_setForegroundColor(&circle2, GRAPHICS_COLOR_DARK_RED);
    }
}


void playScreen(Graphics_Context context, Graphics_Context circle)
{
    Graphics_clearDisplay(&context);

    char buffer[BUFFER_SIZE];

    snprintf(buffer, BUFFER_SIZE, "Avatar vs. Dangers");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 1, true);

    snprintf(buffer, BUFFER_SIZE, "---------------------------");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 12, true);

    snprintf(buffer, BUFFER_SIZE, "H:");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 115, true);

    snprintf(buffer, BUFFER_SIZE, "%01d", 3);
    Graphics_drawString(&context, (int8_t*) buffer, -1, 15, 115, true);

    snprintf(buffer, BUFFER_SIZE, "D:");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 28, 115, true);

    snprintf(buffer, BUFFER_SIZE, "%01d", 0);
    Graphics_drawString(&context, (int8_t*) buffer, -1, 43, 115, true);

    snprintf(buffer, BUFFER_SIZE, "S:");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 56, 115, true);

    snprintf(buffer, BUFFER_SIZE, "%03d", 0);
    Graphics_drawString(&context, (int8_t*) buffer, -1, 68, 115, true);

    snprintf(buffer, BUFFER_SIZE, "P:");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 95, 115, true);

    snprintf(buffer, BUFFER_SIZE, "%02d", 0);
    Graphics_drawString(&context, (int8_t*) buffer, -1, 110, 115, true);

    Graphics_Rectangle rect;
    rect.xMax = 119;
    rect.xMin = 9;
    rect.yMax = 112;
    rect.yMin = 22;
    Graphics_drawRectangle(&context, &rect);

    Graphics_fillCircle(&circle, 64, 67, 7);
}

void spawnShield(Application* app, Graphics_Context circle)
{
    Graphics_setForegroundColor(&circle, GRAPHICS_COLOR_SLATE_BLUE);

    int randx = (rand() % 93) + 18;
    int randy = (rand() % 73) + 31;

    Graphics_fillCircle(&circle, randx, randy, 7);

    app->shieldPosX = randx;
    app->shieldPosY = randy;

    Graphics_setForegroundColor(&circle, GRAPHICS_COLOR_DARK_RED);
}


void spawnDanger(Application* app, Graphics_Context circle)
{
    Graphics_setForegroundColor(&circle, GRAPHICS_COLOR_RED);

    int randx = (rand() % 93) + 18;
    int randy = (rand() % 73) + 31;

    Graphics_fillCircle(&circle, randx, randy, 7);

    app->dangerPosX = randx;
    app->dangerPosY = randy;

    Graphics_setForegroundColor(&circle, GRAPHICS_COLOR_DARK_RED);
}


void rulesScreen(Graphics_Context context)
{
    Graphics_clearDisplay(&context);

    char buffer[BUFFER_SIZE];

    Graphics_setFont(&context, &g_sFontFixed6x8);

    snprintf(buffer, BUFFER_SIZE, "How to Play");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 1, true);

    snprintf(buffer, BUFFER_SIZE, "---------------------------");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 10, true);

    snprintf(buffer, BUFFER_SIZE, "Avoid the RED dangers.");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 20, true);

    snprintf(buffer, BUFFER_SIZE, "Collect the BLUE");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 30, true);

    snprintf(buffer, BUFFER_SIZE, "Shields. Spend Shield");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 40, true);

    snprintf(buffer, BUFFER_SIZE, "points by pressing B2.");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 50, true);

    snprintf(buffer, BUFFER_SIZE, "to get a shield that");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 60, true);

    snprintf(buffer, BUFFER_SIZE, "destroys the RED");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 70, true);

    snprintf(buffer, BUFFER_SIZE, "dangers. Earn Points");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 80, true);

    snprintf(buffer, BUFFER_SIZE, "by destroying the");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 90, true);

    snprintf(buffer, BUFFER_SIZE, "RED dangers.");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 100, true);

    snprintf(buffer, BUFFER_SIZE, "To Main Menu: JS");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 10, 112, true);
}

void scoresScreen(Application* app, Graphics_Context context)
{
    Graphics_clearDisplay(&context);

    char buffer[BUFFER_SIZE];
    int i;
    int n = 0;

    snprintf(buffer, BUFFER_SIZE, "High Scores");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 1, true);

    snprintf(buffer, BUFFER_SIZE, "---------------------------");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 14, true);

    for(i = 0; i < 5; i++)
    {
        snprintf(buffer, BUFFER_SIZE, "Score %d: %d", i+1, app->highScores[i]);
        Graphics_drawString(&context, (int8_t*) buffer, -1, 10, 30 + n, true);
        n = n + 15;
    }

    snprintf(buffer, BUFFER_SIZE, "To Main Menu: JS");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 8, 112, true);
}


void deathScreen(Application* app, Graphics_Context context, Graphics_Context circle)
{
    Graphics_clearDisplay(&context);
    Graphics_clearDisplay(&circle);
    char buffer[BUFFER_SIZE];
    int i, k, temp;

    app->highScores[app->deaths] = app->score;

    for(i = 0; i < 5; i++)
    {
        for(k = i + 1; k < 5; k++)
        {
            if(app->highScores[i] < app->highScores[k])
            {
                temp = app->highScores[i];
                app->highScores[i] = app->highScores[k];
                app->highScores[k] = temp;
            }
        }
    }

    app->deaths++;

    snprintf(buffer, BUFFER_SIZE, "Game Over");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 1, true);

    snprintf(buffer, BUFFER_SIZE, "---------------------------");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 1, 14, true);

    snprintf(buffer, BUFFER_SIZE, "Score: %d", app->score);
    Graphics_drawString(&context, (int8_t*) buffer, -1, 10, 30, true);

    snprintf(buffer, BUFFER_SIZE, "To Main Menu: JS");
    Graphics_drawString(&context, (int8_t*) buffer, -1, 8, 112, true);

    Application_reset(app);
}


void initADC()
{
    ADC14_enableModule();

    ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC,
                     ADC_PREDIVIDER_1,
                     ADC_DIVIDER_1,
                      0
                     );

    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, true);

    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
}

void startADC()
{
   ADC14_enableConversion();
   ADC14_toggleConversionTrigger();
}

void initJoyStick()
{
    ADC14_configureConversionMemory(ADC_MEM0,
                                  ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                  ADC_INPUT_A15,                 // joystick X
                                  ADC_NONDIFFERENTIAL_INPUTS);

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
                                               GPIO_PIN0,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    ADC14_configureConversionMemory(ADC_MEM1,
                                    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A9,                 // joystick Y
                                    ADC_NONDIFFERENTIAL_INPUTS);

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
                                               GPIO_PIN4,
                                               GPIO_TERTIARY_MODULE_FUNCTION);
}

void getSampleJoyStick(unsigned *X, unsigned *Y) {
    *X = ADC14_getResult(ADC_MEM0);
    *Y = ADC14_getResult(ADC_MEM1);
}


void delay(int x)
{
    int c, d;
    for (c = 1; c <= x; c++)
       for (d = 1; d <= x; d++){}
}

int main()
{
    WDT_A_holdTimer();

    InitSystemTiming();

    // Initialize the main Application object and HAL object
    HAL hal = HAL_construct();
    Application app = Application_construct();

    Graphics_Context context = InitGraphics();
    Graphics_Context image = ImageGraphics();
    Graphics_Context circle = CircleGraphics();
    Graphics_Context circle2 = Circle2Graphics();
    Graphics_Context shieldVoid = Circle2Graphics();
    Graphics_Context greenShield = ShieldGraphics();


    drawImage(&image);
    delay(1500);
    Graphics_Initialize(context, image);

    // Do not remove this line. This is your non-blocking check.
    InitNonBlockingLED();

    SWTimer shieldTimer = SWTimer_construct(1500);
    SWTimer shieldSpawnTimer = SWTimer_construct(3000);
    SWTimer_start(&shieldSpawnTimer);

    SWTimer dangerSpawnTimer = SWTimer_construct(3000);
    SWTimer_start(&dangerSpawnTimer);

    unsigned vx, vy;
    initADC();
    initJoyStick();
    startADC();

    while (1)
    {
        // Do not remove this line. This is your non-blocking check.
        PollNonBlockingLED();

        getSampleJoyStick(&vx, &vy);

        //booleans to record movements
        bool joyStickPushedtoUp = false;
        bool joyStickPushedtoDown = false;
        bool joyStickPushedtoRight = false;
        bool joyStickPushedtoLeft = false;

        int Vx = (int)vx - 7800;
        int Vy = (int)vy - 8400;

        //conditionals to ensure movement in the given direction
        if (vx > RIGHT_THRESHOLD) joyStickPushedtoRight = true;
        if (vx < LEFT_THRESHOLD) joyStickPushedtoLeft = true;
        if(vy < DOWN_THRESHOLD) joyStickPushedtoDown = true;
        if(vy > UP_THRESHOLD) joyStickPushedtoUp = true;

        Application_loop(&app, &hal, context, image, circle, circle2, shieldVoid, greenShield, joyStickPushedtoDown,
                         joyStickPushedtoUp, joyStickPushedtoRight, joyStickPushedtoLeft, Vx, Vy, &shieldTimer, &shieldSpawnTimer, &dangerSpawnTimer);
        HAL_refresh(&hal);
    }
}


uint32_t CircularIncrement(uint32_t value, uint32_t maximum)
{
    return (value + 1) % maximum;
}

uint32_t RevCircularIncrement(uint32_t value, uint32_t maximum)
{
    return (value + (maximum - 1)) % maximum;
}

Application Application_construct()
{
    Application app;
    int i;

    app.baudChoice = BAUD_9600;
    app.MainScreen = true;
    app.canRedraw = true;
    app.shieldStart = false;
    app.canDrawShield = true;
    app.canDrawDanger = true;
    app.screen = PLAY;
    app.diff = BASE;
    app.xpos = 64;
    app.ypos = 67;
    app.maxSpeed = 0.5;
    app.shieldPosX = 0;
    app.shieldPosY = 0;
    app.shieldPoints = 0;
    app.dangerPosX = 0;
    app.dangerPosY = 0;
    app.healthPoints = 3;
    app.score = 0;
    app.deaths = 0;
    for(i = 0; i < 5; i++) app.highScores[i] = 0;

    return app;
}


void Application_reset(Application* app)
{
    app->canRedraw = true;
    app->shieldStart = false;
    app->canDrawShield = true;
    app->canDrawDanger = true;
    app->diff = BASE;
    app->xpos = 64;
    app->ypos = 67;
    app->maxSpeed = 0.5;
    app->shieldPosX = 0;
    app->shieldPosY = 0;
    app->shieldPoints = 0;
    app->dangerPosX = 0;
    app->dangerPosY = 0;
    app->healthPoints = 3;
    app->score = 0;
}


void Application_loop(Application* app, HAL* hal, Graphics_Context context, Graphics_Context image, Graphics_Context circle, Graphics_Context circle2,
                      Graphics_Context shieldVoid, Graphics_Context greenShield, bool joyStickPushedtoDown, bool joyStickPushedtoUp,
                      bool joyStickPushedtoRight, bool joyStickPushedtoLeft, int Vx, int Vy, SWTimer* shieldTimer, SWTimer* shieldSpawnTimer, SWTimer* dangerSpawnTimer)
{
    char buffer[BUFFER_SIZE];

    int boundry = 500;
    double ySpeed = abs(Vy)*(app->maxSpeed/10000);
    double xSpeed = abs(Vx)*(app->maxSpeed/10000);

    if (joyStickPushedtoDown && app->MainScreen && app->canRedraw) //Operates Menu
    {
        app->canRedraw = false;
        uint32_t newScreen = CircularIncrement((uint32_t) app->screen, NUM_SCREENS);
        app->screen = (screenChoice) newScreen;

        updateCursor(context, app);
    }
    else if (joyStickPushedtoUp && app->MainScreen && app->canRedraw) //Operates Menu
    {
        app->canRedraw = false;
        uint32_t newScreen = RevCircularIncrement((uint32_t) app->screen, NUM_SCREENS);
        app->screen = (screenChoice) newScreen;

        updateCursor(context, app);
    }

    if (!joyStickPushedtoUp && !joyStickPushedtoDown && app->MainScreen && app->canRedraw == false) app->canRedraw = true;

    if (Button_isTapped(&hal->boosterpackJS) && app->MainScreen == false && app->screen != PLAY) //Returns to Main Menu
    {
        app->MainScreen = true;
        Graphics_Initialize(context, image);
        if(app->screen == DEATH) app->screen = PLAY;
        updateCursor(context, app);
    }
    else if (Button_isTapped(&hal->boosterpackJS) && app->MainScreen) Application_updateScreen(app, hal, context, circle); //Updates Screen

    //Play Loop
    if(app->MainScreen == false && app->screen == PLAY) //If playing the game
    {
        if(Button_isTapped(&hal->boosterpackS1)) //changes difficulty
        {
            int32_t newDiff = CircularIncrement((uint32_t) app->diff, NUM_DIFF);
            app->diff = (difficulty) newDiff;

            snprintf(buffer, BUFFER_SIZE, "%01d", (int)newDiff);
            Graphics_drawString(&context, (int8_t*) buffer, -1, 43, 115, true);
        }

        if(Button_isTapped(&hal->boosterpackS2) && app->shieldPoints > 0) //Activates shield
        {
            SWTimer_start(shieldTimer);
            app->shieldStart = true;
            app->maxSpeed = 2.5;
            Graphics_drawCircle(&greenShield, app->xpos, app->ypos, 25);

            app->shieldPoints--;
            snprintf(buffer, BUFFER_SIZE, "%02d", app->shieldPoints);
            Graphics_drawString(&context, (int8_t*) buffer, -1, 110, 115, true);
        }


        if(SWTimer_expired(shieldTimer) && app->shieldStart) //Deactivates shield
        {
            app->shieldStart = false;
            app->maxSpeed = 0.5;
            Graphics_drawCircle(&shieldVoid, app->xpos, app->ypos, 25);
            playScreenRedraw(app, context, circle, circle2);
        }

        if(app->healthPoints == 2) //changes color of the player based on health
        {
            Graphics_setForegroundColor(&circle, GRAPHICS_COLOR_LIGHT_GREEN);
            Graphics_fillCircle(&circle, app->xpos, app->ypos, 7);
            if(app->shieldStart == false) app->maxSpeed = 0.75;
        }
        if(app->healthPoints == 1)
        {
            Graphics_setForegroundColor(&circle, GRAPHICS_COLOR_YELLOW);
            Graphics_fillCircle(&circle, app->xpos, app->ypos, 7);
            if(app->shieldStart == false) app->maxSpeed = 0.75;
        }
        if(app->healthPoints == 0)
        {
            Graphics_setForegroundColor(&circle, GRAPHICS_COLOR_BLACK);
            Graphics_fillCircle(&circle, app->xpos, app->ypos, 7);
            if(app->shieldStart == false) app->maxSpeed = 0.75;
        }

        if(Vy > boundry) //Determines movement
        {
            if(Vx > boundry)
            {
                Graphics_fillCircle(&circle2, app->xpos, app->ypos, 7);
                if(app->shieldStart) Graphics_drawCircle(&shieldVoid, app->xpos, app->ypos, 25);
                if(app->ypos > 31)  app->ypos = (app->ypos) - ySpeed;
                if(app->xpos < 110) app->xpos = (app->xpos) + xSpeed;
                if(app->shieldStart) Graphics_drawCircle(&greenShield, app->xpos, app->ypos, 25);
                Graphics_fillCircle(&circle, app->xpos, app->ypos, 7);
            }
            else if (Vx < -boundry)
            {
                Graphics_fillCircle(&circle2, app->xpos, app->ypos, 7);
                if(app->shieldStart) Graphics_drawCircle(&shieldVoid, app->xpos, app->ypos, 25);
                if(app->ypos > 31) app->ypos = (app->ypos) - ySpeed;
                if(app->xpos > 18) app->xpos = (app->xpos) - xSpeed;
                if(app->shieldStart) Graphics_drawCircle(&greenShield, app->xpos, app->ypos, 25);
                Graphics_fillCircle(&circle, app->xpos, app->ypos, 7);
            }
            else if (Vx <= boundry && Vx >= -boundry)
            {
                Graphics_fillCircle(&circle2, app->xpos, app->ypos, 7);
                if(app->shieldStart) Graphics_drawCircle(&shieldVoid, app->xpos, app->ypos, 25);
                if(app->ypos > 31) app->ypos = (app->ypos) - ySpeed;
                if(app->shieldStart) Graphics_drawCircle(&greenShield, app->xpos, app->ypos, 25);
                Graphics_fillCircle(&circle, app->xpos, app->ypos, 7);
            }
        }
        else if (Vy < -boundry)
        {
            if(Vx > boundry)
            {
                Graphics_fillCircle(&circle2, app->xpos, app->ypos, 7);
                if(app->shieldStart) Graphics_drawCircle(&shieldVoid, app->xpos, app->ypos, 25);
                if(app->ypos < 103) app->ypos = (app->ypos) + ySpeed;
                if(app->xpos < 110) app->xpos = (app->xpos) + xSpeed;
                if(app->shieldStart) Graphics_drawCircle(&greenShield, app->xpos, app->ypos, 25);
                Graphics_fillCircle(&circle, app->xpos, app->ypos, 7);
            }
            else if (Vx < -boundry)
            {
                Graphics_fillCircle(&circle2, app->xpos, app->ypos, 7);
                if(app->shieldStart) Graphics_drawCircle(&shieldVoid, app->xpos, app->ypos, 25);
                if(app->ypos < 103) app->ypos = (app->ypos) + ySpeed;
                if(app->xpos > 18)  app->xpos = (app->xpos) - xSpeed;
                if(app->shieldStart) Graphics_drawCircle(&greenShield, app->xpos, app->ypos, 25);
                Graphics_fillCircle(&circle, app->xpos, app->ypos, 7);
            }
            else if (Vx <= boundry && Vx >= -boundry)
            {
                Graphics_fillCircle(&circle2, app->xpos, app->ypos, 7);
                if(app->shieldStart) Graphics_drawCircle(&shieldVoid, app->xpos, app->ypos, 25);
                if(app->ypos < 103) app->ypos = (app->ypos) + ySpeed;
                if(app->shieldStart) Graphics_drawCircle(&greenShield, app->xpos, app->ypos, 25);
                Graphics_fillCircle(&circle, app->xpos, app->ypos, 7);
            }
        }
        else if (Vy <= boundry && Vy >= -boundry)
        {
            if(Vx > boundry)
            {
                Graphics_fillCircle(&circle2, app->xpos, app->ypos, 7);
                if(app->shieldStart) Graphics_drawCircle(&shieldVoid, app->xpos, app->ypos, 25);
                if(app->xpos < 110) app->xpos = (app->xpos) + xSpeed;
                if(app->shieldStart) Graphics_drawCircle(&greenShield, app->xpos, app->ypos, 25);
                Graphics_fillCircle(&circle, app->xpos, app->ypos, 7);
            }
            else if (Vx < -boundry)
            {
                Graphics_fillCircle(&circle2, app->xpos, app->ypos, 7);
                if(app->shieldStart) Graphics_drawCircle(&shieldVoid, app->xpos, app->ypos, 25);
                if(app->xpos > 18)  app->xpos = (app->xpos) - xSpeed;
                if(app->shieldStart) Graphics_drawCircle(&greenShield, app->xpos, app->ypos, 25);
                Graphics_fillCircle(&circle, app->xpos, app->ypos, 7);
            }
        }

        //Shield Drop
        if(app->canDrawShield && SWTimer_expired(shieldSpawnTimer))
        {
            spawnShield(app, circle2);
            app->canDrawShield = false;
        }

        int minXpos_shield = app->shieldPosX - 7;
        int maxXpos_shield = app->shieldPosX + 7;
        int minYpos_shield = app->shieldPosY - 7;
        int maxYpos_shield = app->shieldPosY + 7;

        //Tests collision with shield
        if((((app->xpos + 7) >= minXpos_shield && (app->xpos + 7) <= maxXpos_shield) || ((app->xpos - 7) <= maxXpos_shield && (app->xpos - 7) >= minXpos_shield)) && app->canDrawShield == false
                && (((app->ypos + 7) >= minYpos_shield && (app->ypos + 7) <= maxYpos_shield) || ((app->ypos - 7) <= maxYpos_shield && (app->ypos - 7) >= minYpos_shield)))
        {
            Graphics_fillCircle(&circle2, app->shieldPosX, app->shieldPosY, 7);

            app->shieldPoints++;

            snprintf(buffer, BUFFER_SIZE, "%02d", app->shieldPoints);
            Graphics_drawString(&context, (int8_t*) buffer, -1, 110, 115, true);

            SWTimer_start(shieldSpawnTimer);

            app->canDrawShield = true;
        }

        //Danger Drop
        if(app->canDrawDanger && SWTimer_expired(dangerSpawnTimer))
        {
            spawnDanger(app, circle2);
            app->canDrawDanger = false;
        }

        int minXpos_danger = app->dangerPosX - 7;
        int maxXpos_danger = app->dangerPosX + 7;
        int minYpos_danger = app->dangerPosY - 7;
        int maxYpos_danger = app->dangerPosY + 7;

        //Tests collision with danger
        if((((app->xpos + 7) >= minXpos_danger && (app->xpos + 7) <= maxXpos_danger) || ((app->xpos - 7) <= maxXpos_danger && (app->xpos - 7) >= minXpos_danger)) && app->canDrawDanger == false
                && (((app->ypos + 7) >= minYpos_danger && (app->ypos + 7) <= maxYpos_danger) || ((app->ypos - 7) <= maxYpos_danger && (app->ypos - 7) >= minYpos_danger)))
        {
            Graphics_fillCircle(&circle2, app->dangerPosX, app->dangerPosY, 7);

            app->healthPoints--;
            snprintf(buffer, BUFFER_SIZE, "%01d", app->healthPoints);
            Graphics_drawString(&context, (int8_t*) buffer, -1, 15, 115, true);

            SWTimer_start(dangerSpawnTimer);
            app->canDrawDanger = true;
        }

        //Tests collision with danger from green shield
        if((((maxYpos_danger <= (app->ypos + 25) && minYpos_danger >= (app->ypos - 25)) && ((maxXpos_danger > (app->xpos - 25) && minXpos_danger < (app->xpos - 25))
                || (maxXpos_danger > (app->xpos + 25) && minXpos_danger < (app->xpos + 25)) || (maxXpos_danger <= (app->xpos + 25) && minXpos_danger >= (app->xpos - 25))))
                || ((maxXpos_danger <= (app->xpos + 25) && minXpos_danger >= (app->xpos - 25)) && ((maxYpos_danger > (app->ypos - 25) && minYpos_danger < (app->ypos - 25))
                || (maxYpos_danger > (app->ypos + 25) && minYpos_danger < (app->ypos + 25))))) && app->canDrawDanger == false && app->shieldStart)
        {
            Graphics_fillCircle(&circle2, app->dangerPosX, app->dangerPosY, 7);

            app->score++;
            snprintf(buffer, BUFFER_SIZE, "%03d", app->score);
            Graphics_drawString(&context, (int8_t*) buffer, -1, 68, 115, true);

            SWTimer_start(dangerSpawnTimer);
            app->canDrawDanger = true;
        }

        if(app->healthPoints == 0)
        {
            app->screen = DEATH;
            Application_updateScreen(app, hal, context, circle);
        }

    }
}


void Application_updateScreen(Application* app, HAL* hal, Graphics_Context context, Graphics_Context circle)
{
    switch(app->screen)
    {
    case PLAY:
        app->MainScreen = false;
        playScreen(context, circle);
        break;

    case RULES:
        app->MainScreen = false;
        rulesScreen(context);
        break;

    case SCORES:
        app->MainScreen = false;
        scoresScreen(app, context);
        break;

    case DEATH:
        deathScreen(app, context, circle);
        break;
    }
}
