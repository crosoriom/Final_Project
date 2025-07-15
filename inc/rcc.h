#ifndef RCC_H
#define RCC_H

#include <stdint.h>
#include "flash.h"

// --- Peripheral base Address ---
#define RCC ((ResetClockControl_t *)0x40021000UL)

// --- RCC_CR Register Bits ---
#define RCC_CR_MSION_Pos      (0U)
#define RCC_CR_MSION          (1U << RCC_CR_MSION_Pos)
#define RCC_CR_MSIRDY_Pos     (1U)
#define RCC_CR_MSIRDY         (1U << RCC_CR_MSIRDY_Pos)
#define RCC_CR_HSION_Pos      (8U)
#define RCC_CR_HSION          (1U << RCC_CR_HSION_Pos)
#define RCC_CR_HSIRDY_Pos     (10U)
#define RCC_CR_HSIRDY         (1U << RCC_CR_HSIRDY_Pos)
#define RCC_CR_HSEON_Pos      (16U)
#define RCC_CR_HSEON          (1U << RCC_CR_HSEON_Pos)
#define RCC_CR_HSERDY_Pos     (17U)
#define RCC_CR_HSERDY         (1U << RCC_CR_HSERDY_Pos)
#define RCC_CR_PLLON_Pos      (24U)
#define RCC_CR_PLLON          (1U << RCC_CR_PLLON_Pos)
#define RCC_CR_PLLRDY_Pos     (25U)
#define RCC_CR_PLLRDY         (1U << RCC_CR_PLLRDY_Pos)

// --- RCC_CFGR Register Bits ---
#define RCC_CFGR_SW_Pos       (0U)
#define RCC_CFGR_SW_Msk       (0x3U << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SWS_Pos      (2U)
#define RCC_CFGR_SWS_Msk      (0x3U << RCC_CFGR_SWS_Pos)

// --- RCC_APB2ENR Register Bits ---
#define RCC_APB2ENR_SYSCFGEN_Pos (0U)
#define RCC_APB2ENR_SYSCFGEN     (1U << RCC_APB2ENR_SYSCFGEN_Pos)
#define RCC_APB2ENR_TIM1EN_Pos   (11U)
#define RCC_APB2ENR_TIM1EN       (1U << RCC_APB2ENR_TIM1EN_Pos)
#define RCC_APB2ENR_USART1EN_Pos (14U)
#define RCC_APB2ENR_USART1EN     (1U << RCC_APB2ENR_USART1EN_Pos)

typedef struct {
	volatile uint32_t CR;
	volatile uint32_t ICSCR;
	volatile uint32_t CFGR;
	volatile uint32_t PLLCFGR;
	volatile uint32_t PLLSAI1CFGR;
	volatile uint32_t PLLSAI2CFGR;
	volatile uint32_t CIER;
	volatile uint32_t CIFR;
	volatile uint32_t CICR;
	volatile uint32_t RESERVED0;
	volatile uint32_t AHB1RSTR;
	volatile uint32_t AHB2RSTR;
	volatile uint32_t AHB3RSTR;
	volatile uint32_t RESERVED1;
	volatile uint32_t APB1RSTR1;
	volatile uint32_t APB1RSTR2;
	volatile uint32_t APB2RSTR;
	volatile uint32_t RESERVED2;
	volatile uint32_t AHB1ENR;
	volatile uint32_t AHB2ENR;
	volatile uint32_t AHB3ENR;
	volatile uint32_t RESERVED3;
	volatile uint32_t APB1ENR1;
	volatile uint32_t APB1ENR2;
	volatile uint32_t APB2ENR;
	volatile uint32_t RESERVED4;
	volatile uint32_t AHB1SMENR;
	volatile uint32_t AHB2SMENR;
	volatile uint32_t AHB3SMENR;
	volatile uint32_t RESERVED5;
	volatile uint32_t APB1SMENR1;
	volatile uint32_t APB1SMENR2;
	volatile uint32_t APB2SMENR;
	volatile uint32_t RESERVED6;
	volatile uint32_t CCIPR;
	volatile uint32_t BDCR;
	volatile uint32_t CSR;
}ResetClockControl_t;

/**
 * @brief Enumeration for the main system clock sources.
 */
typedef enum {
    SYSCLK_SRC_MSI, // Multi-Speed Internal oscillator
    SYSCLK_SRC_HSI, // High-Speed Internal oscillator
    SYSCLK_SRC_HSE, // High-Speed External oscillator (if available)
    SYSCLK_SRC_PLL  // Phase-Locked Loop
} rcc_clksrc_t;

/**
 * @brief Enables the clock for SYSCFG and the Power Interface.
 * @note Power interface clock is required to change voltage scaling.
 */
void rcc_sys_power_clock_enable(void);

/**
 * @brief Configures and selects the main system clock (SYSCLK).
 * @param[in] clock_source The desired clock source from rcc_clksrc_t.
 */
void rcc_set_system_clock(rcc_clksrc_t clock_source);

/**
 * @brief Enables the clock for a specific GPIO port.
 * @param[in] port_index Integer for the port (0 for GPIOA, 1 for GPIOB, etc.).
 */
void rcc_gpio_clock_enable(uint8_t port_index);

/**
 * @brief Enables the clock for a specific USART peripheral.
 * @param[in] usart_number The number for the USART (1, 2, 3, etc.).
 * @note usart_number 4 and 5 activates the clock for UART com.
 */
void rcc_usart_clock_enable(uint8_t usart_number);

/**
 * @brief Enables the clock for a specific I2C peripheral.
 * @param[in] i2c_number The number for the I2C (1, 2, 3).
 */
void rcc_i2c_clock_enable(uint8_t i2c_number);

/**
 * @brief Enables the clock for a specific Timer peripheral.
 * @param[in] timer_number The number of the timer (1, 2, 3, etc.).
 */
void rcc_tim_clock_enable(uint8_t timer_number);

#endif
