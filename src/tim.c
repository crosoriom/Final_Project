#include "tim.h"

gpio_config_t timer_get_pin_config(void *Timer, timer_channel_t channel)
{
    gpio_config_t config = {
        .port   = NULL,
        .mode   = GPIO_MODE_ALTERNATE,
        .otype  = GPIO_OTYPE_PUSHPULL,
        .ospeed = GPIO_OSPEED_HIGH,
        .pupd   = GPIO_PUPD_NONE,
        .alt_func = 0
    };

    if(Timer == TIM2)
        config.alt_func = 1;    // All TIM2 channels use AF1
        switch (channel) {
        case TIM_CHANNEL1:      // Options: PA0, PA5, PA15. Using PA5 (Nucleo Green LED).
            config.port = GPIOA; config.pin = 5;
            break;
        case TIM_CHANNEL2:      // Options: PA1, PB3. Using PB3.
            config.port = GPIOB; config.pin = 3;
            break;
        case TIM_CHANNEL3:      // Options: PA2, PB10. Using PB10.
            config.port = GPIOB; config.pin = 10;
            break;
        case TIM_CHANNEL4:      // Options: PA3, PB11. Using PB11.
            config.port = GPIOB; config.pin = 11;
            break;
        }
    if(Timer == TIM3)
        config.alt_func = 2;    // All TIM3 channels use AF2
        switch (channel) {
        case TIM_CHANNEL1:      // Options: PA6, PB4, PC6. Using PA6.
            config.port = GPIOA; config.pin = 6;
            break;
        case TIM_CHANNEL2:      // Options: PA7, PB5, PC7. Using PA7.
            config.port = GPIOA; config.pin = 7;
            break;
        case TIM_CHANNEL3:      // Options: PB0, PC8. Using PB0.
            config.port = GPIOB; config.pin = 0;
            break;
        case TIM_CHANNEL4:      // Options: PB1, PC9. Using PB1.
            config.port = GPIOB; config.pin = 1;
            break;
        }
    if(Timer == TIM4)
        config.alt_func = 2;    // All TIM4 channels use AF2
        switch (channel) {
        case TIM_CHANNEL1:      // Options: PB6, PD12. Using PB6.
            config.port = GPIOA; config.pin = 6;
            break;
        case TIM_CHANNEL2:      // Options: PB7, PD13. Using PB7.
            config.port = GPIOB; config.pin = 7;
            break;
        case TIM_CHANNEL3:      // Options: PB8, PD14. Using PB8.
            config.port = GPIOB; config.pin = 8;
            break;
        case TIM_CHANNEL4:      // Options: PB9, PD15. Using PB9.
            config.port = GPIOB; config.pin = 9;
            break;
        }
    if(Timer == TIM5)
        config.alt_func = 2;    // All TIM5 channels use AF2
        switch (channel) {
        case TIM_CHANNEL1:      // Options: PA0, PH10. Using PA0.
            config.port = GPIOA; config.pin = 0;
            break;
        case TIM_CHANNEL2:      // Options: PA1, PH11. Using PA1.
            config.port = GPIOA; config.pin = 1;
            break;
        case TIM_CHANNEL3:      // Options: PA2, PH12. Using PA2.
            config.port = GPIOA; config.pin = 2;
            break;
        case TIM_CHANNEL4:      // Options: PA3, PI0. Using PA3.
            config.port = GPIOA; config.pin = 3;
            break;
        }

    return config;
}

void timer_clock_enable(void *Timer)
{
    if(Timer == TIM1) rcc_tim_clock_enable(1);
    else if(Timer == TIM2) rcc_tim_clock_enable(2);
    else if(Timer == TIM3) rcc_tim_clock_enable(3);
    else if(Timer == TIM4) rcc_tim_clock_enable(4);
    else if(Timer == TIM5) rcc_tim_clock_enable(5);
    else if(Timer == TIM6) rcc_tim_clock_enable(6);
    else if(Timer == TIM7) rcc_tim_clock_enable(7);
    else if(Timer == TIM8) rcc_tim_clock_enable(8);
    else if(Timer == TIM15) rcc_tim_clock_enable(15);
    else if(Timer == TIM16) rcc_tim_clock_enable(16);
    else if(Timer == TIM17) rcc_tim_clock_enable(17);
    else return;
}

void pwm_init(pwm_config_t *config)
{
    GeneralPurpose_Timer_t *TIMx = config->pwmTimer;

    // 1. Enable Timer Clock
    timer_clock_enable(TIMx);

    // 2. Get the complete GPIO configuration for the required timer and channel
    gpio_config_t pin_config = timer_get_pin_config(TIMx, config->pwmChannel);
    if(pin_config.port == NULL)
        return; // Exit if no valid pin mapping found
    // Initialize the GPIO pin using the retrieved configuration
    gpio_init(&pin_config); // This is your new, powerful GPIO init function

    // 3. Configure Timer Base: Prescaler and Period
    TIMx->PSC = config->prescaler - 1;
    TIMx->ARR = config->period - 1;

    // 4. Configure PWM Channel
    volatile uint32_t *ccmr_reg = (config->pwmChannel < TIM_CHANNEL3) ? &TIMx->CCMR1 : &TIMx->CCMR2;
    uint8_t shift = (config->pwmChannel % 2) * 8;   // 0 for ch1/3, 8 for ch2/4

    // Set PWM Mode 1 (OCxM bits = 110) and enable Preload (OCxPE bit = 1)
    // Preload enable is crucial for glitch-free duty cycle updates.
    *ccmr_reg &= ~((0x7U << (4 + shift)) | (0x1U << (3 + shift))); // Clear previous settings
    *ccmr_reg |= (0x6U << (4 + shift));   // OCxM = 110 for PWM Mode 1
    *ccmr_reg |= (0x1U << (3 + shift));   // OCxPE = 1 (Output Compare Preload Enable)

    // 5. Enable the specific PWM Channel Output
    TIMx->CCER |= (0x1U << (4 * config->pwmChannel));

    // 6. Main Output Enable and Counter Start
    TIMx->CR1 |= (0x1U << 7);   // ARPE: Auto-reload preload enable
    TIMx->EGR |= (0x1U << 0);   // UG: Generate and update event to load PSC and ARR
    TIMx->CR1 |= (0x1U << 0);   // CEN: Counter enable. Starts the timer.
}

void pwm_set_dutyCycle(GeneralPurpose_Timer_t *TIMx, timer_channel_t channel, int dutyCycle)
{
    if(dutyCycle < 0 || dutyCycle > 100)
        return;

    // The period is the value in ARR + 1
    uint32_t period = TIMx->ARR;
    uint32_t crrValue = ((period + 1) * dutyCycle) / 100;

    switch (channel) {
        case TIM_CHANNEL1: TIMx->CCR1 = crrValue; break;
        case TIM_CHANNEL2: TIMx->CCR2 = crrValue; break;
        case TIM_CHANNEL3: TIMx->CCR3 = crrValue; break;
        case TIM_CHANNEL4: TIMx->CCR4 = crrValue; break;
        default: return;
    }
}
