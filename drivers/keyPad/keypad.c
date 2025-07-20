#include "keyPad/keypad.h"

void keypad_init(const keypad_config_t *config)
{
    if (config == NULL) return;

    keypad_config = *config;

    // Configure Row pins as Push-Pull Outputs, initially HIGH
    for (int i = 0; i < NUM_ROWS; i++) {
        gpio_config_t row_config = {
            .port = keypad_config.row_port[i], .pin = keypad_config.row_pin[i],
            .mode = GPIO_MODE_OUTPUT, .otype = GPIO_OTYPE_PUSHPULL,
        };
        gpio_init(&row_config);
        gpio_set_pin(keypad_config.row_port[i], keypad_config.row_pin[i]);
    }

    // Configure Column pins as Inputs with Pull-up and Falling Edge EXTI
    for (int i = 0; i < NUM_COLS; i++) {
        exti_gpio_init(
            keypad_config.col_port[i],
            keypad_config.col_pin[i],
            GPIO_PUPD_PULLUP,
            FALLING_EDGE
        );
    }

    ring_buffer_init(&keypad_rb, keypad_buffer_data, KEYPAD_BUFFER_SIZE);
    keypad_initialized = true;
}

void keypad_irq_handler(void)
{
    if(!keypad_initialized) return;

    // 1. Disable all column interrupts to prevent bouncing and re-entry.
    // We only need to disable the shared IRQ lines, not individual pins.
    for(int c = 0; c < NUM_COLS; c++) {
        IRQn_t irq = get_irqn_for_exti_line(keypad_config.col_pin[c]);
        nvic_irq_disable(irq);
    }

    // 2. Short blocking delay for initial debounce.
    // This is generally acceptable in an ISR if it's very short. 5-10ms is typical.
    systick_delay_ms(5);

    char pressed_key = '\0';

    // 3. Perform the scan
    for(int r = 0; r < NUM_ROWS; r++) {
        gpio_reset_pin(keypad_config.row_port[r], keypad_config.row_pin[r]);
        for(int c = 0; c < NUM_COLS; c++) {
            if(gpio_read_pin(keypad_config.col_port[c], keypad_config.col_pin[c]) == 0) {
                pressed_key = keypad_map[r][c];
                break;
            }
        }

        // Set the current row back to HIGH before checking the next row
        gpio_set_pin(keypad_config.row_port[r], keypad_config.row_pin[r]);

        // If we found a key in this row, stop scanning other rows
        if(pressed_key != '\0') {
            break;
        }
    }

    // 4. If a valid key was found, write it to the buffer.
    if(pressed_key != '\0') {
        ring_buffer_write(&keypad_rb, (uint8_t)pressed_key);
    }
    
    // 5. CRITICAL: Clear all potential pending EXTI flags for the columns.
    // This prevents the ISR from re-firing immediately after we re-enable it.
    for(int c = 0; c < NUM_COLS; c++) {
        exti_clear_interrupt(keypad_config.col_pin[c]);
        // 6. CRITICAL: Re-enable all column interrupts before exiting.
        IRQn_t irq = get_irqn_for_exti_line(keypad_config.col_pin[c]);
        nvic_irq_enable(irq);
    }
}

bool keypad_read_key(char *key)
{
    if(key == NULL) return false;
    return ring_buffer_read(&keypad_rb, (uint8_t *)key);
}
