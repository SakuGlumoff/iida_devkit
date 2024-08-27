#pragma once

#include "debug_print.h"

#ifdef __cplusplus
extern "C" {
#endif
	void Panic(void) __attribute__((__noreturn__));
#ifdef __cplusplus
}
#endif

#define PANIC(fmt, ...)                                                        \
	DBG_PRINTF_ERROR(fmt, ##__VA_ARGS__);                                      \
	Panic()
