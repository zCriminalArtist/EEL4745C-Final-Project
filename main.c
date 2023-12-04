// Lab 4, uP2 Fall 2023
// Created: 2023-07-31
// Updated: 2023-08-01
// Lab 4 is intended to introduce you to more advanced RTOS concepts. In this, you will
// - implement blocking, yielding, sleeping
// - Thread priorities, aperiodic & periodic threads
// - IPC using FIFOs
// - Dynamic thread creation & deletion

/************************************Includes***************************************/

#include "G8RTOS/G8RTOS.h"
#include "./MultimodDrivers/multimod.h"
#include <driverlib/timer.h>

#include "./threads.h"

#include "./sprites/bird.h"

/************************************Includes***************************************/
/***********************************Semaphores**************************************/

semaphore_t sem_UART;

/***********************************Semaphores**************************************/

/********************************Public Functions***********************************/

extern uint32_t SystemTime;
/********************************Public Functions***********************************/


/************************************MAIN*******************************************/
int main(void) {
    // Sets clock speed to 80 MHz. You'll need it!
	SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    G8RTOS_Init();
    multimod_init();

    ST7789_drawImage(100, 50, 17, 12, bird, 6);



    G8RTOS_Launch();
    while (1);
}

/************************************MAIN*******************************************/
