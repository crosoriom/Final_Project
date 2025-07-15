#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stddef.h>
#include "rcc.h"

/*Base address for all GPIOs*/
#define GPIOA ((gpio_t *)0x48000000UL)
#define GPIOB ((gpio_t *)0x48000400UL)
#define GPIOC ((gpio_t *)0x48000800UL)
#define GPIOD ((gpio_t *)0x48000C00UL)
#define GPIOE ((gpio_t *)0x48001000UL)
#define GPIOF ((gpio_t *)0x48001400UL)
#define GPIOG ((gpio_t *)0x48001800UL)
#define GPIOH ((gpio_t *)0x48001C00UL)

// --- Configuration Enumerations ---
typedef enum {
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_ALTERNATE,
    GPIO_MODE_ANALOG
} gpio_mode_t;

typedef enum {
    GPIO_OTYPE_PUSHPULL,
    GPIO_OTYPE_OPENDRAIN
} gpio_otype_t;

typedef enum {
    GPIO_OSPEED_LOW,
    GPIO_OSPEED_MEDIUM,
    GPIO_OSPEED_HIGH,
    GPIO_OSPEED_VERY_HIGH
} gpio_ospeed_t;

typedef enum {
    GPIO_PUPD_NONE      = 0U,
    GPIO_PUPD_PULLUP    = 1U,
    GPIO_PUPD_PULLDOWN  = 2U
} gpio_pupd_t;

typedef struct {
	volatile uint32_t MODER;
	volatile uint32_t OTYPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t LCKR;
	volatile uint32_t AFR[2];
	volatile uint32_t BRR;
	volatile uint32_t ASCR;
}gpio_t;

// --- GPIO Configuration Struct ---
// A single struct to hold all configuration parameters for a pin.
typedef struct {
    gpio_t*			port;
    uint8_t      	pin;
    gpio_mode_t  	mode;
    gpio_otype_t 	otype;
    gpio_ospeed_t 	ospeed;
    gpio_pupd_t  	pupd;
    uint8_t      	alt_func; // Alternate function number (0-15)
} gpio_config_t;

/**
 * @brief Helper function to get the numeric index (0-7) for a GPIO port.
 * @param[in] port Pointer to the GPIO port struct.
 * @return The port index (0 for A, 1 for B...), or 0xFF on error.
 */
uint8_t get_port_index(gpio_t *GPIOx);

/**
 * @brief Initializes a GPIO pin with the specified configuration.
 * @param[in] config Pointer to a gpio_config_t struct with all settings.
 */
void gpio_init(const gpio_config_t *config);

/**
 * @brief Sets a GPIO pin to a HIGH state atomically.
 * @param[in] port Pointer to the GPIO port.
 * @param[in] pin The pin number (0-15).
 */
void gpio_set_pin(gpio_t *port, uint8_t pin);

/**
 * @brief Resets a GPIO pin to a LOW state atomically.
 * @param[in] port Pointer to the GPIO port.
 * @param[in] pin The pin number (0-15).
 */
void gpio_reset_pin(gpio_t *port, uint8_t pin);

/**
 * @brief Toggles the state of a GPIO output pin.
 * @param[in] port Pointer to the GPIO port.
 * @param[in] pin The pin number (0-15).
 */
void gpio_toggle_pin(gpio_t *port, uint8_t pin);

/**
 * @brief Reads the input level of a GPIO pin.
 * @param[in] port Pointer to the GPIO port.
 * @param[in] pin The pin number (0-15).
 * @return 1 if the pin is HIGH, 0 if the pin is LOW.
 */
uint8_t gpio_read_pin(gpio_t *port, uint8_t pin);

#endif
