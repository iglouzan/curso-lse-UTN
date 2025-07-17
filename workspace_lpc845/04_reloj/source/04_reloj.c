#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "task.h"

#define B_USER	4
#define A	10
#define B	11
#define C	6
#define D	14
#define E	0
#define F	13
#define G	15
#define ANODE 9
#define ANODE2 8

void task_hello(void *params){
	while(1){
		PRINTF("hola desde una tarea!\r\n");
		vTaskDelay(500);

	}
}

int main(void){
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	xTaskCreate(
		task_hello,
		"Hello",
		configMINIMAL_STACK_SIZE,
		NULL,
		1,
		NULL
	);


	vTaskStartScheduler();

	while(1){

	}
	return 0;
}

