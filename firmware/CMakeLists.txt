cmake_minimum_required(VERSION 3.23)

project(iida_devkit)

set(${CMAKE_PROJECT_NAME}_VERSION_MAJOR 1)
set(${CMAKE_PROJECT_NAME}_VERSION_MINOR 0)
set(${CMAKE_PROJECT_NAME}_VERSION_FIX 0)

enable_language(C CXX ASM)

set(MCU_OPT
	-mcpu=cortex-m33
	-mfpu=fpv4-sp-d16
	-mfloat-abi=hard
	-mthumb
)

option(CONFIG_HSE "Use HSE" OFF)
option(CONFIG_EXT_FLASH "Use external flash" OFF)
option(CONFIG_MCP9808 "Use MCP9808" OFF)
option(CONFIG_SELFTEST "Enable selftest" OFF)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/stm32l552xx/memory.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/common.cmake)

configure_file(${CMAKE_CURRENT_LIST_DIR}/include/config.hpp.in include/config.hpp)
configure_file(${CMAKE_CURRENT_LIST_DIR}/include/memorymap.hpp.in include/memorymap.hpp)
configure_file(${CMAKE_CURRENT_LIST_DIR}/include/common.hpp.in include/common.hpp)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/etl)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/ptsw)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/application)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/bootloader)
