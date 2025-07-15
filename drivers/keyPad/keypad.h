#ifndef KEYPAD_H
#define KEYPAD_H

#include "ringBuffer/ringBuffer.h"
#include "gpio.h"
#include "exti.h"

#define NUM_ROWS 4
#define NUM_COLS 4
#define KEYPAD_BUFFER_SIZE (NUM_ROWS * NUM_COLS)

static const char keypad_map[NUM_ROWS][NUM_COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

static ring_buffer_t keypad_rb;
static uint8_t keypad_buffer_data[KEYPAD_BUFFER_SIZE];

typedef struct {
    gpio_t *row_port[NUM_ROWS];
    uint8_t row_pin[NUM_ROWS];
    gpio_t *col_port[NUM_COLS];
    uint8_t col_pin[NUM_COLS];
} keypad_config_t;

// --- Debouncing State Machine ---
typedef enum {
    SCAN_STATE_IDLE,       // Waiting for an EXTI trigger
    SCAN_STATE_DEBOUNCE,   // Triggered, waiting for debounce time
    SCAN_STATE_SCAN,       // Debounce complete, performing the scan
    SCAN_STATE_WAIT_RELEASE // Key found, waiting for all keys to be released
} keypad_state_t;

static keypad_state_t keypad_state = SCAN_STATE_IDLE;
static uint8_t debounce_counter = 0;
#define DEBOUNCE_TIME 20 // Debounce for 20 scan calls (e.g., 20ms if scanned every 1ms)

/*
 * @brief Initializes the keypad GPIOs and interrupt triggers.
 *
 * Configures row pins as outputs and column pins as inputs with EXTI.
 *
 * @param[in] config Pointer to the keypad_config_t struct.
 */
void keypad_init(const keypad_config_t *config);

/**
 * @brief This is the main processing function for the keypad.
 *
 * It should be called periodically from a timer interrupt (e.g., SysTick_Handler
 * every 1ms). It contains the state machine for scanning and debouncing.
 */
void keypad_process(void);

/**
 * @brief Notifies the driver that a potential key press has occurred.
 *
 * This function should be called from your EXTI interrupt handlers for the column pins.
 * It's a simple, non-blocking trigger for the state machine.
 */
void keypad_notify_on_press(void);

/**
 * @brief Reads a single character from the keypad buffer.
 *
 * @param[out] key Pointer to a variable to store the read key.
 * @return true if a key was read, false if the buffer is empty.
 */
bool keypad_read_key(char *key);

#endif
