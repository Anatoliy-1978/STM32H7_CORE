/*
 * main.c    Board STM32H7_CORE
 * 03.01.2026
 */
 
/* Includes */
#include "stm32h7xx.h"
#include "main.h"
 
 
/* Function prototypes */
void USART1_Init(void); 
void USART1_SendString(const char *s);
/* Function prototypes END */
 
/* USER CODE 0 */

/* USER CODE END 0 */
 
int main(void)
{
    USART1_Init();
    USART1_SendString("===Start_STM32H743IIT6===...");
    USART1_SendString("Ok\n\r");
    while(1)
    {
        
    }        
    
}
 
/* main.c END */
 

 
