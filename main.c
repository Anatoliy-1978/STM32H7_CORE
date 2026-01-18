/*
 * main.c    Board STM32H7_CORE
 * 03.01.2026
 */
 
/* Includes */
#include "stm32h7xx.h"
#include "main.h"
 
 
/* Function prototypes */
void USART1_Init(void); 
void System_Clock_Init(void);
/* Function prototypes END */
 
/* USER CODE 0 */

/* USER CODE END 0 */
 
int main(void)
{
    /*=== initialization of the UART === */
    USART1_Init();
    USART1_SendString("===Start_STM32H743IIT6===...");
    USART1_SendString("Ok\n\r");
    
    /*=== clock frequency initialization ===*/
    USART1_SendString("===System_Clock_Init===...");
    System_Clock_Init();
    USART1_SendString("Ok\n\r");
 
    while(1)
    {
        
    }        
    
}
 
/* main.c END */
 

 


