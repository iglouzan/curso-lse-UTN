#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "fsl_debug_console.h"


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
    GPIO_PortInit(GPIO, 0); // Inicio puerto 2
    // Configuro el pin 0 del GPIO 1 como salida
    GPIO_PinInit(GPIO, 0, B_USER, &config_in);
    GPIO_PinInit(GPIO, 0, A, &config_out);
    GPIO_PinInit(GPIO, 0, B, &config_out);
    GPIO_PinInit(GPIO, 0, C, &config_out);
    GPIO_PinInit(GPIO, 0, D, &config_out);
    GPIO_PinInit(GPIO, 0, E, &config_out);
    GPIO_PinInit(GPIO, 0, F, &config_out);
    //GPIO_PinInit(GPIO, 0, G, &config_out);
    GPIO_PinInit(GPIO, 0, ANODE, &config_out);
    GPIO_PinInit(GPIO, 0, ANODE2, &config_out);

    while(1) {
    	if(!GPIO_PinRead(GPIO, 0, B_USER)) {
    		for(uint32_t i = 0; i < 200000; i++);
    		// agrego un for para implementar el antirebote
    		GPIO_PinWrite(GPIO, 0, ANODE , 0);
    		GPIO_PinWrite(GPIO, 0, ANODE2 , 0);
    	    GPIO_PinWrite(GPIO, 0, A, 0);
    	    GPIO_PinWrite(GPIO, 0, B, 1);
    	    GPIO_PinWrite(GPIO, 0, C, 1);
    	    GPIO_PinWrite(GPIO, 0, D, 1);
    	    GPIO_PinWrite(GPIO, 0, F, 1);
    	    GPIO_PinWrite(GPIO, 0, G, 1);
    	    for(uint32_t i = 0; i < 200000; i++);
    	    GPIO_PinWrite(GPIO, 0, ANODE , 0);
    	    GPIO_PinWrite(GPIO, 0, ANODE2 , 0);
    	    GPIO_PinWrite(GPIO, 0, A, 1);
    	    GPIO_PinWrite(GPIO, 0, B, 0);
    	    GPIO_PinWrite(GPIO, 0, C, 1);
    	    GPIO_PinWrite(GPIO, 0, D, 1);
    	    GPIO_PinWrite(GPIO, 0, F, 1);
    	    GPIO_PinWrite(GPIO, 0, G, 1);
    	    for(uint32_t i = 0; i < 200000; i++);
    	    GPIO_PinWrite(GPIO, 0, ANODE , 0);
    	    GPIO_PinWrite(GPIO, 0, ANODE2 , 0);
    	    GPIO_PinWrite(GPIO, 0, A, 1);
    	    GPIO_PinWrite(GPIO, 0, B, 1);
    	    GPIO_PinWrite(GPIO, 0, C, 0);
    	    GPIO_PinWrite(GPIO, 0, D, 1);
    	    GPIO_PinWrite(GPIO, 0, F, 1);
    	    GPIO_PinWrite(GPIO, 0, G, 1);
    	    for(uint32_t i = 0; i < 200000; i++);
    	    GPIO_PinWrite(GPIO, 0, ANODE , 0);
    	    GPIO_PinWrite(GPIO, 0, ANODE2 , 0);
    	    GPIO_PinWrite(GPIO, 0, A, 1);
    	    GPIO_PinWrite(GPIO, 0, B, 1);
    	    GPIO_PinWrite(GPIO, 0, C, 1);
    	    GPIO_PinWrite(GPIO, 0, D, 0);
    	    GPIO_PinWrite(GPIO, 0, F, 1);
    	    GPIO_PinWrite(GPIO, 0, G, 1);
    	    for(uint32_t i = 0; i < 200000; i++);
    	    GPIO_PinWrite(GPIO, 0, ANODE , 0);
    	    GPIO_PinWrite(GPIO, 0, ANODE2 , 0);
    	    GPIO_PinWrite(GPIO, 0, A, 1);
    	    GPIO_PinWrite(GPIO, 0, B, 1);
    	    GPIO_PinWrite(GPIO, 0, C, 1);
    	    GPIO_PinWrite(GPIO, 0, D, 1);
    	    GPIO_PinWrite(GPIO, 0, F, 0);
    	    GPIO_PinWrite(GPIO, 0, G, 1);
    	}
    	else {
    	// Apago el LED
    		for(uint32_t i = 0; i < 20000; i++);
    	    GPIO_PinWrite(GPIO, 0, ANODE , 1);
    	    GPIO_PinWrite(GPIO, 0, ANODE2 , 1);
    	    GPIO_PinWrite(GPIO, 0, A, 1);
    	    GPIO_PinWrite(GPIO, 0, B, 1);
    	    GPIO_PinWrite(GPIO, 0, C, 1);
    	    GPIO_PinWrite(GPIO, 0, D, 1);
    	    GPIO_PinWrite(GPIO, 0, F, 1);
    	    GPIO_PinWrite(GPIO, 0, G, 1);

    	}
    	// Cambio el estado anterior del LED azul, leo el estado que tiene y lo invierto
    	//GPIO_PinWrite(GPIO, 1, LED_RED, !GPIO_PinRead(GPIO, 1, LED_RED));
    	// Demora
    	//for(uint32_t i = 0; i < 500000; i++);
    }
    return 0;
}
