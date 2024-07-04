file(GLOB SOURCES
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/crc.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/gpio.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/hash.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/i2c.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/icache.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/main.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/octospi.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/rng.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/rtc.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/stm32l5xx_hal_msp.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/stm32l5xx_it.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/syscalls.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/sysmem.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/system_stm32l5xx.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/usart.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/usb.c
	${CMAKE_CURRENT_LIST_DIR}/Drivers/STM32L5xx_HAL_Driver/Src/*.c
	${CMAKE_CURRENT_LIST_DIR}/Drivers/STM32L5xx_HAL_Driver/Src/*.c
	${CMAKE_CURRENT_LIST_DIR}/*.s
)

list(REMOVE_ITEM SOURCES
	${CMAKE_CURRENT_LIST_DIR}/Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_msp_template.c
	${CMAKE_CURRENT_LIST_DIR}/Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_timebase_rtc_wakeup_template.c
	${CMAKE_CURRENT_LIST_DIR}/Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_timebase_tim_template.c
)

add_executable(ptsw ${SOURCES})

set_target_properties(ptsw PROPERTIES
	OUTPUT_NAME "ptsw"
	SUFFIX ".elf"
)

target_include_directories(ptsw PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/Core/Inc
	${CMAKE_CURRENT_LIST_DIR}/Drivers/STM32L5xx_HAL_Driver/Inc
	${CMAKE_CURRENT_LIST_DIR}/Drivers/STM32L5xx_HAL_Driver/Inc/Legacy
	${CMAKE_CURRENT_LIST_DIR}/Drivers/CMSIS/Device/ST/STM32L5xx/Include
	${CMAKE_CURRENT_LIST_DIR}/Drivers/CMSIS/Include
)

target_compile_definitions(ptsw PRIVATE
	USE_HAL_DRIVER
	STM32L552xx
)

target_compile_options(ptsw PRIVATE
	${MCU_OPT}
	-fdata-sections
	-ffunction-sections
	-Wall
	$<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions -fno-rtti>
	$<$<CONFIG:Debug>:-gdwarf-2>
	$<$<CONFIG:Release>:-Og -g0>
)

target_link_options(ptsw PRIVATE
	${MCU_OPT}
	-Wl,--start-group
	-lc
	-lm
	-Wl,--end-group
	-specs=nano.specs
	-T ${CMAKE_CURRENT_LIST_DIR}/STM32L552xE_FLASH.ld
	-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/ptsw.map,--cref
	-Wl,--gc-sections
	-Wl,--print-memory-usage
)

add_custom_command(TARGET ptsw POST_BUILD
	COMMAND ${CMAKE_OBJDUMP} -D ${CMAKE_CURRENT_BINARY_DIR}/ptsw.elf > ${CMAKE_CURRENT_BINARY_DIR}/ptsw.lst
	COMMAND ${CMAKE_OBJCOPY} -O ihex ${CMAKE_CURRENT_BINARY_DIR}/ptsw.elf ${CMAKE_CURRENT_BINARY_DIR}/ptsw.hex
)