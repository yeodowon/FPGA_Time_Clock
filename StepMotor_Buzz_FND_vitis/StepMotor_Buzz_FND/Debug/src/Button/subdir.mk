################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Button/Button.c 

OBJS += \
./src/Button/Button.o 

C_DEPS += \
./src/Button/Button.d 


# Each subdirectory must supply rules for building sources it contributes
src/Button/%.o: ../src/Button/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MicroBlaze gcc compiler'
	mb-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -ID:/project/fpga/StepMotor_Buzz_FND_vitis/Design_StepMotor_Buzz_FND_wrapper/export/Design_StepMotor_Buzz_FND_wrapper/sw/Design_StepMotor_Buzz_FND_wrapper/domain_microblaze_0/bspinclude/include -mlittle-endian -mcpu=v11.0 -mxl-soft-mul -Wl,--no-relax -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


