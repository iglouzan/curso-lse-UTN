#include "board.h"
#include "fsl_sctimer.h"
#include "fsl_swm.h"
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "fsl_common.h"
#include "fsl_swm.h"
#include "fsl_debug_console.h"
#include "fsl_adc.h"
#include "fsl_power.h"

// Frecuencia de PWM
#define PWM_FREQ	50
#define servo 		19
#define pote		18
#define ADC_POT_CH	8



/**
 * @brief Programa principal
 */
int main(void) {
	// Inicializacion de consola
	// Inicializacion
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

    CLOCK_EnableClock(kCLOCK_Swm);
	// Configuro la funcion de ADC en el canal del potenciometro
	SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN8, true);
	// Desactivo clock de matriz de conmutacion
	CLOCK_DisableClock(kCLOCK_Swm);

	// Prendo el ADC
	POWER_DisablePD(kPDRUNCFG_PD_ADC0);
    CLOCK_Select(kADC_Clk_From_Fro);
    CLOCK_SetClkDivider(kCLOCK_DivAdcClk, 1);

	uint32_t frequency = CLOCK_GetFreq(kCLOCK_Fro) / CLOCK_GetClkDivider(kCLOCK_DivAdcClk);
	    ADC_DoSelfCalibration(
	    		ADC0,
				frequency
			);

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



	// Conecto la salida 4 del SCT al LED azul
    CLOCK_EnableClock(kCLOCK_Swm);
    SWM_SetMovablePinSelect(SWM0, kSWM_SCT_OUT4, kSWM_PortPin_P1_1);
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
    uint32_t event;
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

    gpio_pin_config_t config = { kGPIO_DigitalOutput, 0 };
    GPIO_PortInit(GPIO, 0);
    GPIO_PinInit(GPIO, 0, servo, &config);
    GPIO_PinInit(GPIO, 0, pote, &config);

    while (1) {


    	adc_result_info_t adc_info;
    	// Inicio conversion
    	ADC_DoSoftwareTriggerConvSeqA(ADC0);
    	// Espero a terminar la conversion
    		  while(!ADC_GetChannelConversionResult(ADC0, ADC_POT_CH, &adc_info)){};
    		  // Muestro resultado
    		    	PRINTF(
    		    		"El resultado del canal %ld en el PIO0_7 dio %d\n",
    		    		adc_info.channelNumber,
    		    		adc_info.result
						);
    		    	// Variable para guardar el duty
    uint32_t duty;
    		    	// Pido por consola un nuevo ancho de pulso
    PRINTF("Ingrese un nuevo ciclo de actividad: ");
    SCANF("%d", &duty);
    		    	// Verifico que este entre 0 y 100
    if(duty < 11 && duty > 3){
    		    		//Actualizo el ancho de pulso
    SCTIMER_UpdatePwmDutycycle(SCT0, kSCTIMER_Out_4, duty, event);
    		    	// Cambio el estado anterior del LED azul
    }


    }
}

