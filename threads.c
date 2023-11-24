// G8RTOS_Threads.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for thread functions.

/************************************Includes***************************************/

#include "./threads.h"

#include "./MultimodDrivers/multimod.h"
#include "./MiscFunctions/Shapes/inc/cube.h"
#include "./MiscFunctions/LinAlg/inc/linalg.h"
#include "./MiscFunctions/LinAlg/inc/quaternions.h"
#include "./MiscFunctions/LinAlg/inc/vect3d.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Change this to change the number of points that make up each line of a cube.
// Note that if you set this too high you will have a stack overflow!
#define Num_Interpolated_Points 10

// sizeof(float) * num_lines * (Num_Interpolated_Points + 2) = ?

#define MAX_NUM_CUBES           (MAX_THREADS - 3)


/*********************************Global Variables**********************************/
Quat_t spaceship_pos = {20, 20, 110, 50};
uint8_t num_enemies = 0;
uint8_t reset = 0;

Quat_t world_camera_frame_offset = {0, 0, 0, 50};
Quat_t world_camera_frame_rot_offset;
Quat_t world_view_rot = {1, 0, 0, 0};
Quat_t world_view_rot_inverse = {1, 0, 0, 0};

// How many cubes?
uint8_t num_cubes = 0;

// y-axis controls z or y
uint8_t joystick_y = 1;

// Kill a cube?
uint8_t kill_cube = 0;

/*********************************Global Variables**********************************/

/*************************************Threads***************************************/

void Idle_Thread(void) {
    time_t t;
    srand((unsigned) time(&t));
    while(1);
}

void SpaceshipMove_Thread(void) {
    // Initialize / declare any variables here
	uint16_t minVal = 130;
	uint16_t maxVal = 4020;
    while(1) {
        // Get result from joystick
    	uint16_t x = G8RTOS_ReadFIFO(JOYSTICK_FIFO);
    	uint16_t y = G8RTOS_ReadFIFO(JOYSTICK_FIFO);

    	double x_norm = ((double)(x - minVal) / (maxVal - minVal)) * 2 - 1;
    	double y_norm = ((double)(y - minVal) / (maxVal - minVal)) * 2 - 1;


        // If joystick axis within deadzone, set to 0. Otherwise normalize it.
    	if (fabs(x_norm) < 0.1) {
    		x_norm = 0.0;
    	} else {
    		if (x_norm > 1.0) x_norm = 1.0;
    		if (x_norm < -1.0) x_norm = -1.0;
    	}

    	if (fabs(y_norm) < 0.1) {
    	    y_norm = 0.0;
    	} else {
    	    if (y_norm > 1.0) y_norm = 1.0;
    	    if (y_norm < -1.0) y_norm = -1.0;
    	}

        // Update world camera position. Update y/z coordinates depending on the joystick toggle.
    	spaceship_pos.y -= x_norm;

        // sleep
    	sleep(10);
    }
}

void Spaceship_Thread(void) {
    while(1) {
    	G8RTOS_WaitSemaphore(&sem_SPIA);

    	ST7789_Select();
    	ST7789_DrawRectangle(0, 50, X_MAX, 20, 0x0000);
    	ST7789_Deselect();

    	G8RTOS_SignalSemaphore(&sem_SPIA);

    	if (reset) G8RTOS_KillSelf();

        G8RTOS_WaitSemaphore(&sem_SPIA);

        ST7789_Select();
        ST7789_DrawRectangle(spaceship_pos.y, spaceship_pos.z, spaceship_pos.w, spaceship_pos.x, 0xFFFF);
        ST7789_Deselect();

        G8RTOS_SignalSemaphore(&sem_SPIA);

        sleep(100);
    }
}

void Enemy_Thread(void) {
	Quat_t enemy_pos = {20, 20, 40, 250};
	enemy_pos.y += num_enemies*30;
	num_enemies++;

	while(1) {
		G8RTOS_WaitSemaphore(&sem_SPIA);

		ST7789_Select();
		ST7789_DrawRectangle(enemy_pos.y, enemy_pos.z, enemy_pos.w, enemy_pos.x, 0x0000);
		ST7789_Deselect();

		G8RTOS_SignalSemaphore(&sem_SPIA);

		if (reset) G8RTOS_KillSelf();

		int8_t x = (rand() % 10);
		int8_t sign = (rand() % 2);
		enemy_pos.y += sign ? x : -1*x;

		G8RTOS_WaitSemaphore(&sem_SPIA);

		ST7789_Select();
		ST7789_DrawRectangle(enemy_pos.y, enemy_pos.z, enemy_pos.w, enemy_pos.x, ((0 << 11) | (0 << 5) | 31));
		ST7789_Deselect();

		G8RTOS_SignalSemaphore(&sem_SPIA);

		if (G8RTOS_GetNumberOfThreads() < MAX_THREADS) {
			G8RTOS_WriteFIFO(SPAWNCOOR_FIFO, enemy_pos.y);
			G8RTOS_AddThread(Projectile_Thread, 0, "proj");
		}

		sleep(2000);
	}
}

