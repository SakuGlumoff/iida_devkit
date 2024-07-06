# Calculate and assign variables for CMake to generate linker scripts and headers.

set(SEGGER_RTT_START ${RAM_BASE})
set(SEGGER_RTT_SIZE 0x500)

set(BL_FLASH_START ${FLASH_BASE})
set(BL_FLASH_SIZE 0x4000)
MATH(EXPR BL_RAM_START "${SEGGER_RTT_START} + ${SEGGER_RTT_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
set(BL_RAM_SIZE 0x2000)

MATH(EXPR APP_FLASH_START "${BL_FLASH_START} + ${BL_FLASH_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
set(APP_FLASH_SIZE 0x70000)
MATH(EXPR APP_RAM_START "${BL_RAM_START} + ${BL_RAM_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
set(APP_RAM_SIZE 0x2000)

# Header:
# | Offset | Type | Description                                 |
# |--------|------|---------------------------------------------|
# | 0      | U32  | Identification field (hardcoded 0xDEADBEEF) |
# | 4      | U32  | Size of the image in bytes                  |
# | 8      | U32  | 32-bit CRC value of the image               |
# | 12     | U32  | Reserved                                    |
set(APP_HEADER_SIZE 0x10)
set(APP_HEADER_START ${APP_FLASH_START})
MATH(EXPR APP_CODE_START "${APP_HEADER_START} + ${APP_HEADER_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
set(APP_CODE_SIZE 0x20000)

# Application header ID for bootloader to recognize a valid image.
set(APP_HEADER_ID 0xDEADBEEF)
