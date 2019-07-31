cmake_minimum_required(VERSION 3.14)
project(thumbdv)

set(CMAKE_C_STANDARD 11)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION gnu)
set(CMAKE_VERBOSE_MAKEFILE ON)

## Setting up the four types of builds we have
set(CMAKE_CONFIGURATION_TYPES "ThumbDV_Debug;ThumbDV_Release;Test_Dongle" CACHE STRING "" FORCE)

include_directories(.)
include_directories(include)
include_directories(ThumbDV)
include_directories(SmartSDR_Interface)

link_directories(/src/flex/smoothlake/firmware/src/Firmware/lib
        /src/flex/smartsdr-dsp/DSP_API/lib)

add_executable(thumbdv
        SmartSDR_Interface/cmd_basics.c
        SmartSDR_Interface/cmd_engine.c
        SmartSDR_Interface/discovery_client.c
        SmartSDR_Interface/hal_buffer.c
        SmartSDR_Interface/hal_listener.c
        SmartSDR_Interface/hal_vita.c
        SmartSDR_Interface/io_utils.c
        SmartSDR_Interface/sched_waveform.c
        SmartSDR_Interface/smartsdr_dsp_api.c
        SmartSDR_Interface/status_processor.c
        SmartSDR_Interface/traffic_cop.c
        SmartSDR_Interface/utils.c
        SmartSDR_Interface/vita_output.c
        ThumbDV/bit_pattern_matcher.c
        ThumbDV/dstar.c
        ThumbDV/gmsk_modem.c
        ThumbDV/slow_data.c
        ThumbDV/thumbDV.c
        circular_buffer.c
        resampler.c
        main.c)


if (CMAKE_BUILD_TYPE MATCHES ThumbDV_Release)
    target_compile_options(thumbdv PRIVATE
            -U_FORTIFY_SOURCE
            -O3
            -mcpu=cortex-a8
            -mfpu=neon
            -mfloat-abi=softfp
            -ftree-vectorize
            -Wall
            -c
            -fmessage-length=0
            -lpthread
            -MMD
            -MP
            -MF
            )
elseif(CMAKE_BUILD_TYPE MATCHES Test_Dongle)
    target_compile_options(thumbdv PRIVATE
            -U_FORTIFY_SOURCE
            -O3
            #-mcpu=cortex-a8
            #-mfpu=neon
            #-mfloat-abi=softfp
            -ftree-vectorize
            -Wall
            -c
            -fmessage-length=0
            -lpthread
            -MMD
            -MP
            -MF
            )
elseif(CMAKE_BUILD_TYPE MATCHES ThumbDV_Debug)
    target_compile_options(thumbdv PRIVATE
            -O0
            -mcpu=cortex-a8
            -mfpu=neon
            -mfloat-abi=softfp
            -ftree-vectorize
            -Wall
            -ggdb
            -g3
            -fstack-protector-all
            -funwind-tables
            -c
            -fmessage-length=0
            -lpthread
            -MMD
            -MP
            -MF
            )
    target_compile_definitions(thumbdv PRIVATE
            DEBUG)

elseif(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_options(thumbdv PRIVATE
            -O0
            -ftree-vectorize
            -g3
            -ggdb
            -g3
            -fstack-protector-all
            -funwind-tables
            -Wall
            -c
            -fmessage-length=0
            -lpthread
            -MMD
            -MP
            -MF
            )
    target_compile_definitions(thumbdv PRIVATE
            DEBUG)
    
endif()

TARGET_LINK_LIBRARIES(thumbdv PRIVATE
        ftd2xx
        rt
        pthread
        dl
        m)


target_link_options(thumbdv INTERFACE
        -Map=Firmware.map)


add_custom_command(TARGET thumbdv POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:thumbdv> /nfsroots/microburst/home/root)