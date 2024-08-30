file(GLOB SOURCES
	${CMAKE_CURRENT_LIST_DIR}/../drivers/*.cpp
	${CMAKE_CURRENT_LIST_DIR}/../test/selftest.cpp
	${CMAKE_CURRENT_LIST_DIR}/../lib/DebugPrint-GCC/debug_print.c
	${CMAKE_CURRENT_LIST_DIR}/../lib/RTT/RTT/*.c
	${CMAKE_CURRENT_LIST_DIR}/../lib/RTT/RTT/*.S
	${CMAKE_CURRENT_LIST_DIR}/*.cpp
	${CMAKE_CURRENT_LIST_DIR}/*.s
)

add_executable(application ${SOURCES})

set_target_properties(application PROPERTIES
	OUTPUT_NAME "application"
	SUFFIX ".elf"
)

target_include_directories(application PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}/../include
	${CMAKE_CURRENT_BINARY_DIR}/include
	${CMAKE_CURRENT_LIST_DIR}/../include
	${CMAKE_CURRENT_LIST_DIR}/../drivers/include
	${CMAKE_CURRENT_LIST_DIR}/../test/include
	${CMAKE_CURRENT_LIST_DIR}/../lib/CMSIS_5/CMSIS/Core/Include
	${CMAKE_CURRENT_LIST_DIR}/../lib/DebugPrint-GCC
	${CMAKE_CURRENT_LIST_DIR}/../lib/RTT/RTT
	${CMAKE_CURRENT_LIST_DIR}/../lib/RTT/Config
)

target_compile_definitions(application PRIVATE
	SEGGER_RTT_SECTION=".segger_rtt"
	SEGGER_RTT_MODE_DEFAULT=SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL
)

target_compile_options(application PRIVATE
	${MCU_OPT}
	-fdata-sections
	-ffunction-sections
	-fno-common
	-Wall
	-Wextra
	-Wpedantic
	$<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions -fno-rtti>
	$<$<CONFIG:Debug>:-O0 -g3>
	$<$<CONFIG:Release>:-Og -g0>
)

target_link_options(application PRIVATE
	${MCU_OPT}
	-Wl,--start-group
	-lc
	-lm
	-lstdc++
	-lnosys
	-Wl,--end-group
	-specs=nano.specs
	-specs=nosys.specs
	-T ${CMAKE_CURRENT_BINARY_DIR}/application.ld
	-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/application.map,--cref
	-Wl,--gc-sections
	-Wl,--no-warn-rwx-segments
	-Wl,--print-memory-usage
)

target_link_libraries(application PRIVATE etl::etl)

configure_file(${CMAKE_CURRENT_LIST_DIR}/header.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/header.cpp)
target_sources(application PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/header.cpp)

configure_file(${CMAKE_CURRENT_LIST_DIR}/application.ld.in ${CMAKE_CURRENT_BINARY_DIR}/application.ld)
configure_file(${CMAKE_CURRENT_LIST_DIR}/postbuild.sh.in ${CMAKE_CURRENT_BINARY_DIR}/postbuild.sh @ONLY)

add_custom_command(TARGET application POST_BUILD
	COMMAND ${CMAKE_OBJDUMP} -D ${CMAKE_CURRENT_BINARY_DIR}/application.elf > ${CMAKE_CURRENT_BINARY_DIR}/application.lst
	COMMAND ${CMAKE_OBJCOPY} -O ihex ${CMAKE_CURRENT_BINARY_DIR}/application.elf ${CMAKE_CURRENT_BINARY_DIR}/application.hex
	COMMAND ${CMAKE_OBJCOPY} -I ihex -O binary ${CMAKE_CURRENT_BINARY_DIR}/application.hex ${CMAKE_CURRENT_BINARY_DIR}/application.bin
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/postbuild.sh
	COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_CURRENT_BINARY_DIR}/application_with_header.elf ${CMAKE_CURRENT_BINARY_DIR}/application_with_header.bin
)