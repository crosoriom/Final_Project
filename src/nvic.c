#include "nvic.h"

void nvic_irq_enable(IRQn_t IRQn)
{
    if ((int)IRQn < 0 || (int)IRQn > 82) {
        return;
    }

    // ISER[n] is the Interrupt Set-Enable Register for IRQs [32*n] to [32*n + 31].
    // ISER[0] handles IRQs 0-31
    // ISER[1] handles IRQs 32-63
    // ...
    NVIC->ISER[IRQn / 32] |= (1U << (IRQn % 32));
}

void nvic_irq_disable(IRQn_t IRQn)
{
    if ((int)IRQn < 0 || (int)IRQn > 82) {
        return;
    }

    // The logic is identical to enabling, but uses the Interrupt Clear-Enable Register (ICER).
    // Writing a '1' to a bit in ICER disables the corresponding interrupt.
    NVIC->ICER[IRQn / 32] |= (1U << (IRQn % 32));
}

void nvic_irq_set_priority(IRQn_t IRQn, uint8_t priority)
{
    if ((int)IRQn < 0 || (int)IRQn > 82) {
        return;
    }

    // The IPR registers are byte-addressable. Each IRQ has its own 8-bit field.
    // The Cortex-M4 architecture supports 256 priority levels (0-255), but
    // STM32 MCUs only implement a subset. The L476RG implements 16 levels (0-15).
    // These levels are stored in the most significant bits of the byte.
    // So we must shift our priority value to the left.
    NVIC->IPR[IRQn] = ((priority << 4) & 0xFF);
}

void nvic_irq_clear_pending(IRQn_t IRQn)
{
    if ((int)IRQn < 0 || (int)IRQn > 82) {
        return;
    }

    // The Interrupt Clear-Pending Register (ICPR) works just like ISER/ICER.
    NVIC->ICPR[IRQn / 32] = (1U << (IRQn % 32));
}
