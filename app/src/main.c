#include <zephyr/kernel.h>
#include <zephyr/app_version.h>
#include <zephyr/version.h>
#include <zephyr/autoconf.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#include "main.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

static const struct gpio_dt_spec ledA = GPIO_DT_SPEC_GET(DT_NODELABEL(led_a), gpios);

int main(void)
{
	LOG_INF("Iida devkit application: %s %s\n", APP_VERSION_STRING,
		STRINGIFY(APP_BUILD_VERSION));
	LOG_INF("Zephyr: %s\n", STRINGIFY(BUILD_VERSION));
	LOG_INF("Board: %s\n", CONFIG_BOARD_TARGET);
	LOG_INF("Built on %s @ %s\n", __DATE__, __TIME__);

	int err = 0;
	err = gpio_pin_configure_dt(&ledA, GPIO_OUTPUT_ACTIVE);
	if (err) {
		LOG_ERR("Error configuring LED A: %d", err);
		return err;
	}

	while (1) {
		err = gpio_pin_toggle_dt(&ledA);
		if (err) {
			LOG_ERR("Error toggling LED A: %d", err);
		}
		k_msleep(1000);
	}
}
