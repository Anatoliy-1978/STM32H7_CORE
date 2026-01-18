/*
 * tim6_7.h   Board STM32H7_CORE
 * 03.01.2026
 */
#ifndef __TIM6_7_H__
#define __TIM6_7_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx.h"
#include <stdint.h>

    // Макросы для удобства
    // Константа для частоты таймера TIM6
    // Из вашего кода RCC: PCLK1 (APB1 периферия) = 50 МГц
    // Согласно Reference Manual (раздел 8.7.9):
    // - Если APB1 prescaler (D2PPRE1) ? 1, то TIMxCLK = APB1_CLK ? 2
    // - У вас D2PPRE1 = DIV4, значит TIM6_CLK = 50 МГц * 2 = 100 МГц
#define TIM6_CLOCK_FREQ_HZ 100000000UL  // 100 МГц




// Прототипы функций
void SysTick_Tim_Init(void);
void SysTick_Tim_Enable(void);
void SysTick_Tim_Disable(void);
uint32_t SysTick_Tim_GetTick(void);
void TIM6_DAC_IRQHandler(void);
void delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif // __TIM6_7_H__
