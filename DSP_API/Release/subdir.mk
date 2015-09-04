################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../circular_buffer.c \
../main.c \
../resampler.c 

OBJS += \
./circular_buffer.o \
./main.o \
./resampler.o 

C_DEPS += \
./circular_buffer.d \
./main.d \
./resampler.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-linux-gnueabihf-gcc -static -I"/src/flex-git/smartsdr-dsp/DSP_API" -I"/src/flex-git/smartsdr-dsp/DSP_API/SmartSDR_Interface" -I"/src/flex-git/smartsdr-dsp/DSP_API/CODEC2_FREEDV" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


