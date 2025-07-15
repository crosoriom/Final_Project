#ifndef SYSCFG_H
#define SYSCFG_H

#include <stdint.h>
#include "rcc.h"

#define SYSCFG ((SystemConfiguration_t *)0x40010000UL)

/**
 * @brief Register map for the core SYSCFG peripheral.
 */
typedef struct {
    volatile uint32_t MEMRMP;       // Memory remap register
    volatile uint32_t CFGR1;        // Configuration register 1
    volatile uint32_t EXTICR[4];    // External interrupt configuration registers 1-4
    volatile uint32_t SCSR;         // SCSR CCMSRAM control and status register
    volatile uint32_t CFGR2;        // CFGR2 CCM SRAM control and status register
    volatile uint32_t SWPR;         // SWPR SRAM2 write protection register
    volatile uint32_t SKR;          // SKR SRAM2 key register
} SystemConfiguration_t;

/**
 * @brief Maps a GPIO pin to a specific EXTI line.
 *
 * This function configures the SYSCFG_EXTICRx registers to select which GPIO
 * port (GPIOA, GPIOB, etc.) is connected to a given EXTI line (EXTI0-EXTI15).
 * For example, to use pin PC13 as the source for EXTI13, you must map
 * port C to EXTI line 13.
 *
 * @param[in] GPIO_port Number for the GPIO
 * @param[in] pin The pin number (0-15) that will be the interrupt source.
 */
void syscfg_exti_map(uint8_t GPIO_port, uint8_t pin);

#endif
