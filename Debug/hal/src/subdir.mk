################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hal/src/hal_advtmr.c \
../hal/src/hal_apbtmr.c \
../hal/src/hal_copctl.c \
../hal/src/hal_evt_ctl.c \
../hal/src/hal_gpio.c \
../hal/src/hal_intc.c \
../hal/src/hal_pwm.c \
../hal/src/hal_spi.c \
../hal/src/hal_uart.c \
../hal/src/hal_udma.c 

OBJS += \
./hal/src/hal_advtmr.o \
./hal/src/hal_apbtmr.o \
./hal/src/hal_copctl.o \
./hal/src/hal_evt_ctl.o \
./hal/src/hal_gpio.o \
./hal/src/hal_intc.o \
./hal/src/hal_pwm.o \
./hal/src/hal_spi.o \
./hal/src/hal_uart.o \
./hal/src/hal_udma.o 

C_DEPS += \
./hal/src/hal_advtmr.d \
./hal/src/hal_apbtmr.d \
./hal/src/hal_copctl.d \
./hal/src/hal_evt_ctl.d \
./hal/src/hal_gpio.d \
./hal/src/hal_intc.d \
./hal/src/hal_pwm.d \
./hal/src/hal_spi.d \
./hal/src/hal_uart.d \
./hal/src/hal_udma.d 


# Each subdirectory must supply rules for building sources it contributes
hal/src/%.o: ../hal/src/%.c hal/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv32-unknown-elf-gcc -march=rv32imfc_xcorev_xfhalf -msmall-data-limit=8 -mno-save-restore -O1 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g3 -DHWREV=0x00010001 -DCORE_CV -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/common" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/ipc" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/comunication" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/comunication_SPIS" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/driverUser" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/dsp/inc" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/hal/inc" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/hw" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/processing" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/reconstruction" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/src" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/startup" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/utils" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


