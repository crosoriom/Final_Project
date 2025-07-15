#ifndef NVIC_H
#define NVIC_H

#include <stdint.h>

#define NVIC ((NestedVectoredInterruptController_t *)0xE000E100UL)

// --- Interrupt Number Enumeration ---
// It is crucial that these enum values match the vector table positions exactly.
typedef enum {
    /* Cortex-M4 Processor Core Exceptions */
    NonMaskableInt_IRQn     = -14,  // 2 Non Maskable Interrupt
    HardFault_IRQn          = -13,  // 3 Hard Fault Interrupt
    MemoryManagement_IRQn   = -12,  // 4 Memory Management Interrupt
    BusFault_IRQn           = -11,  // 5 Bus Fault Interrupt
    UsageFault_IRQn         = -10,  // 6 Usage Fault Interrupt
    SVCall_IRQn             = -5,   // 11 SV Call Interrupt
    DebugMonitor_IRQn       = -4,   // 12 Debug Monitor Interrupt
    PendSV_IRQn             = -2,   // 14 Pend SV Interrupt
    SysTick_IRQn            = -1,   // 15 System Tick Interrupt

    /* STM32L476RG Specific Interrupts */
    WWDG_IRQn               = 0,    // Window WatchDog
    PVD_PVM_IRQn            = 1,    // PVD/PWM through EXTI Line detection
    TAMP_STAMP_IRQn         = 2,    // Tamper and TimeStamp interrupts
    RTC_WKUP_IRQn           = 3,    // RTC Wakeup interrupt
    FLASH_IRQn              = 4,    // FLASH global Interrupt
    RCC_IRQn                = 5,    // RCC global Interrupt
    EXTI0_IRQn              = 6,    // EXTI Line0 Interrupt
    EXTI1_IRQn              = 7,    // EXTI Line1 Interrupt
    EXTI2_IRQn              = 8,    // EXTI Line2 Interrupt
    EXTI3_IRQn              = 9,    // EXTI Line3 Interrupt
    EXTI4_IRQn              = 10,   // EXTI Line4 Interrupt
    DMA1_CH1_IRQn           = 11,   // DMA1 Channel 1 global Interrupt
    DMA1_CH2_IRQn           = 12,   // DMA1 Channel 2 global Interrupt
    DMA1_CH3_IRQn           = 13,   // DMA1 Channel 3 global Interrupt
    DMA1_CH4_IRQn           = 14,   // DMA1 Channel 4 global Interrupt
    DMA1_CH5_IRQn           = 15,   // DMA1 Channel 5 global Interrupt
    DMA1_CH6_IRQn           = 16,   // DMA1 Channel 6 global Interrupt
    DMA1_CH7_IRQn           = 17,   // DMA1 Channel 7 global Interrupt
    ADC1_2_IRQn             = 18,   // ADC1 and ADC2 global Interrupt
    CAN1_TX_IRQn            = 19,   // CAN1 TX Interrupt
    CAN1_RX1_IRQn           = 20,   // CAN1 RX1 Interrupt
    CAN1_RX2_IRQn           = 21,   // CAN1 RX2 Interrupt
    CAN1_SCE_IRQn           = 22,   // CAN1 SCE Interrupt
    EXTI9_5_IRQn            = 23,   // EXTI Line[9:5] Interrupts
    TIM1_BRK_IRQn           = 24,   // TIM1 Break/TIM15 global Interrupts
    TIM1_UP_IRQn            = 25,   // TIM1 Update/TIM16 global Interrupts
    TIM1_TRG_COM_IRQn       = 26,   // TIM1 Trigger and Communication/TIM17 Interrupts
    TIM1_CC_IRQn            = 27,   // TIM1 Capture Compare Interrupt
    TIM2_IRQn               = 28,   // TIM2 global Interrupt
    TIM3_IRQn               = 29,   // TIM3 global Interrupt
    TIM4_IRQn               = 30,   // TIM4 global Interrupt
    I2C1_EV_IRQn            = 31,   // I2C1 Event Interrupt
    I2C1_ER_IRQn            = 32,   // I2C1 Error Interrupt
    I2C2_EV_IRQn            = 33,   // I2C2 Event Interrupt
    I2C2_ER_IRQn            = 34,   // I2C2 Error Interrupt
    SPI1_IRQn               = 35,   // SPI1 global Interrupt
    SPI2_IRQn               = 36,   // SPI2 global Interrupt
    USART1_IRQn             = 37,   // USART1 global Interrupt
    USART2_IRQn             = 38,   // USART2 global Interrupt
    USART3_IRQn             = 39,   // USART3 global Interrupt
    EXTI15_10_IRQn          = 40,   // EXTI Line[15:10] Interrupts
    RTC_ALARM_IRQn          = 41,   // RTC Alarm through EXTI Line 18 interrupt
    DFSDM1_FLT3_IRQn        = 42,   // DFSDM1_FLT3 global Interrupt
    TIM8_BRK_IRQn           = 43,   // TIM8 Break Interrupt
    TIM8_UP_IRQn            = 44,   // TIM8 Update Interrupt
    TIM8_TRG_COM_IRQn       = 45,   // TIM8 Trigger and Communication Interrupt
    TIM8_CC_IRQn            = 46,   // TIM8 Capture Compare Interrupt
    ADC3_IRQn               = 47,   // ADC3 global Interrupt
    FMC_IRQn                = 48,   // FMC global Interrupt
    SDMMC1_IRQn             = 49,   // SDMMC1 global Interrupt
    TIM5_IRQn               = 50,   // TIM5 global Interrupt
    SPI3_IRQn               = 51,   // SPI3 global Interrupt
    UART4_IRQn              = 52,   // UART4 global Interrupt
    UART5_IRQn              = 53,   // UART5 global Interrupt
    TIM6_DACUNDER           = 54,   // TIM6 global and DAC1 Underrun Interrupts
    TIM7_IRQn               = 55,   // TIM7 global Interrupt
    DMA2_CH1                = 56,   // DMA2 Channel 1 Interrupt
    DMA2_CH2                = 57,   // DMA2 Channel 2 Interrupt
    DMA2_CH3                = 58,   // DMA2 Channel 3 Interrupt
    DMA2_CH4                = 59,   // DMA2 Channel 4 Interrupt
    DMA2_CH5                = 60,   // DMA2 Channel 5 Interrupt
    DFSDM1_FLT0_IRQn        = 61,   // DFSDM1_FLT0 global Interrupt
    DFSDM1_FLT1_IRQn        = 62,   // DFSDM1_FLT1 global Interrupt
    DFSDM1_FLT2_IRQn        = 63,   // DFSDM1_FLT2 global Interrupt
    COMP_IRQn               = 64,   // COMP1/COMP2 through EXTI lines 21/22 interrupts
    LPTIM1_IRQn             = 65,   // LPTIM1 global Interrupt
    LPTIM2_IRQn             = 66,   // LPTIM2 global Interrupt
    OTG_FS_IRQn             = 67,   // OTG_FS global Interrupt
    DMA2_CH6                = 68,   // DMA2 Channel 6 Interrupt
    DMA2_CH7                = 69,   // DMA2 Channel 7 Interrupt
    LPUART1_IRQn            = 70,   // LPUART1 global Interrupt
    QUADSPI_IRQn            = 71,   // QUADSPI global Interrupt
    I2C3_EV_IRQn            = 72,   // I2C3 Event Interrupt
    I2C3_ER_IRQn            = 73,   // I2C3 Error Interrupt
    SAI1_IRQn               = 74,   // SAI1 global Interrupt
    SAI2_IRQn               = 75,   // SAI2 global Interrupt
    SWPMI1_IRQn             = 76,   // SWPMI1 global Interrupt
    TSC_IRQn                = 77,   // TSC global Interrupt
    LCD_IRQn                = 78,   // LCD global Interrupt
    AES_IRQn                = 79,   // AES global Interrupt
    RNG_HASH_IRQn           = 80,   // RNG and HASH Interrupts
    FPU_IRQn                = 81,   // Floating Point Interrupt
    HASH_CRS_IRQn           = 82    // HASH and CRS Interrupts
}IRQn_t;