void Projectile_Thread(void) {
	int8_t x = G8RTOS_ReadFIFO(SPAWNCOOR_FIFO);
	Quat_t proj_pos = {4, 4, x, 220};

	while(1) {
		G8RTOS_WaitSemaphore(&sem_SPIA);

		ST7789_Select();
		ST7789_DrawRectangle(proj_pos.y, proj_pos.z, proj_pos.w, proj_pos.x, 0x0000);
		ST7789_Deselect();

		G8RTOS_SignalSemaphore(&sem_SPIA);

		proj_pos.z -= 20;

		if (reset) G8RTOS_KillSelf();

		if (proj_pos.z < 40) {
			G8RTOS_KillSelf();
		}

		if ((abs(proj_pos.y - spaceship_pos.y) < 10) && ((abs(proj_pos.z - spaceship_pos.z) < 30))) {
			reset = 1;
			G8RTOS_WaitSemaphore(&sem_SPIA);

			ST7789_Select();
			ST7789_Fill(((0 << 11) | (0 << 5) | 30));
			ST7789_Deselect();

			G8RTOS_SignalSemaphore(&sem_SPIA);

			UARTprintf("You Lost");
			G8RTOS_KillSelf();
		}

		G8RTOS_WaitSemaphore(&sem_SPIA);

		ST7789_Select();
		ST7789_DrawRectangle(proj_pos.y, proj_pos.z, proj_pos.w, proj_pos.x, ((0 << 11) | (32 << 5) | 0));
		ST7789_Deselect();

		G8RTOS_SignalSemaphore(&sem_SPIA);



		sleep(500);
	}
}

void Read_Buttons() {
    // Initialize / declare any variables here
    
    while(1) {
        // Wait for a signal to read the buttons on the Multimod board.
    	G8RTOS_WaitSemaphore(&sem_PCA9555_Debounce);
        // Sleep to debounce
    	sleep(10);
        // Read the buttons status on the Multimod board.
    	uint8_t status = MultimodButtons_Get();
//    	UARTprintf("Buttons: %d\n", status);
        // Process the buttons and determine what actions need to be performed.
    	if (!(status & SW1)) {
    		reset = 1;
    		G8RTOS_WaitSemaphore(&sem_SPIA);

    					ST7789_Select();
    					ST7789_Fill(0x0000);
    					ST7789_Deselect();

    					G8RTOS_SignalSemaphore(&sem_SPIA);
    		UARTprintf("pressed");
    		sleep(1000);
    		reset = 0;
    		num_enemies = 0;
    		spaceship_pos.y = 120;
    		G8RTOS_AddThread(Spaceship_Thread, 0, "ship");
//    		G8RTOS_AddThread(SpaceshipMove_Thread, 0, "move");

    		    G8RTOS_AddThread(Enemy_Thread, 0, "1");
    		    G8RTOS_AddThread(Enemy_Thread, 0, "2");
    		    G8RTOS_AddThread(Enemy_Thread, 0, "3");
    		    G8RTOS_AddThread(Enemy_Thread, 0, "4");
    		    G8RTOS_AddThread(Enemy_Thread, 0, "5");

    	}
        // Clear the interrupt
    	GPIOIntClear(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
        // Re-enable the interrupt so it can occur again.
    	GPIOIntEnable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    }
}

void Read_JoystickPress() {
    // Initialize / declare any variables here
    
    while(1) {
        // Wait for a signal to read the joystick press
    	G8RTOS_WaitSemaphore(&sem_Joystick_Debounce);
        // Sleep to debounce
    	sleep(10);
        // Read the joystick switch status on the Multimod board.
    	uint8_t status = JOYSTICK_GetPress();
        // Toggle the joystick_y flag.
//    	UARTprintf("Status: %d\n", status);
    	if (status) joystick_y = !joystick_y;
//    	UARTprintf("Status: %d\n", joystick_y);
        // Clear the interrupt
    	GPIOIntClear(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
        // Re-enable the interrupt so it can occur again.
    	GPIOIntEnable(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
    }
}



/********************************Periodic Threads***********************************/

void Get_Joystick(void) {
    // Read the joystick
	uint16_t x = JOYSTICK_GetX();
	uint16_t y = JOYSTICK_GetY();
    // Send through FIFO.
	G8RTOS_WriteFIFO(JOYSTICK_FIFO, x);
	G8RTOS_WriteFIFO(JOYSTICK_FIFO, y);
}



/*******************************Aperiodic Threads***********************************/

void GPIOE_Handler() {
    // Disable interrupt
	GPIOIntDisable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    // Signal relevant semaphore
	G8RTOS_SignalSemaphore(&sem_PCA9555_Debounce);
}

void GPIOD_Handler() {
    // Disable interrupt
	GPIOIntDisable(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
    // Signal relevant semaphore
	G8RTOS_SignalSemaphore(&sem_Joystick_Debounce);
}
