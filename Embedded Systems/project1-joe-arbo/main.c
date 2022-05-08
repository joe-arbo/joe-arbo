/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>

/* Standard Includes */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Module Includes */
#include <LcdDriver/Crystalfontz128x128_ST7735.h>

#define BUFFER_SIZE 5
#define OFF 0x0000

/*Declare comparison functions*/
void LED1_comp(int right, int left);
void LED2_comp(int right, int left);
void BLED_comp(int right, int left);

/* Initializes a graphics context used for drawing things to the screen */
Graphics_Context InitGraphics()
{
    Graphics_Context context;

    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&context, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);

    Graphics_setFont(&context, &g_sFontCmss16b);

    Graphics_setForegroundColor(&context, GRAPHICS_COLOR_BLUE);
    Graphics_setBackgroundColor(&context, GRAPHICS_COLOR_WHITE);
    Graphics_clearDisplay(&context);

    return context;
}

void initialize()
{
    //Stop Watchdog timer
    WDT_A_holdTimer();

    /*Turns off all LEDs*/
    P1OUT = OFF;
    P2OUT = OFF;
    P5OUT = OFF;

    /*Initializes all Peripherals*/
    //LED1
    P1DIR |= BIT0;

    //LED2
    P2DIR |= (BIT0 | BIT1 | BIT2);

    //BLED
    P5DIR |= BIT6;
    P2DIR |= (BIT4 | BIT6);

    //Button L1
    P1DIR &= ~BIT1;
    P1REN |= BIT1;
    P1OUT |= BIT1;

    //Button L2
    P1DIR &= ~BIT4;
    P1REN |= BIT4;
    P1OUT |= BIT4;

    //Button B1
    P5DIR &= ~BIT1;
    P5REN |= BIT1;
    P5OUT |= BIT1;

    //Button B2
    P3DIR &= ~BIT5;
    P3REN |= BIT5;
    P3OUT |= BIT5;
}

int main(void)
{
    initialize();

    Graphics_Context context = InitGraphics();

    int left = 0;
    int right = 0;

    char buffer[BUFFER_SIZE];

    while (true)
    {
        snprintf(buffer, BUFFER_SIZE, "%02d", left);
        Graphics_drawString(&context, (int8_t*) buffer, -1, 30, 55, true);

        snprintf(buffer, BUFFER_SIZE, "%02d", right);
        Graphics_drawString(&context, (int8_t*) buffer, -1, 100, 55, true);

        snprintf(buffer, BUFFER_SIZE, "L:");
        Graphics_drawString(&context, (int8_t*) buffer, -1, 15, 55, true); //Adds "L:" to the screen

        snprintf(buffer, BUFFER_SIZE, "R:");
        Graphics_drawString(&context, (int8_t*) buffer, -1, 85, 55, true); //Adds "R:" to the screen


        //Increment and Decrement Left
        if (left > 0 && (P1IN & BIT1) == 0) left--;
        if (left < 40 && (P1IN & BIT4) == 0) left++;

        //Increment and Decrement Right
        if (right > 0 && (P3IN & BIT5) == 0) right--;
        if (right < 40 && (P5IN & BIT1) == 0) right++;

        //Test for LED1 condition using LED1_comp()
        LED1_comp(right, left);

        //Compare for BLED condition using BLED_comp()
        BLED_comp(right, left);

        //Compare for LED2 condition using LED2_comp()
        LED2_comp(right, left);
    }
}

/*Test for LED1 condition using diffA*/
void LED1_comp(int right, int left)
{
    int diffA = abs(right - left); //Magnitude of the difference

    if (diffA <= 10) P1OUT |= BIT0;
    else P1OUT &= ~BIT0;
}

/*Compare for LED2 condition using diffR*/
void LED2_comp(int right, int left)
{
    int diffR = right - left; //Difference with respect to right

    if (diffR >= 10 && diffR <= 20) P2OUT |= BIT0;
    else P2OUT &= ~BIT0;

    if (diffR >= 20 && diffR <= 30) P2OUT |= BIT1;
    else P2OUT &= ~BIT1;

    if (diffR >= 30 && diffR <= 40) P2OUT |= BIT2;
    else P2OUT &= ~BIT2;
}

/*Compare for BLED condition using diffL*/
void BLED_comp(int right, int left)
{
    int diffL = left - right; //Difference with respect to left

    if (diffL >= 10 && diffL <= 20) P2OUT |= BIT6;
    else P2OUT &= ~BIT6;

    if (diffL >= 20 && diffL <= 30) P2OUT |= BIT4;
    else P2OUT &= ~BIT4;

    if (diffL >= 30 && diffL <= 40) P5OUT |= BIT6;
    else P5OUT &= ~BIT6;
}

