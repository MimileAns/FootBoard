################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/drivers/drv_CAN_bus.c \
../Core/Src/drivers/drv_ICM-42670.c 

OBJS += \
./Core/Src/drivers/drv_CAN_bus.o \
./Core/Src/drivers/drv_ICM-42670.o 

C_DEPS += \
./Core/Src/drivers/drv_CAN_bus.d \
./Core/Src/drivers/drv_ICM-42670.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/drivers/%.o Core/Src/drivers/%.su Core/Src/drivers/%.cyclo: ../Core/Src/drivers/%.c Core/Src/drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-drivers

clean-Core-2f-Src-2f-drivers:
	-$(RM) ./Core/Src/drivers/drv_CAN_bus.cyclo ./Core/Src/drivers/drv_CAN_bus.d ./Core/Src/drivers/drv_CAN_bus.o ./Core/Src/drivers/drv_CAN_bus.su ./Core/Src/drivers/drv_ICM-42670.cyclo ./Core/Src/drivers/drv_ICM-42670.d ./Core/Src/drivers/drv_ICM-42670.o ./Core/Src/drivers/drv_ICM-42670.su

.PHONY: clean-Core-2f-Src-2f-drivers

