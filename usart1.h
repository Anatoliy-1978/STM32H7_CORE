/*
 * usart1.h   Board STM32H7_CORE
 * 03.01.2026
 */
#ifndef __USART1_H__
#define __USART1_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx.h"

// Инициализация USART1

void USART1_Init(void);


// Отправка одного символа
void USART1_SendChar(char c);

// Отправка строки
void USART1_SendString(const char *s);

// Отправка десятичного числа
void USART1_SendDecimal(uint32_t num);

// Отправка шеснадчати ричного
void USART1_SendHex(uint32_t num);
#ifdef __cplusplus
}
#endif

#endif // __USART1_H__


