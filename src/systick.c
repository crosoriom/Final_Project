#include "systick.h"

// This global variable holds the system tick count.
// It is declared as 'volatile' because it is modified in an ISR and read
// in the main application code. This prevents the compiler from making
// unsafe optimizations.
static volatile uint32_t tick_counter = 0;

void systick_init(uint32_t ticks)
{
	if(ticks > 0x00FFFFFF)
		ticks = 0x00FFFFFF;
	
	// 1. Dissable the peripheral for configuration
	SYSTICK->CTRL = ~SYSTICK_CTRL_ENABLE;
	
	// 2. Set the reload value
	SYSTICK->LOAD = ticks - 1;

	// 3. Clear the current value register
	SYSTICK->VAL = 0;
	
	// 4. Configure and enable the SysTick timer.
    //    - SYSTICK_CTRL_ENABLE:    Enable the counter.
    //    - SYSTICK_CTRL_TICKINT:   Enable the SysTick interrupt.
    //    - SYSTICK_CTRL_CLKSOURCE: Select the processor clock (AHB).
    SYSTICK->CTRL = SYSTICK_CTRL_ENABLE | SYSTICK_CTRL_TICKINT | SYSTICK_CTRL_CLKSOURCE;
}

uint32_t systick_getTick(void)
{
	return tick_counter;
}

void systick_delay_ms(uint32_t time)
{
	// Record the start time of the delay
    uint32_t start_tick = systick_getTick();
    while(systick_getTick() - start_tick < time)
        ;
    return ;
}

void systick_reset(void)
{
	tick_counter = 0;
}

void SysTick_Handler(void)
{
	tick_counter++;
}
