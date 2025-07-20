#include "main.h"

// --- Global variables ---
volatile bool g_button_pressed_flag = false;

// --- Configurations ---
const keypad_config_t keypad_conf = {
    .row_port = {GPIOA, GPIOB, GPIOB, GPIOB},
    .row_pin  = {10, 3, 5, 4},
    .col_port = {GPIOB, GPIOA, GPIOA, GPIOC},
    .col_pin  = {10, 8, 9, 7}
};

const usart_config_t usart2_config = {
    .usart_port = USART2,
    .baudrate   = 115200,
    .word_lengt = NINE_BITS_LENGHT,
    .stop_bits  = ONE_STOP_BIT,
    .parity     = ODD_PARITY
};

const gpio_config_t heartbeat_config = {
    .port   = GPIOA,
    .pin    = 5,
    .mode   = GPIO_MODE_OUTPUT
};

int main(void) {
    // 1. Initialize system clock to 80MHz using PLL
    rcc_set_system_clock(SYSCLK_SRC_HSI);
    
    // 2. Initialize SysTick for a 1ms tick at 80MHz
    systick_init(16000);

    // 3. Initialize peripherals
    gpio_init(&heartbeat_config);
    keypad_init(&keypad_conf);
    usart_init(&usart2_config, 16000000); // Use 80MHz clock
    
    // 4. Initialize the user button interrupt on PC13
    exti_gpio_init(GPIOC, 13, GPIO_PUPD_PULLUP, FALLING_EDGE);

    uint32_t heartbeat_last_tick = 0;
    
    usart_send_string(USART2, "System Initialized. Ready.\r\n");
    
    while(1) {
        char pressed_key;

        if(gpio_read_pin(GPIOC, 13) == 0)
            gpio_toggle_pin(GPIOA, 5);

        // Task 1: Non-blocking Heartbeat LED
        if(systick_getTick() - heartbeat_last_tick >= 500) {
            heartbeat_last_tick = systick_getTick();
            gpio_toggle_pin(GPIOA, 5);
        }

        // Task 2: Process button press flag from ISR
        if(g_button_pressed_flag) {
            g_button_pressed_flag = false; // Clear flag
            usart_send_string(USART2, "Button Pressed\r\n");
        }
        
        /*
        // Task 3: Check Keypad Buffer
        if(keypad_read_key(&pressed_key)) {
            usart_send_string(USART2, "Key pressed: ");
            usart_send_char(USART2, pressed_key);
            usart_send_string(USART2, "\r\n");
        }
        */
    }
    return 0;
}

// NONeSAFE Interrupt Handler
void EXTI15_10_IRQHandler(void)
{
    if(EXTI->PR1 & (1U << 13)) {
        EXTI->PR1 = (1U << 13); // Clear pending flag
        g_button_pressed_flag = true;
    }
    if(EXTI->PR1 & (1U << 10))
        keypad_irq_handler();
}

void EXTI9_5_IRQHandler(void)
{
    if(EXTI->PR1 & (1U << 9))
        keypad_irq_handler();
    if(EXTI->PR1 & (1U << 8))
        keypad_irq_handler();
    if(EXTI->PR1 & (1U << 7))
        keypad_irq_handler();
}
