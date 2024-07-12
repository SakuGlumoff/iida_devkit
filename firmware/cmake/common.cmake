# Calculate and assign variables for CMake to generate linker scripts and headers.

set(SEGGER_RTT_START ${RAM_BASE})
set(SEGGER_RTT_SIZE 0x500)

set(BL_FLASH_START ${FLASH_BASE})
# Keep flash size in 4Kb (4096 / 0x1000) increments (page size) to ease flash usage.
set(BL_FLASH_SIZE 0x4000)
MATH(EXPR BL_RAM_START "${SEGGER_RTT_START} + ${SEGGER_RTT_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
MATH(EXPR BL_RAM_SIZE "${RAM_SIZE} - ${SEGGER_RTT_SIZE}" OUTPUT_FORMAT HEXADECIMAL)

MATH(EXPR APP_FLASH_START "${BL_FLASH_START} + ${BL_FLASH_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
set(APP_FLASH_SIZE 0x70000)
MATH(EXPR APP_RAM_START "${BL_RAM_START}" OUTPUT_FORMAT HEXADECIMAL)
MATH(EXPR APP_RAM_SIZE "${RAM_SIZE} - ${SEGGER_RTT_SIZE}" OUTPUT_FORMAT HEXADECIMAL)

# Since the interrupt vector table is at the beginning, ensure that the application code starts at the next 32 word boundary.
# The application code starts after the header so align the header size to 32 words.
set(APP_HEADER_SIZE 0x100)
set(APP_HEADER_START ${APP_FLASH_START})
MATH(EXPR APP_CODE_START "${APP_HEADER_START} + ${APP_HEADER_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
set(APP_CODE_SIZE 0x20000)

# Application header ID for bootloader to recognize a valid image.
set(APP_HEADER_ID 0xDEADBEEF)
