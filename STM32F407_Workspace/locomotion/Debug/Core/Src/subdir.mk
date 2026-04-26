################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/bot.c \
../Core/Src/kfs_arm_control.c \
../Core/Src/locomotion.c \
../Core/Src/main.c \
../Core/Src/staff_arm_control.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/test_code.c \
../Core/Src/user.c 

OBJS += \
./Core/Src/bot.o \
./Core/Src/kfs_arm_control.o \
./Core/Src/locomotion.o \
./Core/Src/main.o \
./Core/Src/staff_arm_control.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/test_code.o \
./Core/Src/user.o 

C_DEPS += \
./Core/Src/bot.d \
./Core/Src/kfs_arm_control.d \
./Core/Src/locomotion.d \
./Core/Src/main.d \
./Core/Src/staff_arm_control.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/test_code.d \
./Core/Src/user.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../USB_HOST/App -I../USB_HOST/Target -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/bot.cyclo ./Core/Src/bot.d ./Core/Src/bot.o ./Core/Src/bot.su ./Core/Src/kfs_arm_control.cyclo ./Core/Src/kfs_arm_control.d ./Core/Src/kfs_arm_control.o ./Core/Src/kfs_arm_control.su ./Core/Src/locomotion.cyclo ./Core/Src/locomotion.d ./Core/Src/locomotion.o ./Core/Src/locomotion.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/staff_arm_control.cyclo ./Core/Src/staff_arm_control.d ./Core/Src/staff_arm_control.o ./Core/Src/staff_arm_control.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/test_code.cyclo ./Core/Src/test_code.d ./Core/Src/test_code.o ./Core/Src/test_code.su ./Core/Src/user.cyclo ./Core/Src/user.d ./Core/Src/user.o ./Core/Src/user.su

.PHONY: clean-Core-2f-Src

