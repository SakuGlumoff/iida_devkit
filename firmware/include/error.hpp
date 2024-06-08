#pragma once

#include "debug_print.h"

#define SystemError(fmt, ...)                                                  \
	DBG_PRINTF_ERROR(fmt, ##__VA_ARGS__);                                  \
	System::ErrorHandler()
