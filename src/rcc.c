#include "rcc.h"

void rcc_sys_power_clock_enable(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;	// Enable SYSCFG clock
    RCC->APB1ENR1 |= (1U << 28);			// Enable PWREN clock. This bit is needed for CPU speed above 26 MHz
}

/**
 * @brief Configures the system to run from the 80MHz PLL, sourced by HSI.
 */
static void rcc_configure_pll_80mhz_from_hsi(void)
{
	// 1. Enable HSI and wait for it to be ready
	RCC->CR |= RCC_CR_HSION;
	while(!(RCC->CR & RCC_CR_HSIRDY));

	// 2. Configure Flash latency and power settings
	rcc_sys_power_clock_enable();
	// Set Voltage Scaling to Range 1 (high-performance) for clocks > 26MHz
    // PWR->CR1 |= PWR_CR1_VOS_0; // You'd need a PWR driver for this.
	
	// Call the Flash driver function to set 4 wait states for 80MHz.
    flash_configure_for_high_speed();

	// 3. Configure the PLL
	RCC->CR &= ~RCC_CR_PLLON;
	while(RCC->CR & RCC_CR_PLLRDY);

	// Configure PLL: HSI (16MHz) -> /M=2 -> *N=20 -> /R=2 = 80MHz SYSCLK
    // PLLSRC = HSI (0b10)
    // PLLM   = /2   (0b001)
    // PLLN   = *20  (0b0010100)
    // PLLR   = /2   (0b00, this is the default and PLLREN enables it)
    RCC->PLLCFGR = (2U << 0) | (1U << 4) | (20U << 8) | (1U << 24);

	// Enable the PLL and wait for it to lock
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));

	// 4. Switch System Clock to PLL
    // First, clear the SW bits, then set them to select PLL (0b11)
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW_Msk) | (3U << RCC_CFGR_SW_Pos);

    // Wait until the SWS bits confirm the PLL is the source (0b11)
    while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != (3U << RCC_CFGR_SWS_Pos));
}

void rcc_set_system_clock(rcc_clksrc_t clock_source)
{
	switch (clock_source) {
	case SYSCLK_SRC_MSI:
		// MSI is default, but here's how to switch back to it.
        RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW_Msk) | (0U << RCC_CFGR_SW_Pos);
        while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != (0U << RCC_CFGR_SWS_Pos));
        break;

	case SYSCLK_SRC_HSI:
		RCC->CR |= RCC_CR_HSION;
		while(!(RCC->CR & RCC_CR_HSIRDY));

		RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW_Msk) | (1U << RCC_CFGR_SW_Pos);
		while((RCC->CFGR & RCC_CFGR_SW_Msk) != (1U << RCC_CFGR_SW_Pos));
		break;

	case SYSCLK_SRC_HSE:
		break;

	case SYSCLK_SRC_PLL:
		rcc_configure_pll_80mhz_from_hsi();
		break;
	}
}

void rcc_gpio_clock_enable(uint8_t port_index)
{
	if(port_index <= 7)
		RCC->AHB2ENR |= (1U << port_index);
}

void rcc_usart_clock_enable(uint8_t usart_number)
{
	switch (usart_number) {
		// USARTs in APB1 Bus.
		case 2: RCC->APB1ENR1 |= (1U << 17); break;
		case 3: RCC->APB1ENR1 |= (1U << 18); break;
		case 4: RCC->APB1ENR1 |= (1U << 19); break;
		case 5: RCC->APB1ENR1 |= (1U << 20); break;

		// USARTs in APB2 Bus.
		case 1: RCC->APB2ENR |= (1U << 14); break;
	}
}

void rcc_i2c_clock_enable(uint8_t i2c_number)
{
	switch (i2c_number) {
		case 1: RCC->APB1ENR1 |= (1U << 21); break;
		case 2: RCC->APB1ENR1 |= (1U << 22); break;
		case 3: RCC->APB1ENR1 |= (1U << 23); break;
	}
}

void rcc_tim_clock_enable(uint8_t timer_number)
{
	switch (timer_number) {
		// Timers on APB1 Bus.
		case 2: RCC->APB1ENR1 |= (1U << 0); break;
		case 3: RCC->APB1ENR1 |= (1U << 1); break;
		case 4: RCC->APB1ENR1 |= (1U << 2); break;
		case 5: RCC->APB1ENR1 |= (1U << 3); break;
		case 6: RCC->APB1ENR1 |= (1U << 4); break;
		case 7: RCC->APB1ENR1 |= (1U << 5); break;

		// Timers on APB2 Bus.
		case 1: RCC->APB2ENR |= (1U << 11); break;
		case 8: RCC->APB2ENR |= (1U << 13); break;
		case 15: RCC->APB2ENR |= (1U << 16); break;
		case 16: RCC->APB2ENR |= (1U << 17); break;
		case 17: RCC->APB2ENR |= (1U << 18); break;
	}
}
