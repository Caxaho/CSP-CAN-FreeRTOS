################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../interfaces/src/csp_if_can_stm32.c 

OBJS += \
./interfaces/src/csp_if_can_stm32.o 

C_DEPS += \
./interfaces/src/csp_if_can_stm32.d 


# Each subdirectory must supply rules for building sources it contributes
interfaces/src/%.o interfaces/src/%.su interfaces/src/%.cyclo: ../interfaces/src/%.c interfaces/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../Core/Inc -I../Drivers -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -I"C:/dev/STM32_Projects/CSP-CAN-FreeRTOS/interfaces/include" -I"C:/dev/STM32_Projects/CSP-CAN-FreeRTOS/csp_build/include" -I"C:/dev/STM32_Projects/CSP-CAN-FreeRTOS/csp_build/include_csp" -I"C:/dev/STM32_Projects/CSP-CAN-FreeRTOS/csp_build" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-interfaces-2f-src

clean-interfaces-2f-src:
	-$(RM) ./interfaces/src/csp_if_can_stm32.cyclo ./interfaces/src/csp_if_can_stm32.d ./interfaces/src/csp_if_can_stm32.o ./interfaces/src/csp_if_can_stm32.su

.PHONY: clean-interfaces-2f-src

