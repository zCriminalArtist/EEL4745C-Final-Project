// G8RTOS_Threads.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for thread functions.

/************************************Includes***************************************/

#include "./threads.h"

#include "./MultimodDrivers/multimod.h"

#include "./sprites/bird.h"
#include "./sprites/floor.h"
#include "./sprites/background.h"
#include "./sprites/play.h"
#include "./sprites/gameover.h"
#include "./sprites/ground_pipe.h"
#include "./sprites/ceiling_pipe.h"

#include <stdio.h>
#include <stdlib.h>

// Change this to change the number of points that make up each line of a cube.
// Note that if you set this too high you will have a stack overflow!
#define Num_Interpolated_Points 10

// sizeof(float) * num_lines * (Num_Interpolated_Points + 2) = ?

#define MAX_NUM_CUBES           (MAX_THREADS - 3)


/*********************************Global Variables**********************************/
struct bird bird = {140, 0};
//struct pipe groundPipes[4];
//struct pipe ceilingPipes[4];
struct pipe groundPipe;
struct pipe ceilingPipe;

uint8_t playing = 0;
/*********************************Global Variables**********************************/

/*************************************Threads***************************************/

void Update_Bird(void) {
	while(1) {

		if (bird.birdPosY > 70) {
			G8RTOS_WaitSemaphore(&sem_SPIA);
			ST7789_DrawRectangle(70, bird.birdPosY, 17*2, 15*2, 0xCE0A);
			G8RTOS_SignalSemaphore(&sem_SPIA);

			bird.birdPosY -= bird.birdSpeed;
			bird.birdSpeed += 1;
		} else {
			G8RTOS_WaitSemaphore(&sem_SPIA);
			ST7789_drawImage(50, 170, 140, 34, gameover_sprite, 1, 0, 0);
			G8RTOS_SignalSemaphore(&sem_SPIA);
			playing = 0;
		}

		G8RTOS_WaitSemaphore(&sem_SPIA);
		ST7789_drawImage(70, bird.birdPosY, 17, 12, bird_sprite, 2, 0, 0);
		G8RTOS_SignalSemaphore(&sem_SPIA);

		sleep(60);

		if (!playing) {
			G8RTOS_KillSelf();
		}
	}
}

void Update_Pipes(void) {
	while(1) {

		G8RTOS_WaitSemaphore(&sem_SPIA);
		ST7789_DrawRectangle(groundPipe.posX + 43, 70, 5, 130, 0xCE0A);
		G8RTOS_SignalSemaphore(&sem_SPIA);

		G8RTOS_WaitSemaphore(&sem_SPIA);
		ST7789_DrawRectangle(ceilingPipe.posX + 43, ceilingPipe.posY, 5, 100, 0xCE0A);
		G8RTOS_SignalSemaphore(&sem_SPIA);

		groundPipe.posX -= 7;
		ceilingPipe.posX -= 7;

		G8RTOS_WaitSemaphore(&sem_SPIA);
		ST7789_drawImage(groundPipe.posX, groundPipe.posY, 44, 130, ground_pipe_sprite, 1, 0, 70);
		G8RTOS_SignalSemaphore(&sem_SPIA);

		G8RTOS_WaitSemaphore(&sem_SPIA);
		ST7789_drawImage(ceilingPipe.posX, ceilingPipe.posY, 44, 130, ceiling_pipe_sprite, 1, 0, 0);
		G8RTOS_SignalSemaphore(&sem_SPIA);

//		for (int i = 0; i < 2; i++) {
//			G8RTOS_WaitSemaphore(&sem_SPIA);
//			ST7789_DrawRectangle(groundPipes[i].posX + 43, 70, 5, 130, 0xCE0A);
//			G8RTOS_SignalSemaphore(&sem_SPIA);
//
//			G8RTOS_WaitSemaphore(&sem_SPIA);
//			ST7789_DrawRectangle(ceilingPipes[i].posX + 43, ceilingPipes[i].posY, 5, 110, 0xCE0A);
//			G8RTOS_SignalSemaphore(&sem_SPIA);
//
//			groundPipes[i].posX -= 4;
//			ceilingPipes[i].posX -= 4;
//
//			G8RTOS_WaitSemaphore(&sem_SPIA);
//			ST7789_drawImage(groundPipes[i].posX, groundPipes[i].posY, 44, 130, ground_pipe_sprite, 1, 0, 70);
//			G8RTOS_SignalSemaphore(&sem_SPIA);
//
//			G8RTOS_WaitSemaphore(&sem_SPIA);
//			ST7789_drawImage(ceilingPipes[i].posX, ceilingPipes[i].posY, 44, 130, ceiling_pipe_sprite, 1, 0, 0);
//			G8RTOS_SignalSemaphore(&sem_SPIA);
//		}

		sleep(60);

		if (!playing) {
			G8RTOS_KillSelf();
		}
	}
}

