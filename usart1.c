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
        // ждём, пока передаточный буфер освободится
    }
    USART1->TDR = (uint8_t)c;
}
/* USER CODE END 2 */

/* USER CODE 3 */
void USART1_SendString(const char *s)
{
    while (*s != '\0')
    {
        USART1_SendChar(*s);
        s++;
    }
}
/* USER CODE END 3 */

/* USER CODE 4 */
void USART1_SendDecimal(uint32_t num)
{
    char buffer[10];
    int i = 0;
    
    if (num == 0) {
        USART1_SendChar('0');
        return;
    }
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        USART1_SendChar(buffer[--i]);
    }
}
/* USER CODE END 4 */

/* USER CODE 5 */
void USART1_SendHex(uint32_t num, uint8_t width)
{
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[9];
    int i;
    
    if (num == 0 && width == 0) {
        USART1_SendString("0");
        return;
    }
    
    // Заполняем буфер с конца
    for (i = 7; i >= 0; i--) {
        buffer[i] = hex_chars[num & 0xF];
        num >>= 4;
    }
    
    // Определяем сколько символов выводить
    int start = 8 - width;
    if (start < 0) start = 0;
    
    // Пропускаем ведущие нули, если width не задан
    if (width == 0) {
        start = 0;
        while (start < 7 && buffer[start] == '0') {
            start++;
        }
    }
    
    // Выводим префикс 0x
    USART1_SendString("0x");
    
    // Выводим оставшиеся символы
    for (i = start; i < 8; i++) {
        USART1_SendChar(buffer[i]);
    }
}
/* USER CODE END 5 */

/* USER CODE 6 */
// Быстрая версия для 8-битных значений
void USART1_SendHex8(uint8_t num)
{
    char hex_chars[] = "0123456789ABCDEF";
    USART1_SendString("0x");
    USART1_SendChar(hex_chars[(num >> 4) & 0xF]);
    USART1_SendChar(hex_chars[num & 0xF]);
}

// Версия для 16-битных значений
void USART1_SendHex16(uint16_t num)
{
    USART1_SendHex(num, 4);
}

// Версия для 32-битных значений
void USART1_SendHex32(uint32_t num)
{
    USART1_SendHex(num, 8);
}
/* USER CODE END 6 */

/* usart1.c END */


