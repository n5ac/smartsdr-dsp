################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SmartSDR_Interface/cmd_basics.c \
../SmartSDR_Interface/cmd_engine.c \
../SmartSDR_Interface/discovery_client.c \
../SmartSDR_Interface/hal_buffer.c \
../SmartSDR_Interface/hal_listener.c \
../SmartSDR_Interface/hal_vita.c \
../SmartSDR_Interface/io_utils.c \
../SmartSDR_Interface/sched_waveform.c \
../SmartSDR_Interface/smartsdr_dsp_api.c \
../SmartSDR_Interface/status_processor.c \
../SmartSDR_Interface/traffic_cop.c \
../SmartSDR_Interface/utils.c \
../SmartSDR_Interface/vita_output.c 

OBJS += \
./SmartSDR_Interface/cmd_basics.o \
./SmartSDR_Interface/cmd_engine.o \
./SmartSDR_Interface/discovery_client.o \
./SmartSDR_Interface/hal_buffer.o \
./SmartSDR_Interface/hal_listener.o \
./SmartSDR_Interface/hal_vita.o \
./SmartSDR_Interface/io_utils.o \
./SmartSDR_Interface/sched_waveform.o \
./SmartSDR_Interface/smartsdr_dsp_api.o \
./SmartSDR_Interface/status_processor.o \
./SmartSDR_Interface/traffic_cop.o \
./SmartSDR_Interface/utils.o \
./SmartSDR_Interface/vita_output.o 

C_DEPS += \
./SmartSDR_Interface/cmd_basics.d \
./SmartSDR_Interface/cmd_engine.d \
./SmartSDR_Interface/discovery_client.d \
./SmartSDR_Interface/hal_buffer.d \
./SmartSDR_Interface/hal_listener.d \
./SmartSDR_Interface/hal_vita.d \
./SmartSDR_Interface/io_utils.d \
./SmartSDR_Interface/sched_waveform.d \
./SmartSDR_Interface/smartsdr_dsp_api.d \
./SmartSDR_Interface/status_processor.d \
./SmartSDR_Interface/traffic_cop.d \
./SmartSDR_Interface/utils.d \
./SmartSDR_Interface/vita_output.d 


# Each subdirectory must supply rules for building sources it contributes
SmartSDR_Interface/%.o: ../SmartSDR_Interface/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-linux-gnueabihf-gcc -static -I"/src/flex-git/smartsdr-dsp/DSP_API" -I"/src/flex-git/smartsdr-dsp/DSP_API/SmartSDR_Interface" -I"/src/flex-git/smartsdr-dsp/DSP_API/CODEC2_FREEDV" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


