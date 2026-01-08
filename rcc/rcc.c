/*
 * rcc.c    Board STM32H7_CORE
 * 08.01.2026
 */
 
/* Includes */
#include "rcc.h"
 
 
/* Function prototypes */
 
/* Function prototypes END */

/* USER CODE 0 */
void System_Clock_Init(void)
{
  RCC->APB4ENR |= RCC_APB4ENR_PWREN;
  __DSB();
}
/* USER CODE END 0 */

/* USER CODE 2 */

/* USER CODE END 2 */

/* USER CODE 3 */

/* USER CODE END 3 */

/* USER CODE 4 */

/* USER CODE END 4 */

/* rcc.c END */
