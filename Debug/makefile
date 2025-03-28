################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utils/subdir.mk
-include startup/subdir.mk
-include src/subdir.mk
-include reconstruction/subdir.mk
-include processing/subdir.mk
-include ipc/subdir.mk
-include hal/src/subdir.mk
-include driverUser/subdir.mk
-include comunication_SPIS/subdir.mk
-include comunication/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(SX_DEPS)),)
-include $(SX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := hydr_cv32e40p_ahmmod_FW_standalone
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
hydr_cv32e40p_ahmmod_FW_standalone.hex \

SECONDARY_SIZE += \
hydr_cv32e40p_ahmmod_FW_standalone.siz \


# All Target
all: main-build

# Main-build Target
main-build: hydr_cv32e40p_ahmmod_FW_standalone.elf secondary-outputs

# Tool invocations
hydr_cv32e40p_ahmmod_FW_standalone.elf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU RISC-V Cross C Linker'
	riscv32-unknown-elf-gcc -march=rv32imfc_xcorev_xfhalf -msmall-data-limit=8 -mno-save-restore -O1 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g3 -T "/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/ld/link_sectors.ld" -T "/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/ld/link_hydr_cv32e40_wcopr.ld" -Xlinker --gc-sections -L"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/hal/libs" -L"/home/andream/projects/Hydrogen_FW/application/hydr_cv32e40p_ahmmod_FW_standalone/dsp/libs" -Wl,-Map,"hydr_cv32e40p_ahmmod_FW_standalone.map" --specs=nano.specs --specs=nosys.specs -o "hydr_cv32e40p_ahmmod_FW_standalone.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

hydr_cv32e40p_ahmmod_FW_standalone.hex: hydr_cv32e40p_ahmmod_FW_standalone.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: GNU RISC-V Cross Create Flash Image'
	riscv32-unknown-elf-objcopy -O ihex "hydr_cv32e40p_ahmmod_FW_standalone.elf"  "hydr_cv32e40p_ahmmod_FW_standalone.hex"
	@echo 'Finished building: $@'
	@echo ' '

hydr_cv32e40p_ahmmod_FW_standalone.siz: hydr_cv32e40p_ahmmod_FW_standalone.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: GNU RISC-V Cross Print Size'
	riscv32-unknown-elf-size --format=berkeley "hydr_cv32e40p_ahmmod_FW_standalone.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SX_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) hydr_cv32e40p_ahmmod_FW_standalone.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents main-build

-include ../makefile.targets
