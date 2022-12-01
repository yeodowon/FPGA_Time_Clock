################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/Switch/subdir.mk
-include src/Button/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
ELFSIZE += \
StepMotor_Buzz_FND.elf.size \


# All Target
all: StepMotor_Buzz_FND.elf secondary-outputs

# Tool invocations
StepMotor_Buzz_FND.elf: $(OBJS) ../src/lscript.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MicroBlaze gcc linker'
	mb-gcc -Wl,-T -Wl,../src/lscript.ld -LD:/project/fpga/StepMotor_Buzz_FND_vitis/Design_StepMotor_Buzz_FND_wrapper/export/Design_StepMotor_Buzz_FND_wrapper/sw/Design_StepMotor_Buzz_FND_wrapper/domain_microblaze_0/bsplib/lib -mlittle-endian -mcpu=v11.0 -mxl-soft-mul -Wl,--no-relax -Wl,--gc-sections -o "StepMotor_Buzz_FND.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

StepMotor_Buzz_FND.elf.size: StepMotor_Buzz_FND.elf
	@echo 'Invoking: MicroBlaze Print Size'
	mb-size StepMotor_Buzz_FND.elf  |tee "StepMotor_Buzz_FND.elf.size"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(S_UPPER_DEPS)$(C_DEPS)$(ELFSIZE) StepMotor_Buzz_FND.elf
	-@echo ' '

secondary-outputs: $(ELFSIZE)

.PHONY: all clean dependents

-include ../makefile.targets
