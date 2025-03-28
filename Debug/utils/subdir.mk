################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../utils/tbl_trig.c \
../utils/utils_instr_cnt.c \
../utils/wavelet.c 

S_UPPER_SRCS += \
../utils/math_utils.S 

OBJS += \
./utils/math_utils.o \
./utils/tbl_trig.o \
./utils/utils_instr_cnt.o \
./utils/wavelet.o 

S_UPPER_DEPS += \
./utils/math_utils.d 

C_DEPS += \
./utils/tbl_trig.d \
./utils/utils_instr_cnt.d \
./utils/wavelet.d 


# Each subdirectory must supply rules for building sources it contributes
utils/%.o: ../utils/%.S utils/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross Assembler'
	riscv32-unknown-elf-gcc -march=rv32imfc_xcorev_xfhalf -msmall-data-limit=8 -mno-save-restore -O1 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g3 -x assembler-with-cpp -DINCLUDE_COPRBIN -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils/%.o: ../utils/%.c utils/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv32-unknown-elf-gcc -march=rv32imfc_xcorev_xfhalf -msmall-data-limit=8 -mno-save-restore -O1 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g3 -DHWREV=0x00010001 -DCORE_CV -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/common" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/ipc" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/comunication" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/comunication_SPIS" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/driverUser" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/dsp/inc" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/hal/inc" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/hw" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/processing" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/reconstruction" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/src" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/startup" -I"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/utils" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


