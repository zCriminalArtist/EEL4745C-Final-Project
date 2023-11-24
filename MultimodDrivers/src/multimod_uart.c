// multimod_uart.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for UART functions

/************************************Includes***************************************/

#include "../multimod_uart.h"

#include <stdint.h>
#include <stdbool.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>

#include <driverlib/uartstdio.h>
#include <driverlib/gpio.h>
#include <driverlib/uart.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>

/************************************Includes***************************************/

/********************************Public Functions***********************************/

// UART_Init
// Initializes UART serial communication with PC
// Return: void
void UART_Init() {
    // This should have been done in lab 0, so it's just copy & paste.
    // Enable port A
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // Enable UART0 module
	UARTEnable(UART0_BASE);
    // Configure UART0 pins on port A
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

    // Set UART clock source
	UARTClockSourceSet(UART0_BASE, UART_CLOCK_SYSTEM);
//	UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    // Configure UART baud rate
	UARTStdioConfig(0, 115200, SysCtlClockGet());
}

/********************************Public Functions***********************************/

