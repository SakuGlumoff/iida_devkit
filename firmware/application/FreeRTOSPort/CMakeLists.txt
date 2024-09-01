add_library(freertos_kernel_port OBJECT)

target_sources(freertos_kernel_port PRIVATE
	mpu_wrappers_v2_asm.c
	port.c
	portasm.c
)

add_library(freertos_kernel_port_headers INTERFACE)
target_include_directories(freertos_kernel_port_headers INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(freertos_kernel_port PRIVATE
	freertos_kernel_port_headers
	freertos_kernel_include
)
