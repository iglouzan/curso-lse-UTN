################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/Tp_Integrador.c \
../source/mtb.c \
../source/semihost_hardfault.c \
../source/wrappers.c 

C_DEPS += \
./source/Tp_Integrador.d \
./source/mtb.d \
./source/semihost_hardfault.d \
./source/wrappers.d 

OBJS += \
./source/Tp_Integrador.o \
./source/mtb.o \
./source/semihost_hardfault.o \
./source/wrappers.o 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_LPC845M301JBD48 -DCPU_LPC845M301JBD48_cm0plus -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"C:\Users\diazs\Desktop\Programacion escuela\Redes y Protocolos\Sistemas embebidos\curso-lse\workspace_lpc845\Tp_Integrador\board" -I"C:\Users\diazs\Desktop\Programacion escuela\Redes y Protocolos\Sistemas embebidos\curso-lse\workspace_lpc845\Tp_Integrador\source" -I"C:\Users\diazs\Desktop\Programacion escuela\Redes y Protocolos\Sistemas embebidos\curso-lse\workspace_lpc845\Tp_Integrador\component\uart" -I"C:\Users\diazs\Desktop\Programacion escuela\Redes y Protocolos\Sistemas embebidos\curso-lse\workspace_lpc845\Tp_Integrador\drivers" -I"C:\Users\diazs\Desktop\Programacion escuela\Redes y Protocolos\Sistemas embebidos\curso-lse\workspace_lpc845\Tp_Integrador\CMSIS" -I"C:\Users\diazs\Desktop\Programacion escuela\Redes y Protocolos\Sistemas embebidos\curso-lse\workspace_lpc845\Tp_Integrador\device" -I"C:\Users\diazs\Desktop\Programacion escuela\Redes y Protocolos\Sistemas embebidos\curso-lse\workspace_lpc845\Tp_Integrador\utilities" -I"C:\Users\diazs\Desktop\Programacion escuela\Redes y Protocolos\Sistemas embebidos\curso-lse\lpc845\ejemplos\freertos\inc" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source

clean-source:
	-$(RM) ./source/Tp_Integrador.d ./source/Tp_Integrador.o ./source/mtb.d ./source/mtb.o ./source/semihost_hardfault.d ./source/semihost_hardfault.o ./source/wrappers.d ./source/wrappers.o

.PHONY: clean-source

