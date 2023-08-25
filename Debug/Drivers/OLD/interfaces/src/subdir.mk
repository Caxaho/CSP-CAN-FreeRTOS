################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/OLD/interfaces/src/csp_if_can_stm32.c 

OBJS += \
./Drivers/OLD/interfaces/src/csp_if_can_stm32.o 

C_DEPS += \
./Drivers/OLD/interfaces/src/csp_if_can_stm32.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/OLD/interfaces/src/%.o Drivers/OLD/interfaces/src/%.su Drivers/OLD/interfaces/src/%.cyclo: ../Drivers/OLD/interfaces/src/%.c Drivers/OLD/interfaces/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../Core/Inc -IDrivers/interfaces/src/ -IDrivers/interfaces/include -IDrivers/CAN/src -IDrivers/CAN/include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-OLD-2f-interfaces-2f-src

clean-Drivers-2f-OLD-2f-interfaces-2f-src:
	-$(RM) ./Drivers/OLD/interfaces/src/csp_if_can_stm32.cyclo ./Drivers/OLD/interfaces/src/csp_if_can_stm32.d ./Drivers/OLD/interfaces/src/csp_if_can_stm32.o ./Drivers/OLD/interfaces/src/csp_if_can_stm32.su

.PHONY: clean-Drivers-2f-OLD-2f-interfaces-2f-src

