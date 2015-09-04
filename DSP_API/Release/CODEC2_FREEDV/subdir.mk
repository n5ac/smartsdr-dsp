################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CODEC2_FREEDV/codebook.c \
../CODEC2_FREEDV/codebookd.c \
../CODEC2_FREEDV/codebookdt.c \
../CODEC2_FREEDV/codebookge.c \
../CODEC2_FREEDV/codebookjnd.c \
../CODEC2_FREEDV/codebookjvm.c \
../CODEC2_FREEDV/codebookvq.c \
../CODEC2_FREEDV/codebookvqanssi.c \
../CODEC2_FREEDV/codec2.c \
../CODEC2_FREEDV/dump.c \
../CODEC2_FREEDV/fdmdv.c \
../CODEC2_FREEDV/fifo.c \
../CODEC2_FREEDV/freedv_api.c \
../CODEC2_FREEDV/golay23.c \
../CODEC2_FREEDV/interp.c \
../CODEC2_FREEDV/kiss_fft.c \
../CODEC2_FREEDV/lpc.c \
../CODEC2_FREEDV/lsp.c \
../CODEC2_FREEDV/nlp.c \
../CODEC2_FREEDV/pack.c \
../CODEC2_FREEDV/phase.c \
../CODEC2_FREEDV/postfilter.c \
../CODEC2_FREEDV/quantise.c \
../CODEC2_FREEDV/sine.c \
../CODEC2_FREEDV/varicode.c 

OBJS += \
./CODEC2_FREEDV/codebook.o \
./CODEC2_FREEDV/codebookd.o \
./CODEC2_FREEDV/codebookdt.o \
./CODEC2_FREEDV/codebookge.o \
./CODEC2_FREEDV/codebookjnd.o \
./CODEC2_FREEDV/codebookjvm.o \
./CODEC2_FREEDV/codebookvq.o \
./CODEC2_FREEDV/codebookvqanssi.o \
./CODEC2_FREEDV/codec2.o \
./CODEC2_FREEDV/dump.o \
./CODEC2_FREEDV/fdmdv.o \
./CODEC2_FREEDV/fifo.o \
./CODEC2_FREEDV/freedv_api.o \
./CODEC2_FREEDV/golay23.o \
./CODEC2_FREEDV/interp.o \
./CODEC2_FREEDV/kiss_fft.o \
./CODEC2_FREEDV/lpc.o \
./CODEC2_FREEDV/lsp.o \
./CODEC2_FREEDV/nlp.o \
./CODEC2_FREEDV/pack.o \
./CODEC2_FREEDV/phase.o \
./CODEC2_FREEDV/postfilter.o \
./CODEC2_FREEDV/quantise.o \
./CODEC2_FREEDV/sine.o \
./CODEC2_FREEDV/varicode.o 

C_DEPS += \
./CODEC2_FREEDV/codebook.d \
./CODEC2_FREEDV/codebookd.d \
./CODEC2_FREEDV/codebookdt.d \
./CODEC2_FREEDV/codebookge.d \
./CODEC2_FREEDV/codebookjnd.d \
./CODEC2_FREEDV/codebookjvm.d \
./CODEC2_FREEDV/codebookvq.d \
./CODEC2_FREEDV/codebookvqanssi.d \
./CODEC2_FREEDV/codec2.d \
./CODEC2_FREEDV/dump.d \
./CODEC2_FREEDV/fdmdv.d \
./CODEC2_FREEDV/fifo.d \
./CODEC2_FREEDV/freedv_api.d \
./CODEC2_FREEDV/golay23.d \
./CODEC2_FREEDV/interp.d \
./CODEC2_FREEDV/kiss_fft.d \
./CODEC2_FREEDV/lpc.d \
./CODEC2_FREEDV/lsp.d \
./CODEC2_FREEDV/nlp.d \
./CODEC2_FREEDV/pack.d \
./CODEC2_FREEDV/phase.d \
./CODEC2_FREEDV/postfilter.d \
./CODEC2_FREEDV/quantise.d \
./CODEC2_FREEDV/sine.d \
./CODEC2_FREEDV/varicode.d 


# Each subdirectory must supply rules for building sources it contributes
CODEC2_FREEDV/%.o: ../CODEC2_FREEDV/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-linux-gnueabihf-gcc -static -I"/src/flex-git/smartsdr-dsp/DSP_API" -I"/src/flex-git/smartsdr-dsp/DSP_API/SmartSDR_Interface" -I"/src/flex-git/smartsdr-dsp/DSP_API/CODEC2_FREEDV" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


