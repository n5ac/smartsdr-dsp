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
	arm-angstrom-linux-gnueabi-gcc -I"/src/flex/smartsdr-dsp/DSP_API/CODEC2_FREEDV" -I"/src/flex/smartsdr-dsp/DSP_API" -I"/src/flex/smartsdr-dsp/DSP_API/SmartSDR_Interface" -O3 -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -ftree-vectorize -Wall -c -fmessage-length=0 -lpthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