void Play_StartScreen(void) {
	G8RTOS_WaitSemaphore(&sem_SPIA);
	ST7789_DrawRectangle(0, 150, 240, 170, 0xCE0A);
	ST7789_drawImage(0, 70, 240, 88, background_sprite, 1, 0, 0);
	ST7789_drawImage(0, 0, 480, 70, floor_sprite, 1, 0, 0);
	ST7789_drawImage(70, 100, 100, 100, play_sprite, 1, 0, 0);
	ST7789_drawImage(70, bird.birdPosY, 17, 12, bird_sprite, 2, 0, 0);
	G8RTOS_SignalSemaphore(&sem_SPIA);

	uint16_t scroll = 0;

	while(1) {
		scroll += 3;

		G8RTOS_WaitSemaphore(&sem_SPIA);
		ST7789_drawImage(0, 0, 480, 70, floor_sprite, 1, scroll % 240, 0);
		G8RTOS_SignalSemaphore(&sem_SPIA);

		if (playing) {
			G8RTOS_KillSelf();
		}
	}
}

void Check_ButtonPress(void) {
	while (1) {
		G8RTOS_WaitSemaphore(&sem_PCA9555_Debounce);

		sleep(10);

		uint8_t buttons = MultimodButtons_Get();
		if (!(buttons & SW4)) {
			if (playing) {
				bird.birdSpeed = -7;
			} else {
				playing = 1;
				G8RTOS_WaitSemaphore(&sem_SPIA);
				ST7789_DrawRectangle(0, 71, 240, 200, 0xCE0A);
				G8RTOS_SignalSemaphore(&sem_SPIA);

//				for (int i = 0; i < 2; i++) {
//					groundPipes[i].posX = 120*i + 340;
//					groundPipes[i].posY = rand() % 60;
//					ceilingPipes[i].posX = groundPipes[i].posX;
//					ceilingPipes[i].posY = groundPipes[i].posY + 130 + 60;
//				}
				groundPipe.posX = 360;
				groundPipe.posY = rand() % 60;
				ceilingPipe.posX = groundPipe.posX;
				ceilingPipe.posY = groundPipe.posY + 130 + 60;

			    G8RTOS_AddThread(Update_Bird, 0, "");
			    G8RTOS_AddThread(Update_Pipes, 0, "");
			}
		}

		if (!(buttons & SW1)) {
			if (bird.birdPosY <= 70) {
				playing = 0;
				bird.birdPosY = 140;
				bird.birdSpeed = 0;
				G8RTOS_AddThread(Play_StartScreen, 0, "");
			}
		}

		GPIOIntClear(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
		GPIOIntEnable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
	}
}



/********************************Periodic Threads***********************************/

/*******************************Aperiodic Threads***********************************/
void GPIOE_Handler() {
    // Disable interrupt
	GPIOIntDisable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    // Signal relevant semaphore
	G8RTOS_SignalSemaphore(&sem_PCA9555_Debounce);
}
