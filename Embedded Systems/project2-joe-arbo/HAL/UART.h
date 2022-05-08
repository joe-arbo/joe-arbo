/*
 * UART.h
 *
 *  Created on: Dec 31, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#ifndef HAL_UART_H_
#define HAL_UART_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// Ever since the new API update in mid 2019, the UART configuration struct's
// name changed from [eUSCI_UART_Config] to [eUSCI_UART_ConfigV1]. If your code
// does not compile, uncomment the alternate #define and comment the original
// one.
// -----------------------------------------------------------------------------
// #define UART_Config eUSCI_UART_Config
#define UART_Config eUSCI_UART_ConfigV1

// An enum outlining what baud rates the UART_construct() function can use in
// its initialization.
enum _UART_Baudrate
{
    BAUD_9600, BAUD_19200, BAUD_38400, BAUD_57600, NUM_BAUD_CHOICES
};
typedef enum _UART_Baudrate UART_Baudrate;

// TODO: Write an overview explanation of what this UART struct does, and how it
//       interacts with the functions below. Consult <HAL/Button.h>,
//       <HAL/LED.h>, and <HAL/Timer.h> for examples on how to do this.


/*
 * This struct initializes the moduleInstance (EUSCI_A0_BASE) the port (GPIO_PORT_P1)
 * and the pins (GPIO_PIN2 | GPIO_PIN3) to be used in UART.c. It also initializes the
 * config (eUSCI_UART_ConfigV1) so that the other struct variables can use it. It
 * serves as a way to use an HAL for what the noHAL version accomplished by using the
 * BASE, PORT, and PIN declarations directly.
 */
struct _UART
{
    UART_Config config;

    uint32_t moduleInstance;
    uint32_t port;
    uint32_t pins;
};
typedef struct _UART UART;

// Initializes a UART connection.
UART UART_construct(UART_Baudrate baudChoice);

// TODO: Write a comment which explains what each of these functions does. In the
//       header, prefer explaining WHAT the function does, as opposed to HOW it is
//       implemented.
char UART_getChar(UART* uart);
/*
 * The UART_getChar function receives and stores the char that the UART receives
 * from the user typing in the mobaExterm terminal. It then uses that char
 * to determine which character to print out in the terminal.
 */
bool UART_hasChar(UART* uart);
/*
 * The UART_hasChar function determines if the user has sent a UART data packet to the
 * board by checking the interrupt status of the proper system UART module.
 * It then returns a boolean true if the user did send a packet.
 */
bool UART_canSend(UART* uart);
/*
 * The UART_canSend function determines if the program can send the UART data packet
 * that was previously received and changed to the mobaExterm terminal. If the
 * function determines that it can send the packet, it returns true.
 */
void UART_putChar(UART* uart, char c);
/*
 * The UART_putChar function prints the determined character in the mobaExterm
 * terminal. It will only happen if UART_canSend returns true.
 */


// Updates the UART baudrate to use the new baud choice.
void UART_updateBaud(UART* uart, UART_Baudrate baudChoice);

#endif /* HAL_UART_H_ */