typedef struct {
    volatile uint32_t ISER[8];
    volatile uint32_t RESERVED0[24];
    volatile uint32_t ICER[8];
    volatile uint32_t RESERVED1[24];
    volatile uint32_t ISPR[8];
    volatile uint32_t RESERVED2[24];
    volatile uint32_t ICPR[8];
    volatile uint32_t RESERVED3[24];
    volatile uint32_t IABR[8];
    volatile uint32_t RESERVED4[56];
    volatile uint32_t IPR[60];
    volatile uint32_t RESERVED5[644];
    volatile uint32_t STIR;
}NestedVectoredInterruptController_t;

/**
 * @brief Enables a device-specific interrupt in the NVIC.
 * @param[in] IRQn The interrupt number to enable (must be >= 0).
 */
void nvic_irq_enable(IRQn_t IRQn);

/**
 * @brief Disables a device-specific interrupt in the NVIC.
 * @param[in] IRQn The interrupt number to disable (must be >= 0).
 */
void nvic_irq_disable(IRQn_t IRQn);

/**
 * @brief Sets the priority of a device-specific interrupt.
 * @param[in] IRQn The interrupt number.
 * @param[in] priority The priority to set (0-15 for STM32L4).
 */
void nvic_irq_set_priority(IRQn_t IRQn, uint8_t priority);

/**
 * @brief Clears the pending status of an interrupt.
 * @param[in] IRQn The interrupt number to clear.
 */
void nvic_irq_clear_pending(IRQn_t IRQn);

#endif
