################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CAN\ Driver/src/can_stm32.c 

OBJS += \
./CAN\ Driver/src/can_stm32.o 

C_DEPS += \
./CAN\ Driver/src/can_stm32.d 


# Each subdirectory must supply rules for building sources it contributes
CAN\ Driver/src/can_stm32.o: ../CAN\ Driver/src/can_stm32.c CAN\ Driver/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../Core/Inc -I../Drivers -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -I"C:/dev/STM32_Projects/CSP-CAN-FreeRTOS/csp_build/include" -I"C:/dev/STM32_Projects/CSP-CAN-FreeRTOS/csp_build/include_csp" -I"C:/dev/STM32_Projects/CSP-CAN-FreeRTOS/csp_build" -I"C:/dev/STM32_Projects/CSP-CAN-FreeRTOS/CAN Driver/include" -I"C:/dev/STM32_Projects/CSP-CAN-FreeRTOS/CAN Interface/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"CAN Driver/src/can_stm32.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CAN-20-Driver-2f-src

clean-CAN-20-Driver-2f-src:
	-$(RM) ./CAN\ Driver/src/can_stm32.cyclo ./CAN\ Driver/src/can_stm32.d ./CAN\ Driver/src/can_stm32.o ./CAN\ Driver/src/can_stm32.su

.PHONY: clean-CAN-20-Driver-2f-src

