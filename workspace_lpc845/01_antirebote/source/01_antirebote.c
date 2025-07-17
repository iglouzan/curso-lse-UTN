#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "fsl_debug_console.h"

// Etiqueta para el LED azul
#define LED_RED		2
#define B_USER	4


/*
 * @brief   Application entry point.
*/
int main(void) {
	// Inicializacion
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    // Estructura de configuracion para salida
    gpio_pin_config_t config_out = { kGPIO_DigitalOutput, 1 }; // marca como salida, y que valor por defecto es uno
    // Habilito el clock del GPIO 1
    gpio_pin_config_t config_in = { kGPIO_DigitalInput };
    GPIO_PortInit(GPIO, 1); // puerto 1 inicio
    GPIO_PortInit(GPIO, 0); // Inicio puerto 2
    // Configuro el pin 0 del GPIO 1 como salida
    GPIO_PinInit(GPIO, 1, LED_RED, &config_out);
    GPIO_PinInit(GPIO, 0, B_USER, &config_in);

    while(1) {
    	if(!GPIO_PinRead(GPIO, 0, B_USER)) {
    		for(uint32_t i = 0; i < 20000; i++);
    		// agrego un for para implementar el antirebote
    	    GPIO_PinWrite(GPIO, 1, LED_RED, 0);
    	}
    	else {
    	// Apago el LED
    		for(uint32_t i = 0; i < 20000; i++);
    		GPIO_PinWrite(GPIO, 1, LED_RED, 1);
    	}
    	// Cambio el estado anterior del LED azul, leo el estado que tiene y lo invierto
    	//GPIO_PinWrite(GPIO, 1, LED_RED, !GPIO_PinRead(GPIO, 1, LED_RED));
    	// Demora
    	//for(uint32_t i = 0; i < 500000; i++);
    }
    return 0;
}






#define A	10
#define B	11
#define C	6
#define D	14
#define E	0
#define F	13
#define G	15
#define ANODE 9
#define ANODE2 8
