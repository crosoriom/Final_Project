#ifndef EXTI_H
#define EXTI_H

#include <stdint.h>
#include "syscfg.h"
#include "gpio.h"
#include "nvic.h"

#define EXTI ((ExtendedInterrupts_t *)0x40010400UL)

typedef struct {
    volatile uint32_t IMR1;
    volatile uint32_t EMR1;
    volatile uint32_t RTSR1;
    volatile uint32_t FTSR1;
    volatile uint32_t SWIER1;
    volatile uint32_t PR1;
    volatile uint32_t RESERVED[3];
    volatile uint32_t IMR2;
    volatile uint32_t EMR2;
    volatile uint32_t RTSR2;
    volatile uint32_t FTSR2;
    volatile uint32_t SWIER2;
    volatile uint32_t PR2;
}ExtendedInterrupts_t;

/**
 * @brief Enumeration for the interrupt trigger type.
 */
typedef enum {
    RISING_EDGE,
    FALLING_EDGE,
    BOTH_EDGES
}exti_trigger_t;

/**
 * @brief Configures a GPIO pin to trigger an external interrupt.
 * @param[in] port Pointer to the GPIO port peripheral (e.g., GPIOA).
 * @param[in] pin The pin number (0-15) to be used as the interrupt source.
 * @param[in] pupd The desired pull-up/pull-down configuration for the input pin.
 * @param[in] trigger The edge detection (rising, falling, or both) for the interrupt.
 */
void exti_gpio_init(gpio_t *GPIOx, uint8_t pin, gpio_pupd_t pupd, exti_trigger_t trigger);

/**
 * @brief Helper function to get the correct NVIC IRQn for a given EXTI line.
 * @param[in] pin The pin number (0-15).
 * @return IRQn_Type The corresponding IRQ number for the pin.
 */
IRQn_t get_irqn_for_exti_line(uint8_t pin);

void exti_clear_interrupt(uint8_t exti_line);

/**
 * @brief Activa las interrupciones de UART.
 *
 * La función configura los registros para activar
 * la interrupción del NVIC (vector global de
 * interrupciones) correspondiente al puerto de
 * comunicación serial UART o USART que se esté
 * utilizando.
 *
 * @param *USARTx Dirección del dispositivo de
 * comunicación a configurar.
 */
void usart_interrupt_enable(uint8_t USARTx);

#endif
