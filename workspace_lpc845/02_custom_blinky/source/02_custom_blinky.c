#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "fsl_debug_console.h"
#include "fsl_adc.h"
#include "fsl_power.h"

// Etiqueta para el LED azul
#define LED_RED	2
#define ADC_POT_CH	8

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

    //Habilito funcion analogica para el canal 8
    CLOCK_EnableClock(kCLOCK_Swm);
    // Configuro la funcion de ADC en el canal del potenciometro
    SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN8, true);
    // Desactivo clock de matriz de conmutacion
    CLOCK_DisableClock(kCLOCK_Swm);

    // Elijo clock desde el FRO con divisor de 1 (30MHz)
    CLOCK_Select(kADC_Clk_From_Fro);
    CLOCK_SetClkDivider(kCLOCK_DivAdcClk, 1);

    // Prendo el ADC
    POWER_DisablePD(kPDRUNCFG_PD_ADC0);

    // Obtengo frecuencia deseada y calibro ADC
    uint32_t frequency = CLOCK_GetFreq(kCLOCK_Fro) / CLOCK_GetClkDivider(kCLOCK_DivAdcClk);
    ADC_DoSelfCalibration(
    		ADC0,
			frequency
		);
    // Configuracion por defecto del ADC (Synchronous Mode, Clk Divider 1, Low Power Mode true, High Voltage Range)
    adc_config_t adc_config;
    ADC_GetDefaultConfig(&adc_config);
    ADC_Init(ADC0, &adc_config);

    // Configuro y habilito secuencia A
    adc_conv_seq_config_t adc_sequence = {
    		.channelMask = 1 << 8, // Canal 8 habilitado
			.triggerMask = 0,
			.triggerPolarity = kADC_TriggerPolarityPositiveEdge,
			.enableSyncBypass = false,
			.interruptMode = kADC_InterruptForEachConversion
    };

    ADC_SetConvSeqAConfig(ADC0, &adc_sequence);
    ADC_EnableConvSeqA(ADC0, true);

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

    while(1) {
    	// Cambio el estado anterior del LED azul
    	adc_result_info_t adc_info;
    	// Inicio conversion
    	ADC_DoSoftwareTriggerConvSeqA(ADC8);
    	// Espero a terminar la conversion
    	while(!ADC_GetChannelConversionResult(ADC8, ADC_POT_CH, &adc_info));
    	// Muestro resultado
    	PRINTF(
    		"El resultado del canal %ld en el PIO0_7 dio %d\n",
    		adc_info.channelNumber,
    		adc_info.result
    	    	);

    }
    return 0;
}
