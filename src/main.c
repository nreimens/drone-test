#include "nrfx_systick.h"
#include "nrf_gpio.h"

#define LED_PIN NRF_GPIO_PIN_MAP(0,11)

int main(void)
{
    nrfx_systick_init();
    nrf_gpio_cfg_output(LED_PIN);
    while (1)
    {
        nrfx_systick_delay_ms(100);
        nrf_gpio_pin_toggle(LED_PIN);
    }
}