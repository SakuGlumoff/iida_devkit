file(GLOB SOURCES
	${CMAKE_CURRENT_LIST_DIR}/../drivers/*.cpp
	${CMAKE_CURRENT_LIST_DIR}/../lib/DebugPrint-GCC/debug_print.c
	${CMAKE_CURRENT_LIST_DIR}/*.cpp
	${CMAKE_CURRENT_LIST_DIR}/*.s
)

add_executable(bootloader ${SOURCES})

set_target_properties(bootloader PROPERTIES
	OUTPUT_NAME "bootloader"
	SUFFIX ".elf"
)

target_include_directories(bootloader PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}/../include
	${CMAKE_CURRENT_BINARY_DIR}/include
	${CMAKE_CURRENT_LIST_DIR}/include
	${CMAKE_CURRENT_LIST_DIR}/../include
	${CMAKE_CURRENT_LIST_DIR}/../drivers/include
	${CMAKE_CURRENT_LIST_DIR}/../lib/CMSIS_5/CMSIS/Core/Include
	${CMAKE_CURRENT_LIST_DIR}/../lib/DebugPrint-GCC
)

target_compile_options(bootloader PRIVATE
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
	$<$<BOOL:QEMU>:-DQEMU>
)

target_link_options(bootloader PRIVATE
	${MCU_OPT}
	-Wl,--start-group
	-lc
	-lm
	-lstdc++
	-lnosys
	-Wl,--end-group
	-specs=nano.specs
	-specs=nosys.specs
	-T ${CMAKE_CURRENT_BINARY_DIR}/bootloader.ld
	-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/bootloader.map,--cref
	-Wl,--gc-sections
	-Wl,--no-warn-rwx-segments
	-Wl,--print-memory-usage
)

target_link_libraries(bootloader PRIVATE etl::etl)

configure_file(${CMAKE_CURRENT_LIST_DIR}/bootloader.ld.in ${CMAKE_CURRENT_BINARY_DIR}/bootloader.ld)
configure_file(${CMAKE_CURRENT_LIST_DIR}/include/memorymap.hpp.in include/memorymap.hpp)

add_custom_command(TARGET bootloader POST_BUILD
	COMMAND ${CMAKE_OBJDUMP} -D ${CMAKE_CURRENT_BINARY_DIR}/bootloader.elf > ${CMAKE_CURRENT_BINARY_DIR}/bootloader.lst
	COMMAND ${CMAKE_OBJCOPY} -O ihex ${CMAKE_CURRENT_BINARY_DIR}/bootloader.elf ${CMAKE_CURRENT_BINARY_DIR}/bootloader.hex
)