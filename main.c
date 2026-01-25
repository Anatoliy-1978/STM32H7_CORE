/*
 * main.c    Board STM32H7_CORE
 * 03.01.2026
 */
 
/* Includes */
#include "stm32h7xx.h"
#include "main.h"
#include "tim6_7.h" 
#include "usart1.h" 
#include "fmc_sdram.h"
#include <stdint.h>
#include <stdio.h>

/* Function prototypes */

void USART1_Init(void); 
void System_Clock_Init(void);
void GPIO_Init(void);
void SDRAM_Init_W9825G6KH(void);
void SDRAM_SingleTest(void);


/* Function prototypes END */
 
/* USER CODE 0 */
void GPIO_Init(void)
{
    // 1. Включаем тактирование порта C
    // Port C находится на шине AHB4 (GPIOIOPCEN)
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;
    
    // 2. Короткая пауза для стабилизации тактирования
    volatile uint32_t temp = RCC->AHB4ENR;
    (void)temp;
    
    // 3. Настраиваем PC13 как выход Push-Pull
    // Сначала очищаем биты MODER13 (биты 26-27)
    GPIOC->MODER &= ~(GPIO_MODER_MODE13_Msk);
    
    // Устанавливаем MODER13 = 01 (Output mode)
    GPIOC->MODER |= (1U << (13 * 2));  // 01 << 26
    
    // 4. Настраиваем тип выхода (OT13) - Push-Pull
    // По умолчанию 0 (Push-Pull), но для ясности установим явно
    GPIOC->OTYPER &= ~(1U << 13);  // 0 = Push-Pull
    
    // 5. Настраиваем скорость (OSPEEDR13)
    // Для светодиода достаточно средней скорости
    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED13_Msk);
    GPIOC->OSPEEDR |= (2U << (13 * 2));  // Medium speed
    
    // 6. Инициализируем в выключенном состоянии
    // На плате NUCLEO-H743ZI светодиод зажигается при LOW уровне
    // (активный низкий уровень)
    //GPIOC->ODR |= (1U << 13);  // Устанавливаем HIGH - светодиод ВЫКЛ
}
/* USER CODE END 0 */
 
int main(void)
{   /*=== initialization of the UART === */
    USART1_Init();
    USART1_SendString("===Start_STM32H743IIT6===...Ok\n\r");
   
    
    /*=== clock frequency initialization ===*/
    USART1_SendString("===System_Clock_Init===\n\r");
    System_Clock_Init();
    USART1_SendString("System_Clock_Init...Ok\n\r");
    
    GPIO_Init();
    
    // Инициализация и запуск таймера
    SysTick_Tim_Init();
    SysTick_Tim_Enable();
     
    SDRAM_Init_W9825G6KH();
    
    SDRAM_SingleTest();
    
    while(1)
    {
        
    }        
    
}

/* USER CODE 1 */

void SDRAM_SingleTest(void)
{
    volatile uint32_t *p = (uint32_t *)SDRAM_BASE;
    uint32_t pattern = 0xDEADBEEFU;

    USART1_SendString("\r\n[SDRAM SINGLE TEST]\r\n");

    USART1_SendString("Writing value: ");
    USART1_SendHex32(pattern);
    USART1_SendString(" to address: ");
    USART1_SendHex32((uint32_t)p);
    USART1_SendString("\r\n");

    *p = pattern;   // запись

    uint32_t read = *p;  // чтение

    USART1_SendString("Read back:     ");
    USART1_SendHex32(read);
    USART1_SendString("\r\n");

    if (read == pattern)
        USART1_SendString("RESULT: OK\r\n");
    else
        USART1_SendString("RESULT: FAIL\r\n");
}
   
/* USER CODE END 1 */
/*===========================================================================*/ 
/* main.c END */
 

 



