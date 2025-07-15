#include "uart.h"

int usart_number(usart_t *USARTx)
{
    if(USARTx == USART1) return 1;
    else if(USARTx == USART2) return 2;
    else if(USARTx == USART3) return 3;
    else if(USARTx == UART_4) return 4;
    else if(USARTx == UART_5) return 5;
    else return 0xF;
}

/**
 * @brief Helper to find the GPIO pins and AF for a given USART.
 * @param[in] usart_port The USART peripheral.
 * @param[out] tx_conf Pointer to store the TX pin GPIO config.
 * @param[out] rx_conf Pointer to store the RX pin GPIO config.
 * @return 1 on success, 0 on failure (no mapping found).
 */
static int get_usart_pin_configs(usart_t *usart_port, gpio_config_t *tx_conf, gpio_config_t *rx_conf)
{
    // Common settings for all USART pins
    tx_conf->mode = GPIO_MODE_ALTERNATE;
    tx_conf->otype = GPIO_OTYPE_PUSHPULL;
    tx_conf->ospeed = GPIO_OSPEED_VERY_HIGH;
    tx_conf->pupd = GPIO_PUPD_NONE;
    *rx_conf = *tx_conf; // Copy common settings

    if(usart_port == USART1) {          // Options: TX = PA9, PB6, PG9; RX = PA10, PB7, PG10
        tx_conf->port = GPIOA; tx_conf->pin = 9;
        rx_conf->port = GPIOA; rx_conf->pin = 10;

        tx_conf->alt_func = rx_conf->alt_func = 7;
        return 1;
    }
    if (usart_port == USART2) {         // Options: TX = PA2, PD5; RX = PA3, PD6
        tx_conf->port = GPIOA; tx_conf->pin = 2;
        rx_conf->port = GPIOA; rx_conf->pin = 3;

        tx_conf->alt_func = rx_conf->alt_func = 7;
        return 1;
    }
    if (usart_port == USART3) {         // Options: TX = PB10, PC4, PC10 PD8; RX = PB11, PC5, PC11, PD9
        tx_conf->port = GPIOB; tx_conf->pin = 10;
        rx_conf->port = GPIOB; rx_conf->pin = 11;

        tx_conf->alt_func = rx_conf->alt_func = 7;
        return 1;
    }
    if (usart_port == UART_4) {         // Options: TX = PA0, PC10; RX = PA1, PC11
        tx_conf->port = GPIOA; tx_conf->pin = 0;
        rx_conf->port = GPIOA; rx_conf->pin = 1;

        tx_conf->alt_func = rx_conf->alt_func = 8;
        return 1;
    }
    if (usart_port == UART_5) {         // Options: TX = PC12; RX = PD2
        tx_conf->port = GPIOC; tx_conf->pin = 12;
        rx_conf->port = GPIOD; rx_conf->pin = 2;

        tx_conf->alt_func = rx_conf->alt_func = 8;
        return 1;
    }
    
    // If no mapping is found
    return 0;
}

/**
 * @brief Helper function to set the USART Word Lenght parameter.
 * @param[in] USARTx The USART peripheral.
 * @param[in] lenght Enum for Word Lenght.
 */
void usart_set_word_lenght(usart_t *USARTx, lenghtBit_t lenght)
{
    USARTx->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0);
    switch(lenght) {
        case EIGHT_BITS_LENGHT:
            break;
        case NINE_BITS_LENGHT:
            USARTx->CR1 |= USART_CR1_M0;
            break;
        case SEVEN_BITS_LENGHT:
            USARTx->CR1 |= USART_CR1_M1;
            break;
    }
}

/**
 * @brief Helper function to set the USART Stop Bits parameter.
 * @param[in] USARTx The USART peripheral.
 * @param[in] stop Enum for Stop Bits.
 */
