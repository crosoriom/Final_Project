#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

typedef struct {
    volatile uint32_t CTRL;     // SysTick Control and Status Register
    volatile uint32_t LOAD;     // SysTick Reload Value Register
    volatile uint32_t VAL;      // SysTick Current Value Register
    volatile uint32_t CALIB;    // SysTick Calibration Value Register
}SysTick_t;

#define SYSTICK ((SysTick_t *)0xE000E010UL)	//Base address of System Timer.

// --- SysTick Control Register Bits ---
#define SYSTICK_CTRL_ENABLE_Pos     (0U)
#define SYSTICK_CTRL_ENABLE         (1U << SYSTICK_CTRL_ENABLE_Pos)
#define SYSTICK_CTRL_TICKINT_Pos    (1U)
#define SYSTICK_CTRL_TICKINT        (1U << SYSTICK_CTRL_TICKINT_Pos)
#define SYSTICK_CTRL_CLKSOURCE_Pos  (2U)
#define SYSTICK_CTRL_CLKSOURCE      (1U << SYSTICK_CTRL_CLKSOURCE_Pos)

/**
 * @brief Initializes and starts the SysTick timer.
 *
 * Configures the SysTick timer to generate an interrupt at a specified frequency.
 * This function sets up the SysTick to use the main processor clock (AHB).
 *
 * @param[in] ticks_per_interrupt The number of clock cycles between each SysTick interrupt.
 *                                For an 80MHz clock, a value of 80000 gives a 1ms interrupt.
 *
 * @note This function also enables the SysTick interrupt (SysTick_Handler).
 */
void systick_init(uint32_t ticks_per_interrupt);

/**
 * @brief Provides a non-blocking delay for a specified number of milliseconds.
 *
 * This function waits for a duration without halting the entire system. Other tasks
 * can run if using an RTOS, but in a bare-metal loop, this will still be a busy-wait.
 *
 * @param[in] ms The delay duration in milliseconds.
 */
void systick_delay_ms(uint32_t ms);
/**
 * @brief Gets the current system tick count.
 *
 * Returns the number of milliseconds that have elapsed since the SysTick timer started.
 * This value is incremented by the SysTick_Handler ISR.
 *
 * @return The current system tick count.
 */
uint32_t systick_getTick(void);

/**
 * @brief Pone en cero el contador de interrupciones.
 *
 * Esta función restablece el contador de interrupciones
 * (contador de tiempo) del sistema.
 */
void systick_reset(void);

#endif
