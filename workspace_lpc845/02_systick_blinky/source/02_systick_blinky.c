#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "fsl_debug_console.h"

// Etiqueta para el LED azul
#define LED_BLUE	1
#define LED_D1	29

// uint16_t i = 0;

void SysTick_Handler(void){

	static uint16_t i = 0;
	static uint16_t m = 0;
	//interrupcion cada 100ms
	i++;
	m++;
	if (i == 5 ){
		i = 0;
		GPIO_PinWrite(GPIO, 1, LED_BLUE, !GPIO_PinRead(GPIO, 1, LED_BLUE));
	}
	if (m == 15 ){
		m = 0;
		GPIO_PinWrite(GPIO, 0, LED_D1, !GPIO_PinRead(GPIO, 0, LED_D1));
	}
}
/*
 * @brief   Application entry point.
*/
int main(void) {
	// Cambio el clock a 18Mhz
	BOARD_BootClockFRO18M();
	// Inicializacion
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    // Estructura de configuracion para salida
    gpio_pin_config_t config = { kGPIO_DigitalOutput, 1 }; // marca como salida, y que valor por defecto es uno
    gpio_pin_config_t config2 = { kGPIO_DigitalOutput, 0 };
    // Habilito el clock del GPIO 1
    GPIO_PortInit(GPIO, 1); // puerto 1 pin 1
    GPIO_PortInit(GPIO, 1);
    // Configuro el pin 0 del GPIO 1 como salida
    GPIO_PinInit(GPIO, 1, LED_BLUE, &config);
    GPIO_PinInit(GPIO, 0, LED_D1, &config2);

    // Configuro SysTick para 100mS
    SysTick_Config(SystemCoreClock/10);

    //while(1) {
    	// Cambio el estado anterior del LED azul


    //}
    return 0;
}
