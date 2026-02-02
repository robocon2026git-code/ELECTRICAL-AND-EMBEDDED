################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/My_Drivers/src/stm32f407xx_gpio.c 

OBJS += \
./Drivers/My_Drivers/src/stm32f407xx_gpio.o 

C_DEPS += \
./Drivers/My_Drivers/src/stm32f407xx_gpio.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/My_Drivers/src/%.o Drivers/My_Drivers/src/%.su Drivers/My_Drivers/src/%.cyclo: ../Drivers/My_Drivers/src/%.c Drivers/My_Drivers/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"/home/robocon2026/ELECTO_CORE/workspace_git/ELECTRICAL-AND-EMBEDDED/STM32F407_Workspace/code_x_lib/Drivers/My_Drivers/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-My_Drivers-2f-src

clean-Drivers-2f-My_Drivers-2f-src:
	-$(RM) ./Drivers/My_Drivers/src/stm32f407xx_gpio.cyclo ./Drivers/My_Drivers/src/stm32f407xx_gpio.d ./Drivers/My_Drivers/src/stm32f407xx_gpio.o ./Drivers/My_Drivers/src/stm32f407xx_gpio.su

.PHONY: clean-Drivers-2f-My_Drivers-2f-src

