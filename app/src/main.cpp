#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#define SLEEP_TIME_MS 1000

/* The devicetree node identifier for the "led0" alias. */
#define LED_NODE_R DT_ALIAS(app_led_r)
#define LED_NODE_G DT_ALIAS(app_led_g)
#define LED_NODE_B DT_ALIAS(app_led_b)
    



static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE_G, gpios);
static const struct gpio_dt_spec led_g = GPIO_DT_SPEC_GET(LED_NODE_G, gpios);
static const struct gpio_dt_spec led_b = GPIO_DT_SPEC_GET(LED_NODE_B, gpios);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    bool led_state = true;


        

    if (!gpio_is_ready_dt(&led)) return 0;

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) return 0;

   



     while (1) {

        if (gpio_pin_toggle_dt(&led) < 0) return 0;

        led_state = !led_state;
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
        // k_msleep(SLEEP_TIME_MS);
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }
    return 0;
}
