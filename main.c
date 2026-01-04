/*
 * main.c    Board STM32H7_CORE
 * 03.01.2026
 */
 
/* Includes */
#include "stm32h7xx.h"
#include "main.h"
 
 
/* Function prototypes */
void USART1_Init(void); 
void USART1_SendChar(char c);
/* Function prototypes END */
 
/* USER CODE 0 */

/* USER CODE END 0 */
 
int main(void)
{
    USART1_Init();
    USART1_SendString("HELLOU...");
    USART1_SendString("Ok\n\r");
    while(1)
    {
        
    }        
    
}
 
/* main.c END */
 
 