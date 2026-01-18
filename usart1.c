/*
 * usart1.c    Board STM32H7_CORE
 * 03.01.2026
 */
 
/* Includes */
#include "usart1.h"
 
 
/* Function prototypes */
 
/* Function prototypes END */

/* USER CODE 0 */
void USART1_Init(void)
{
    //включаем тактирование GPIOA
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
    //включаем USART1 на шине APB2  
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    //очищаем регистр режима ввода/вывода GPIOA
    GPIOA->MODER &= ~(GPIO_MODER_MODE9_Msk | GPIO_MODER_MODE10_Msk);
    // Устанавливаем значение 0х02, режим порта альтернативные функции (AF)
    GPIOA->MODER |= (GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1);
    // включаем подтяжку RX на PA10
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD9_Msk | GPIO_PUPDR_PUPD10_Msk);
    GPIOA->PUPDR |=  GPIO_PUPDR_PUPD10_0;
    
    // Выбираем альтернативную функцию для PA9/PA10 AF7 верхнего регистра
    // очищаем регистр AFR[1] порты PA9/PA10
    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL9_Msk | GPIO_AFRH_AFSEL10_Msk);
    //устанавливаем AF7 в регистр AFR[1] порты PA9/PA10
    GPIOA->AFR[1] |= ((7 << GPIO_AFRH_AFSEL9_Pos) | (7 << GPIO_AFRH_AFSEL10_Pos));
    
    /* Устанавливаем BRR для 64 МГц и 115200 бод
     * Расчет: USARTDIV = 64,000,000 / (16 * 115,200) = 34,722222
     * Mantissa = 34 (0x22), Fraction = 12 (0.722222 * 16 = 11,555552 ~ 12)
     * BRR = (34 << 4) | 12 = 0x22C
     */
    
    USART1->BRR = (0x22 << USART_BRR_DIV_MANTISSA_Pos) |  // DIV_Mantissa = 34
                  (0xC << USART_BRR_DIV_FRACTION_Pos);    // DIV_Fraction = 12
   // uint32_t pclk2    = 64000000U;
    //uint32_t baudrate = 115200U;
    //uint32_t usartdiv = (pclk2 + (baudrate / 2U)) / baudrate;
    // настройка скорости
    //USART1->BRR = usartdiv;
    USART1->CR1 = 0;                 // сброс настроек
    USART1->CR1 |= USART_CR1_TE;     // включить передатчик
    USART1->CR1 |= USART_CR1_RE;     // включить приёмник
    USART1->CR1 |= USART_CR1_UE;     // включить USART
}


/* USER CODE 2 */
void USART1_SendChar(char c)
{
    while ((USART1->ISR & USART_ISR_TXE_TXFNF) == 0U)
    {
        // æä¸ì, ïîêà ïåðåäàòî÷íûé áóôåð îñâîáîäèòñÿ
    }
    USART1->TDR = (uint8_t)c;
}
/* USER CODE END 2 */
void USART1_SendString(const char *s)
{
    while (*s != '\0')
    {
        USART1_SendChar(*s);
        s++;
    }
}
/* USER CODE 3 */

/* USER CODE END 3 */

/* USER CODE 4 */

/* USER CODE END 4 */


/* usart1.c END */
