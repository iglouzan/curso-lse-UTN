#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "task.h"
#include "fsl_swm.h"
#include "fsl_i2c.h"
#include "queue.h"
#include "app_tasks.h"
#include "wrappers.h"
#include "fsl_adc.h"
#include "fsl_power.h"

#define USER	4
#define ADC_POT_CH 8
#define S1 16
#define S2 25
#define LED_AZUL 1

#define COM_1 8
#define COM_2 9
#define SEG_A 10
#define SEG_B 11
#define SEG_C 6
#define SEG_D 14
#define SEG_E 0
#define SEG_F 13
#define SEG_G 15

#define PWM_FREQ	1000



int setpoint = 25;
uint32_t event;

xQueueHandle queue_setpoint;
xQueueHandle queue_boo_var;
xQueueHandle queue_adc;
// Cola para selecion de valor para el display
xQueueHandle queue_light_intensity;
xQueueHandle queue_setpoint;
xQueueHandle queue_adc;
// Cola para datos de temperatura
// Handler para la tarea de display write
TaskHandle_t handle_display;


void task_init(void *params) {
	// Inicializacion de GPIO

	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();
	CLOCK_EnableClock(kCLOCK_Swm);
	SWM_SetMovablePinSelect(SWM0, kSWM_SCT_OUT4, kSWM_PortPin_P0_29);
	CLOCK_DisableClock(kCLOCK_Swm);

	// Eligo el clock para el Timer
	    uint32_t sctimer_clock = CLOCK_GetFreq(kCLOCK_Fro);
	    // Configuracion del SCT Timer
	    sctimer_config_t sctimer_config;
	    SCTIMER_GetDefaultConfig(&sctimer_config);
	    SCTIMER_Init(SCT0, &sctimer_config);

	    // Configuro el PWM
	    sctimer_pwm_signal_param_t pwm_config = {
			.output = kSCTIMER_Out_4,		// Salida del Timer
			.level = kSCTIMER_LowTrue,		// Logica negativa
			.dutyCyclePercent = 50			// 50% de ancho de pulso
	    };


	    // Variable para guardar el evento al quese asigna el PWM

	    // Inicializo el PWM
	    SCTIMER_SetupPwm(
			SCT0,
			&pwm_config,
			kSCTIMER_CenterAlignedPwm,
			PWM_FREQ,
			sctimer_clock,
			&event
		);
	    // Inicializo el Timer
	    SCTIMER_StartTimer(SCT0, kSCTIMER_Counter_U);

	wrapper_gpio_init(0);
	wrapper_gpio_init(1);

	// Inicializacion BH1750

	// Arranco de 30 MHz
	BOARD_BootClockFRO30M();
	// Inicializo el clock del I2C1
	wrapper_i2c_init();
	wrapper_bh1750_init();

	// Configuro el ADC
	wrapper_adc_init();
	// Configuro el display
	wrapper_display_init();
	// Configuro botones
	wrapper_btn_init();
	// Inicializo el PWM
	// Elimino tarea para liberar recursos

	vTaskDelete(NULL);
}

void task_BH1750(void *params){

	uint8_t var = 0;

	while(1){

		if(I2C_MasterStart(I2C1, BH1750_ADDR, kI2C_Read) == kStatus_Success) {
			// Resultado
			uint8_t res[2] = {0};
			I2C_MasterReadBlocking(I2C1, res, 2, kI2C_TransferDefaultFlag);
			I2C_MasterStop(I2C1);
			// Devuelvo el resultado
			float lux = ((res[0] << 8) + res[1]) / 1.2;

			var = lux * 100.0 / 20000.0;
			//PRINTF("LUX : %d \r\n",(uint8_t) var);
		}
		xQueueOverwrite(
			queue_light_intensity,
			&var
		);
		vTaskDelay(500);
	}
}

void task_setpoint(void*params) {

	uint8_t setpoint = 50;

	 while(1) {

		if(!GPIO_PinRead(GPIO, 0, S1)) {

			setpoint = setpoint + 1;
			if (setpoint > 75){
				setpoint = 75;
			}
		}
		else if (!GPIO_PinRead(GPIO,0, S2)){
			setpoint = setpoint - 1;
			if (setpoint < 25){
				setpoint = 25;
			}
		}

		xQueueOverwrite(
			queue_setpoint,
			&setpoint
		);
		// Tarda mucho en responder, habria que poner interrupciones o algo
		vTaskDelay(250);
	 }
}


void task_btn(void *params) {
	// Dato para pasar
	bool boo = 0;

	while(1) {
		// Veo que boton se presiono
		if(!GPIO_PinRead(GPIO, 0, USER)) {
			// Antirebote
			boo = !boo;
			vTaskDelay(20);
		}
		xQueueOverwrite(
			queue_boo_var,
			&boo
		);
		vTaskDelay(250);
	}
}

