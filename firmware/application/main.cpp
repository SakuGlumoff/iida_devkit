#include "FreeRTOS.h"
#include "SEGGER_RTT.h"
#include "common.hpp"
#include "config.hpp"
#include "debug_print.h"
#include "gpio.hpp"
#include "pinmap.hpp"
#include "system.hpp"
#include "task.h"

extern "C" int debug_print_callback(char* debugMessage, unsigned int length) {
	unsigned avail = SEGGER_RTT_GetAvailWriteSpace(0);
	if (avail < length) {
		return DBG_PRINT_BUFFER_TOO_SMALL;
	}
	return SEGGER_RTT_Write(0, debugMessage, length);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
	(void)xTask;
	DBG_PRINTF_ERROR("Task stack overflow: %s", pcTaskName);
	while (true) {}
}

/**
 * @brief Initialize system and peripherals.
 */
static void _Init() {
	SystemInit();
	SEGGER_RTT_Init();
}

static constexpr uint32_t LED_TASK_STACK_SIZE = 1'000UL;
static StackType_t        _ledTaskStack[LED_TASK_STACK_SIZE];
static StaticTask_t       _ledTaskBuffer;

static void _ledTask(void* arg) {
	(void)arg;
	Gpio led(
		LED_DBG_A,
		Gpio::Mode::OUTPUT,
		Gpio::Type::PUSH_PULL,
		Gpio::PullUp::PULL_UP,
		Gpio::Speed::LOW
	);
	led = 1;

	while (true) {
		if (led) {
			DBG_PRINTF_TRACE("Setting LED on");
		} else {
			DBG_PRINTF_TRACE("Setting LED off");
		}
		led = !led;
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

int main(void) {
	_Init();

	extern uint32_t const __header_start;
	AppHeader*            header =
		reinterpret_cast<AppHeader*>(const_cast<uint32_t*>(&__header_start));
	DBG_PRINTF_TRACE("Application start");
	DBG_PRINTF_TRACE("Timestamp: %s", header->metadata.timestamp);
	DBG_PRINTF_TRACE("Commit: %s", header->metadata.commit);
	DBG_PRINTF_TRACE("Toolchain: %s", header->metadata.toolchain);
	Sleep(msToTicks(500));

	xTaskCreateStatic(
		_ledTask,
		"LED",
		LED_TASK_STACK_SIZE,
		nullptr,
		tskIDLE_PRIORITY,
		_ledTaskStack,
		&_ledTaskBuffer
	);

	vTaskStartScheduler();

	while (true) {
		__NOP();
	}

	return 0;
}
