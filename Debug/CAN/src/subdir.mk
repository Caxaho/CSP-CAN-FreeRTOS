################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CAN/src/can_stm32.c 

OBJS += \
./CAN/src/can_stm32.o 

C_DEPS += \
./CAN/src/can_stm32.d 


# Each subdirectory must supply rules for building sources it contributes
CAN/src/%.o CAN/src/%.su CAN/src/%.cyclo: ../CAN/src/%.c CAN/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../Core/Inc -I../Drivers -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -I"C:/dev/STM32_Projects/CSP-CAN-FreeRTOS/CAN/include" -I"C:/dev/STM32_Projects/CSP-CAN-FreeRTOS/interfaces/include" -I"C:/dev/STM32_Projects/CSP-CAN-FreeRTOS/csp_build/include" -I"C:/dev/STM32_Projects/CSP-CAN-FreeRTOS/csp_build/include_csp" -I"C:/dev/STM32_Projects/CSP-CAN-FreeRTOS/csp_build" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CAN-2f-src

clean-CAN-2f-src:
	-$(RM) ./CAN/src/can_stm32.cyclo ./CAN/src/can_stm32.d ./CAN/src/can_stm32.o ./CAN/src/can_stm32.su

.PHONY: clean-CAN-2f-src