void usart_set_stop_bits(usart_t *USARTx, stopBit_t stop)
{
    USARTx->CR2 &= ~(3U << 12);
    switch(stop) {
        case HALF_STOP_BIT:
            USARTx->CR2 |= (1U << 12);
            break;
        case ONE_STOP_BIT:
            break;
        case ONE_HALF_STOP_BIT:
            USARTx->CR2 |= (3U << 12);
            break;
        case TWO_STOP_BITS:
            USARTx->CR2 |= (2U << 12);
            break;
    }
}

/**
 * @brief Helper function to set the USART Parity parameter.
 * @param[in] USARTx The USART peripheral.
 * @param[in] stop Enum for Parity.
 */
void usart_set_parity(usart_t *USARTx, parity_t parity)
{
    USARTx->CR1 &= ~(USART_CR1_PCE | USART_CR1_PS);
    switch(parity) {
        case NO_PARITY:
            // PCE is already disabled from the clear operation above. Do nothing.
            break;
        case EVEN_PARITY:
            // Enable Parity Control, but leave Parity Selection as 0 (for Even)
            USARTx->CR1 |= USART_CR1_PCE;
            break;
        case ODD_PARITY:
            USARTx->CR1 |= USART_CR1_PCE | USART_CR1_PS;
            break;
    }
}

void usart_init(const usart_config_t *config, uint32_t pclk_freq)
{
    usart_t *USARTx = config->usart_port;

    // 1. Enable peripheral clock for the USART
    rcc_usart_clock_enable(usart_number(USARTx));

    // 2. Configure GPIO pins for TX and RX
    gpio_config_t tx_pin_config, rx_pin_config;
    if(!get_usart_pin_configs(USARTx, &tx_pin_config, &rx_pin_config))
        return;
    gpio_init(&tx_pin_config);
    gpio_init(&rx_pin_config);

    // 3. Configure the USART peripheral
    USARTx->CR1 &= ~USART_CR1_UE;                        // Disable USART first to allow configuration

    // Ensure oversampling is by 16 for our baud rate calculation
    //USARTx->CR1 &= ~(1U << 15); // Clear OVER8 bit

    // --- Disable hardware flow control ---
    //USARTx->CR3 &= ~((1U << 9) | (1U << 8)); // Clear CTSE and RTSE

    // Set baud rate. BRR = PCLK / Baudrate.
    // For an 80MHz clock with no APB prescaler, PCLK is 80MHz.
    USARTx->BRR = pclk_freq / config->baudrate;

    usart_set_word_lenght(USARTx, config->word_lengt);
    usart_set_stop_bits(USARTx, config->stop_bits);
    usart_set_parity(USARTx, config->parity);

    // 4. Enable Transmitter, Receiver, and then the USART peripheral
    USARTx->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USARTx->CR1 |= USART_CR1_UE;
}

void usart_rx_interrupt_enable(usart_t *USARTx)
{
    // 1. Enable the RXNE interrupt within the USART peripheral
    USARTx->CR1 |= USART_CR1_RXNEIE;

    // 2. Enable the corresponding global interrupt in the NVIC
    switch(usart_number(USARTx)) {
        case 1: nvic_irq_enable(USART1_IRQn); break;
        case 2: nvic_irq_enable(USART2_IRQn); break;
        case 3: nvic_irq_enable(USART3_IRQn); break;
        case 4: nvic_irq_enable(UART4_IRQn); break;
        case 5: nvic_irq_enable(UART5_IRQn); break;
    }
}

void usart_send_char(usart_t *USARTx, char c)
{
    // Wait until the Transmit Data Register (TDR) is empty.
    while (!(USARTx->ISR & USART_ISR_TXE));

    // Write the character to the TDR.
    USARTx->TDR = (uint8_t)c;
}

void usart_send_string(usart_t *USARTx, const char *str)
{
    while(*str)
        usart_send_char(USARTx, *str++);
}

char usart_receive_char(usart_t *usart_port)
{
    // Wait until the Read Data Register (RDR) is not empty.
    while (!(usart_port->ISR & USART_ISR_RXNE));
    
    // Read the character from the RDR. This also clears the RXNE flag.
    return (char)usart_port->RDR;
}
