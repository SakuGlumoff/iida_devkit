file(GLOB SOURCES
	${CMAKE_CURRENT_LIST_DIR}/../drivers/*.cpp
	${CMAKE_CURRENT_LIST_DIR}/../test/selftest.cpp
	${CMAKE_CURRENT_LIST_DIR}/../lib/RTT/RTT/*.c
	${CMAKE_CURRENT_LIST_DIR}/../lib/RTT/RTT/*.S
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
	${CMAKE_CURRENT_LIST_DIR}/../test/include
	${CMAKE_CURRENT_LIST_DIR}/../lib/CMSIS_5/CMSIS/Core/Include
	${CMAKE_CURRENT_LIST_DIR}/../lib/DebugPrint-GCC
	${CMAKE_CURRENT_LIST_DIR}/../lib/RTT/RTT
	${CMAKE_CURRENT_LIST_DIR}/../lib/RTT/Config
)

target_compile_definitions(bootloader PRIVATE
	SEGGER_RTT_SECTION=".segger_rtt"
	SEGGER_RTT_MODE_DEFAULT=SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL
)

target_link_options(bootloader PRIVATE
	-T ${CMAKE_CURRENT_BINARY_DIR}/bootloader.ld
	-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/bootloader.map,--cref
)

target_link_libraries(bootloader PRIVATE etl::etl DEBUGPRINT_GCC)

configure_file(${CMAKE_CURRENT_LIST_DIR}/header.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/header.cpp)
target_sources(bootloader PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/header.cpp)

configure_file(${CMAKE_CURRENT_LIST_DIR}/bootloader.ld.in ${CMAKE_CURRENT_BINARY_DIR}/bootloader.ld)

add_custom_command(TARGET bootloader POST_BUILD
	COMMAND ${CMAKE_OBJDUMP} -D ${CMAKE_CURRENT_BINARY_DIR}/bootloader.elf > ${CMAKE_CURRENT_BINARY_DIR}/bootloader.lst
	COMMAND ${CMAKE_OBJCOPY} -O ihex ${CMAKE_CURRENT_BINARY_DIR}/bootloader.elf ${CMAKE_CURRENT_BINARY_DIR}/bootloader.hex
)