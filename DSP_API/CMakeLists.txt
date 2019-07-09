cmake_minimum_required(VERSION 3.14)
project(thumbdv)

set(CMAKE_C_STANDARD 11)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION gnu)

## Setting up the four types of builds we have
set(CMAKE_CONFIGURATION_TYPES "ThumbDV_Debug;ThumbDV_Release" CACHE STRING "" FORCE)

include_directories(.)
include_directories(include)
include_directories(lib)
include_directories(SmartSDR_Interface)
include_directories(ThumbDV)

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
        main.c
        resampler.c)

add_custom_command(TARGET thumbdv POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:thumbdv> /nfsroots/microburst/home/root)