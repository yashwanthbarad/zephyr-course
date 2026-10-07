#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>

#include "../our_driver/our_driver.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
	const struct device *led = DEVICE_DT_GET(DT_ALIAS(led_sensor));
	struct sensor_value led_state;
	int ret;

	if (!device_is_ready(led)) {
		LOG_ERR("LED sensor is not ready");
		return -ENODEV;
	}

	ret = our_driver_set_led_on_time(led, CONFIG_APP_HEARTBEAT_PERIOD_MS / 2);
	if (ret < 0) {
		LOG_ERR("Failed to configure LED on-time: %d", ret);
		return ret;
	}

	while (1) {
		ret = sensor_sample_fetch(led);
		if (ret < 0) {
			LOG_ERR("Failed to fetch LED state: %d", ret);
			return ret;
		}

		k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

		ret = sensor_channel_get(led, SENSOR_CHAN_PRIV_START, &led_state);
		if (ret < 0) {
			LOG_ERR("Failed to get LED state: %d", ret);
			return ret;
		}

		LOG_INF("LED state before turning it off: %s", led_state.val1 ? "ON" : "OFF");
		k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
	}
}
