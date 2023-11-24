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

void task2();

void task0() {
	int counter0 = 0;
	while(1) {
		counter0 += 1;
		G8RTOS_WaitSemaphore(&sem_UART);
		G8RTOS_WriteFIFO(0, counter0);
		UARTprintf("Wrote '%d' to FIFO\n", counter0);
		G8RTOS_SignalSemaphore(&sem_UART);
		G8RTOS_KillSelf();
	}
}

void task1() {
	while(1) {
		G8RTOS_WaitSemaphore(&sem_UART);
		UARTprintf("Task 0 counter is at: %d\n", G8RTOS_ReadFIFO(0));
		G8RTOS_SignalSemaphore(&sem_UART);
		sleep(1000);
		G8RTOS_AddThread(task2, 0, "task2");
		G8RTOS_KillSelf();
	}
}

void task2() {
	int counter2 = 0;
	while(1) {
		counter2 += 1;
		G8RTOS_WaitSemaphore(&sem_UART);
		UARTprintf("Task 2 counter is at: %d\n", counter2);
		G8RTOS_SignalSemaphore(&sem_UART);
		sleep(2000);
		G8RTOS_AddThread(task0, 0, "task0");
		G8RTOS_AddThread(task1, 1, "task1");
		G8RTOS_KillSelf();
	}
}

void task3() {
	G8RTOS_WaitSemaphore(&sem_UART);
	UARTprintf("Periodic thread\n");
	G8RTOS_SignalSemaphore(&sem_UART);
}

void GPIOF_Handler() {
	uint8_t b = MultimodButtons_Get();
	UARTprintf("Buttons: %d\n", b);
	GPIOIntClear(GPIO_PORTE_BASE, GPIO_PIN_4);
}
/********************************Public Functions***********************************/


/************************************MAIN*******************************************/
int main(void) {
    // Sets clock speed to 80 MHz. You'll need it!
	SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    G8RTOS_Init();
    multimod_init();

//    GPIODirModeSet(GPIO_PORTF_BASE,  GPIO_PIN_4, GPIO_DIR_MODE_IN);
//    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
//           GPIOIntDisable(GPIO_PORTF_BASE,GPIO_PIN_4);         // Disable interrupt for PC4,PC5 and PC6(in case it was enabled)
//           GPIOIntClear(GPIO_PORTF_BASE,GPIO_PIN_4);           // Clear pending interrupts for PC4,PC5 and PC6
//           GPIOIntTypeSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_FALLING_EDGE );// Configure PC4,PC5 and PC6 for falling edge trigger
//           GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4); // Enable interrupt for  PC4,PC5 and PC6

//    while (1) {
//    	uint16_t x = JOYSTICK_GetX();
//    	uint16_t y = JOYSTICK_GetY();
//    	UARTprintf("X: %d\n", x);
//    	UARTprintf("Y: %d\n", y);
//    	SysCtlDelay(80000000);
//    }

//    ST7789_DrawRectangle(100, 50, 50, 100, ((31 << 11) | (0 << 5) | 0));

    ST7789_drawImage(100, 50, 17, 12, bird, 6);

    // Add threads, semaphores, here
//    G8RTOS_InitSemaphore(&sem_SPIA, 1);
//    G8RTOS_InitSemaphore(&sem_PCA9555_Debounce, 0);
//    G8RTOS_InitFIFO(RESET_FIFO);
//    G8RTOS_InitFIFO(JOYSTICK_FIFO);
//    G8RTOS_InitFIFO(SPAWNCOOR_FIFO);
//
//    G8RTOS_AddThread(Spaceship_Thread, 0, "ship");
//    G8RTOS_AddThread(SpaceshipMove_Thread, 0, "move");
//
//    G8RTOS_AddThread(Enemy_Thread, 0, "1");
//    G8RTOS_AddThread(Enemy_Thread, 0, "2");
//    G8RTOS_AddThread(Enemy_Thread, 0, "3");
//    G8RTOS_AddThread(Enemy_Thread, 0, "4");
//    G8RTOS_AddThread(Enemy_Thread, 0, "5");
//
//    G8RTOS_Add_PeriodicEvent(Get_Joystick, 0, 100);
//
//    G8RTOS_Add_APeriodicEvent(GPIOE_Handler, 0, INT_GPIOE);
//    G8RTOS_Add_APeriodicEvent(GPIOD_Handler, 0, INT_GPIOD);
//
//    G8RTOS_AddThread(Read_Buttons, 0, "but");

//    G8RTOS_Add_APeriodicEvent(GPIOF_Handler, 0, INT_GPIOE);
//    G8RTOS_InitFIFO(0);
//    G8RTOS_InitSemaphore(&sem_UART, 1);
//    G8RTOS_AddThread(task2, 0, "task");
//    G8RTOS_Add_PeriodicEvent(task3, 2000, 500);
//
//    G8RTOS_InitSemaphore(&sem_KillCube, 1);
//    G8RTOS_InitSemaphore(&sem_PCA9555_Debounce, 0);
//    G8RTOS_InitSemaphore(&sem_PCA9555_Debounce, 0);
//    G8RTOS_InitSemaphore(&sem_SPIA, 1);
//
//    G8RTOS_InitFIFO(SPAWNCOOR_FIFO);
//    G8RTOS_InitFIFO(JOYSTICK_FIFO);
//
//    G8RTOS_AddThread(CamMove_Thread, 0, "cam");
//    G8RTOS_AddThread(Read_Buttons, 0, "but");
//    G8RTOS_AddThread(Read_JoystickPress, 0, "joy");
//
//    G8RTOS_Add_APeriodicEvent(GPIOE_Handler, 0, INT_GPIOE);
//    G8RTOS_Add_APeriodicEvent(GPIOD_Handler, 0, INT_GPIOD);
//
//    G8RTOS_Add_PeriodicEvent(Get_Joystick, 0, 100);
//    G8RTOS_Add_PeriodicEvent(Print_WorldCoords, 50, 100);

    G8RTOS_Launch();
    while (1);
}

/************************************MAIN*******************************************/
