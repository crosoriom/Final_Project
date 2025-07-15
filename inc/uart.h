#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "nvic.h"
#include "gpio.h"
#include "rcc.h"

#define USART1 ((usart_t *)0x40013800UL)
#define USART2 ((usart_t *)0x40004400UL)
#define USART3 ((usart_t *)0x40004800UL)
#define UART_4 ((usart_t *)0x40004C00UL)
#define UART_5 ((usart_t *)0x40005000UL)

// --- USART Control Register Bits ---
#define USART_CR1_UE_Pos     (0U)
#define USART_CR1_UE         (1U << USART_CR1_UE_Pos) // USART Enable
#define USART_CR1_RE_Pos     (2U)
#define USART_CR1_RE         (1U << USART_CR1_RE_Pos) // Receiver Enable
#define USART_CR1_TE_Pos     (3U)
#define USART_CR1_TE         (1U << USART_CR1_TE_Pos) // Transmitter Enable
#define USART_CR1_RXNEIE_Pos (5U)
#define USART_CR1_RXNEIE     (1U << USART_CR1_RXNEIE_Pos) // RXNE Interrupt Enable
#define USART_CR1_PCE_Pos    (10U)
#define USART_CR1_PCE        (1U << USART_CR1_PCE_Pos)  // Parity Control Enable
#define USART_CR1_PS_Pos     (9U)
#define USART_CR1_PS         (1U << USART_CR1_PS_Pos)   // Parity Selection
#define USART_CR1_M0_Pos     (12U)
#define USART_CR1_M0         (1U << USART_CR1_M0_Pos)   // Word Length Bit 0
#define USART_CR1_M1_Pos     (28U)
#define USART_CR1_M1         (1U << USART_CR1_M1_Pos)   // Word Length Bit 1

// --- USART Status Register Bits ---
#define USART_ISR_TXE_Pos    (7U)
#define USART_ISR_TXE        (1U << USART_ISR_TXE_Pos) // Transmit Data Register Empty
#define USART_ISR_RXNE_Pos   (5U)
#define USART_ISR_RXNE       (1U << USART_ISR_RXNE_Pos) // Read Data Register Not Empty

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t BRR;
    volatile uint32_t GTPR;
    volatile uint32_t RTOR;
    volatile uint32_t RQR;
    volatile uint32_t ISR;
    volatile uint32_t ICR;
    volatile uint32_t RDR;
    volatile uint32_t TDR;
}usart_t;

typedef enum {
    HALF_STOP_BIT,
    ONE_STOP_BIT,
    ONE_HALF_STOP_BIT,
    TWO_STOP_BITS
}stopBit_t;

typedef enum {
    SEVEN_BITS_LENGHT,
    EIGHT_BITS_LENGHT,
    NINE_BITS_LENGHT
}lenghtBit_t;

typedef enum {
    EVEN_PARITY,
    ODD_PARITY,
    NO_PARITY
}parity_t;

typedef struct {
    usart_t *usart_port;
    uint32_t baudrate;
    stopBit_t stop_bits;
    lenghtBit_t word_lengt;
    parity_t parity;
}usart_config_t;

/**
 * @brief Initializes a USART peripheral for communication.
 * @param[in] config Pointer to the usart_config_t struct.
 * @param[in] pclk_freq The frequency of the peripheral clock (PCLK) feeding the USART.
 */
void usart_init(const usart_config_t *config, uint32_t pclk_freq);

/**
 * @brief Enables the USART receive interrupt (RXNE).
 * @param[in] usart_port Pointer to the USART peripheral.
 */
void usart_rx_interrupt_enable(usart_t *usart_port);

/**
 * @brief Sends a single character over USART.
 * @param[in] usart_port Pointer to the USART peripheral.
 * @param[in] c The character to send.
 */
void usart_send_char(usart_t *usart_port, char c);

/**
 * @brief Sends a null-terminated string over USART.
 * @param[in] usart_port Pointer to the USART peripheral.
 * @param[in] str The string to send.
 */
void usart_send_string(usart_t *usart_port, const char *str);

/**
 * @brief Receives a single character from USART.
 * @note This is a blocking function. It will wait until a character is received.
 * @param[in] usart_port Pointer to the USART peripheral.
 * @return The character received.
 */
char usart_receive_char(usart_t *usart_port);

#endif
