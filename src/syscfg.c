#include "syscfg.h"

void syscfg_exti_map(uint8_t GPIO_port, uint8_t pin)
{
    if(pin < 0 || pin > 15)
        return;

    if(GPIO_port == 0xFF)
        return;

    rcc_sys_power_clock_enable();
    
    // Calculate which of the 4 EXTICR registers to use (EXTICR[0]..EXTICR[3])
    uint8_t reg_index = pin / 4;
    // Calculate the bit shift amount. Each pin gets a 4-bit "nibble".
    uint8_t shift_amount = (pin % 4) * 4;

    // Set the port code into the cleared bits
    SYSCFG->EXTICR[reg_index] = (GPIO_port << shift_amount);
}
