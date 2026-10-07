#define DT_DRV_COMPAT example_led_sensor

#include <errno.h>
#include <stdbool.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(led_sensor, CONFIG_SENSOR_LOG_LEVEL);

struct led_sensor_config {
    struct gpio_dt_spec led;
};

struct led_sensor_data {
    bool led_on;
};

static int our_driver_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
    const struct led_sensor_config *config = dev->config;
    struct led_sensor_data *data = dev->data;
    int ret;

    if (chan != SENSOR_CHAN_ALL && chan != SENSOR_CHAN_PRIV_START) {
        return -ENOTSUP;
    }

    ret = gpio_pin_set_dt(&config->led, 1);
    if (ret < 0) {
        LOG_ERR("Failed to turn on LED: %d", ret);
        return ret;
    }

    data->led_on = true;
    return 0;
}

static int our_driver_channel_get(const struct device *dev, enum sensor_channel chan,
                  struct sensor_value *val)
{
    const struct led_sensor_config *config = dev->config;
    struct led_sensor_data *data = dev->data;
    int ret;

    if (chan != SENSOR_CHAN_PRIV_START) {
        return -ENOTSUP;
    }

    val->val1 = data->led_on ? 1 : 0;
    val->val2 = 0;

    ret = gpio_pin_set_dt(&config->led, 0);
    if (ret < 0) {
        LOG_ERR("Failed to turn off LED: %d", ret);
        return ret;
    }

    data->led_on = false;
    return 0;
}

static int led_sensor_init(const struct device *dev)
{
    const struct led_sensor_config *config = dev->config;
    struct led_sensor_data *data = dev->data;

    if (!gpio_is_ready_dt(&config->led)) {
        LOG_ERR("LED GPIO controller is not ready");
        return -ENODEV;
    }

    data->led_on = false;
    return gpio_pin_configure_dt(&config->led, GPIO_OUTPUT_INACTIVE);
}

static DEVICE_API(sensor, our_driver_api) = {
    .sample_fetch = our_driver_sample_fetch,
    .channel_get = our_driver_channel_get,
};

#define LED_SENSOR_DEFINE(inst)							\
    static struct led_sensor_data led_sensor_data_##inst;			\
    static const struct led_sensor_config led_sensor_config_##inst = {	\
        .led = GPIO_DT_SPEC_INST_GET(inst, gpios),			\
    };									\
    DEVICE_DT_INST_DEFINE(inst, led_sensor_init, NULL,			\
                  &led_sensor_data_##inst,				\
                  &led_sensor_config_##inst, POST_KERNEL,		\
                  CONFIG_SENSOR_INIT_PRIORITY, &our_driver_api);

DT_INST_FOREACH_STATUS_OKAY(LED_SENSOR_DEFINE)