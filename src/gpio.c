#include "gpio.h"

uint8_t get_port_index(gpio_t *port)
{
    // CRITICAL FIX: Cast pointers to integers BEFORE doing arithmetic.
    uint32_t port_addr = (uint32_t)port;
    uint32_t base_addr = (uint32_t)GPIOA;

    // Now we are doing simple integer math.
    if (port_addr >= base_addr && port_addr <= (uint32_t)GPIOH) {
        return (port_addr - base_addr) / 0x400;
    }
    return 0xFF; // Invalid port
}

void gpio_init(const gpio_config_t *config)
{
    gpio_t *GPIOx = config->port;
    uint8_t pin = config->pin;
    gpio_mode_t mode = config->mode;

	if (config == NULL || GPIOx == NULL || pin > 15) {
        return;
    }

    // 1. Enable the clock for the GPIO port
    rcc_gpio_clock_enable(get_port_index(config->port));

    // 2. Configure pin mode
    GPIOx->MODER &= ~(3U << (2 * pin));
    GPIOx->MODER |= (mode << (2 * pin));

    // 3. Configure Output Type and Speed
    if(mode == GPIO_MODE_OUTPUT || mode == GPIO_MODE_ALTERNATE) {
        // Output Type (Push-Pull or Open-Drain)
        GPIOx->OTYPER &= ~(1U << pin);
        GPIOx->OTYPER |= (config->otype << pin);

        // Output Speed
        GPIOx->OSPEEDR &= ~(3U << (pin * 2));
        GPIOx->OSPEEDR |= (config->ospeed << (pin * 2));
    }

    // 4. Configure Pull-up / Pull-down Resistor
    GPIOx->PUPDR &= ~(3U << (2 * pin));
    GPIOx->PUPDR |= (config->pupd << (2 * pin));

    // 5. Configure Alternate Function
    if(mode == GPIO_MODE_ALTERNATE) {
        uint8_t af_reg_index = pin / 8; // 0 for AFR[0], 1 for AFR[1]
        uint8_t pin_in_reg = pin % 8;   // Pin position within the register (0-7)

        GPIOx->AFR[af_reg_index] &= ~(0xFU << (4 * pin_in_reg));
        GPIOx->AFR[af_reg_index] |= (config->alt_func << (4 * pin_in_reg));
    }
}

void gpio_set_pin(gpio_t *GPIOx, uint8_t pin)
{
    GPIOx->BSRR = (1U << pin);
}

void gpio_reset_pin(gpio_t *GPIOx, uint8_t pin)
{
    GPIOx->BRR = (1U << pin);
}

void gpio_toggle_pin(gpio_t *GPIOx, uint8_t pin)
{
    GPIOx->ODR ^= (1U << pin);
}

uint8_t gpio_read_pin(gpio_t *GPIOx, uint8_t pin)
{
    if((GPIOx->IDR & (1U << pin)) != 0)
        return 1;
    return 0;
}