void task_display_write(void *params) {

	uint8_t var;
	bool bool_boo = false;

	while(1) {

		xQueuePeek(queue_boo_var, &bool_boo, portMAX_DELAY);

		if(bool_boo){
			// Veo que variable hay que mostrar
			xQueuePeek(queue_light_intensity, &var, 100);
		}
		else {
			xQueuePeek(queue_setpoint, &var, 100);
		}

		// Muestro el numero
		wrapper_display_off();
		wrapper_display_write((uint8_t)(var / 10));
		wrapper_display_on(COM_1);
		vTaskDelay(10);
		wrapper_display_off();
		wrapper_display_write((uint8_t)(var % 10));
		wrapper_display_on(COM_2);
		vTaskDelay(10);
	}
}

void task_adc(void *params) {
	while(1) {
		ADC_DoSoftwareTriggerConvSeqA(ADC0);
		// Espero a terminar la conversion
		vTaskDelay(250);

	}
}


void ADC0_SEQA_IRQHandler(void) {
	// Variable de cambio de contexto
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// Verifico que se haya terminado la conversion correctamente
	if(kADC_ConvSeqAInterruptFlag == (kADC_ConvSeqAInterruptFlag & ADC_GetStatusFlags(ADC0))) {
		// Limpio flag de interrupcion
		ADC_ClearStatusFlags(ADC0, kADC_ConvSeqAInterruptFlag);
		// Resultado de conversionz
		adc_result_info_t ref_info;
		// Leo el valor del ADC
		ADC_GetChannelConversionResult(ADC0, ADC_POT_CH, &ref_info);
		// Mando por la cola los datos
		xQueueOverwriteFromISR(queue_adc, &ref_info.result, &xHigherPriorityTaskWoken);
		// Veo si hace falta un cambio de contexto
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

void task_blueled(void *params) {
	uint16_t result ;


	while(1){
		xQueuePeek(queue_adc, &result, 100);
		uint8_t duty_cycle = result * 100.0 / 4095.0;
		SCTIMER_UpdatePwmDutycycle(SCT0, kSCTIMER_Out_4, duty_cycle, event);
	//	vTaskDelay(100);

	}
}


void task_consola(void *params) {
	uint8_t var_luz;
	uint8_t var_setpoint;
	uint16_t ref_info;

	while(1){
		uint32_t time1111 = xTaskGetTickCount();

		xQueuePeek(queue_light_intensity, &var_luz, 100);
		xQueuePeek(queue_setpoint, &var_setpoint, 100);
		xQueuePeek(queue_adc, &ref_info, 100);
		PRINTF("%3d%% | %3d%% | %4d | %6d ms\n", (uint8_t) var_luz, (uint8_t) var_setpoint, (uint16_t) ref_info, (uint32_t) time1111);

//		PRINTF("INTENSIDAD LUMINICA = %d \n", (uint8_t) var_luz);
//		PRINTF("SETPOINT = %d \n", (uint8_t) var_setpoint);
//		PRINTF("ADC = %d \n", (adc_result_info_t) ref_info);
//		PRINTF("TIME = %d ms\n", (uint32_t) time1111);

		vTaskDelay(5000);

	}
}
/**
 * @brief Escribe valores en el display
 */


/**
 * @brief Programa principal
 */
int main(void) {
	// Clock del sistema a 30 MHzOARD_BootClockFRO30M();

	// Inicializo colas
	queue_setpoint = xQueueCreate(1, sizeof(int));
	queue_boo_var = xQueueCreate(1, sizeof(bool));
	queue_adc = xQueueCreate(1, sizeof(adc_data_t));
	queue_light_intensity = xQueueCreate(1, sizeof(uint8_t));

	// Creacion de tareas

	xTaskCreate(
		task_init,
		"Init",
		tskINIT_STACK,
		NULL,
		tskINIT_PRIORITY,
		NULL
	);

	xTaskCreate(
		task_BH1750,
		"BH1750",
		tskBH1750_STACK,
		NULL,
		tskBH1750_PRIORITY,
		NULL
	);

	xTaskCreate(
		task_setpoint,
		"Setpoint",
		configMINIMAL_STACK_SIZE,
		NULL,
		1,
		NULL
	);

	xTaskCreate(
		task_btn,
		"Btn",
		configMINIMAL_STACK_SIZE,
		NULL,
		1,
		NULL
	);

	xTaskCreate(
		task_display_write,
		"Display",
		configMINIMAL_STACK_SIZE,
		NULL,
		1,
		NULL
	);

	xTaskCreate(
		task_adc,
		"ADC",
		configMINIMAL_STACK_SIZE,
		NULL,
		1,
		NULL
	);

	xTaskCreate(
		task_consola,
		"CONSOLA",
		3 * configMINIMAL_STACK_SIZE,
		NULL,
		1,
		NULL
	);
	xTaskCreate(
		task_blueled,
		"blueled",
		configMINIMAL_STACK_SIZE,
		NULL,
		1,
		NULL
	);

	vTaskStartScheduler();

    while(1);
    return 0;
}
