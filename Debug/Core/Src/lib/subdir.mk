################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/lib/lib_kalman.c \
../Core/Src/lib/utils.c 

OBJS += \
./Core/Src/lib/lib_kalman.o \
./Core/Src/lib/utils.o 

C_DEPS += \
./Core/Src/lib/lib_kalman.d \
./Core/Src/lib/utils.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/lib/%.o Core/Src/lib/%.su Core/Src/lib/%.cyclo: ../Core/Src/lib/%.c Core/Src/lib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-lib

clean-Core-2f-Src-2f-lib:
	-$(RM) ./Core/Src/lib/lib_kalman.cyclo ./Core/Src/lib/lib_kalman.d ./Core/Src/lib/lib_kalman.o ./Core/Src/lib/lib_kalman.su ./Core/Src/lib/utils.cyclo ./Core/Src/lib/utils.d ./Core/Src/lib/utils.o ./Core/Src/lib/utils.su

.PHONY: clean-Core-2f-Src-2f-lib

