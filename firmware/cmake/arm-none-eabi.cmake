# Append current directory to CMAKE_MODULE_PATH for making device specific cmake modules visible
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Target definition
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Set toolchain paths
set(TOOLCHAIN arm-none-eabi)
set(TOOLCHAIN_BIN_DIR $ENV{ARM_GNU_PATH}/bin)
set(TOOLCHAIN_INC_DIR $ENV{ARM_GNU_PATH}/${TOOLCHAIN}/include)
set(TOOLCHAIN_LIB_DIR $ENV{ARM_GNU_PATH}/${TOOLCHAIN}/lib)

# Set system depended extensions
if(WIN32)
	set(TOOLCHAIN_EXT ".exe" )
else()
	set(TOOLCHAIN_EXT "" )
endif()

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(MCU_OPT
	-mcpu=cortex-m33
	-mfpu=fpv4-sp-d16
	-mfloat-abi=hard
	-mthumb
)
string(JOIN " " MCU_OPT_STRING ${MCU_OPT})

set(OBJ_GEN_OPT
	-fdata-sections
	-ffunction-sections
	-fno-common
	-Wall
	-Wextra
	-Wpedantic
)
string(JOIN " " OBJ_GEN_OPT_STRING ${OBJ_GEN_OPT})

set(LINK_OPT
	-Wl,--start-group
	-lc -lm -lstdc++ -lnosys
	-Wl,--end-group
	-specs=nano.specs
	-specs=nosys.specs
	-Wl,--gc-sections
	-Wl,--no-warn-rwx-segments
	-Wl,--print-memory-usage
)
string(JOIN " " LINK_OPT_STRING ${LINK_OPT})

set(CMAKE_C_FLAGS "${MCU_OPT_STRING} ${OBJ_GEN_OPT_STRING}" CACHE INTERNAL "C compiler options")
set(CMAKE_CXX_FLAGS "${MCU_OPT_STRING} ${OBJ_GEN_OPT_STRING} -fno-exceptions -fno-rtti" CACHE INTERNAL "C++ compiler options")
set(CMAKE_ASM_FLAGS "${MCU_OPT_STRING} ${OBJ_GEN_OPT_STRING} -x assembler-with-cpp" CACHE INTERNAL "ASM compiler options")
set(CMAKE_EXE_LINKER_FLAGS "${MCU_OPT_STRING} ${LINK_OPT_STRING}" CACHE INTERNAL "Linker options")

set(CMAKE_C_FLAGS_DEBUG "-O0 -g3" CACHE INTERNAL "C compiler options for debug builds")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3" CACHE INTERNAL "C++ compiler options for debug builds")
set(CMAKE_ASM_FLAGS_DEBUG "-O0 -g3" CACHE INTERNAL "ASM compiler options for debug builds")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-O0 -g3" CACHE INTERNAL "Linker options for debug builds")

set(CMAKE_C_FLAGS_RELEASE "-O3 -flto" CACHE INTERNAL "C compiler options for debug builds")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto" CACHE INTERNAL "C++ compiler options for debug builds")
set(CMAKE_ASM_FLAGS_RELEASE "-O3 -flto" CACHE INTERNAL "ASM compiler options for debug builds")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-O3 -flto" CACHE INTERNAL "Linker options for debug builds")

# Set compilers
set(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-gcc${TOOLCHAIN_EXT} CACHE INTERNAL "C Compiler")
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-g++${TOOLCHAIN_EXT} CACHE INTERNAL "C++ Compiler")
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-gcc${TOOLCHAIN_EXT} CACHE INTERNAL "ASM Compiler")

# Set utils
set(CMAKE_OBJDUMP ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-objdump${TOOLCHAIN_EXT} CACHE INTERNAL "Objdump tool")
set(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-objcopy${TOOLCHAIN_EXT} CACHE INTERNAL "Objcopy tool")
set(CMAKE_SIZE ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-size${TOOLCHAIN_EXT} CACHE INTERNAL "Size tool")

set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_BIN_DIR} ${CMAKE_PREFIX_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
