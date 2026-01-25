/*
 * fmc_sdram.c    Board STM32H7_CORE
 * 03.01.2026
 */
 
/* Includes */
#include "fmc_sdram.h"
#include "usart1.h" 
#include <stdbool.h>
#include "tim6_7.h"

/* Function prototypes */
void SDRAM_Init_W9825G6KH(void)
{
    FMC_GPIO_Init();
    SDRAM_InitFMC_Registers();
    SDRAM_InitializationSequence();
} 
/* Function prototypes END */

/* USER CODE 0 */
void FMC_GPIO_Init(void)
{
/** FMC GPIO Configuration
    PF0   ------> FMC_A0
    PF1   ------> FMC_A1
    PF2   ------> FMC_A2
    PF3   ------> FMC_A3
    PF4   ------> FMC_A4
    PF5   ------> FMC_A5
    PH2   ------> FMC_SDCKE0
    PH3   ------> FMC_SDNE0
    PH5   ------> FMC_SDNWE
    PF11   ------> FMC_SDNRAS
    PF12   ------> FMC_A6
    PF13   ------> FMC_A7
    PF14   ------> FMC_A8
    PF15   ------> FMC_A9
    PG0   ------> FMC_A10
    PG1   ------> FMC_A11
    PE7   ------> FMC_D4
    PE8   ------> FMC_D5
    PE9   ------> FMC_D6
    PE10   ------> FMC_D7
    PE11   ------> FMC_D8
    PE12   ------> FMC_D9
    PE13   ------> FMC_D10
    PE14   ------> FMC_D11
    PE15   ------> FMC_D12
    PD8   ------> FMC_D13
    PD9   ------> FMC_D14
    PD10   ------> FMC_D15
    PD14   ------> FMC_D0
    PD15   ------> FMC_D1
    PG2   ------> FMC_A12
    PG4   ------> FMC_BA0
    PG5   ------> FMC_BA1
    PG8   ------> FMC_SDCLK
    PD0   ------> FMC_D2
    PD1   ------> FMC_D3
    PG15   ------> FMC_SDNCAS
    PE0   ------> FMC_NBL0
    PE1   ------> FMC_NBL1
    */
    
    USART1_SendString("[FMC] Starting SDRAM initialization...\n\r");
    // 1. Включаем тактирование FMC и GPIO
    USART1_SendString("[FMC] Step 1: Enabling clock...");
    RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;                  // включения тактирования FMC
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN |               // включения тактирования GPIOD    
                    RCC_AHB4ENR_GPIOEEN |               // включения тактирования GPIOE
                    RCC_AHB4ENR_GPIOFEN |               // включения тактирования GPIOF
                    RCC_AHB4ENR_GPIOGEN |               // включения тактирования GPIOG
                    RCC_AHB4ENR_GPIOHEN ;               // включения тактирования GPIOH
   
    //for (volatile  uint32_t  i = 0; i < 10; i++);
    delay_ms(10);
    USART1_SendString("OK\n\r");
    
    // 2. Настраиваем пины GPIO для FMC (SDRAM Bank 1)
    // GPIOD
    // Режим: Alternate function (10)
    USART1_SendString("[FMC] GPIO configuration completed:\n\r");
    GPIOD->MODER &= ~(GPIO_MODER_MODE0_Msk | GPIO_MODER_MODE1_Msk | GPIO_MODER_MODE8_Msk |
                      GPIO_MODER_MODE9_Msk | GPIO_MODER_MODE10_Msk | GPIO_MODER_MODE14_Msk | 
                      GPIO_MODER_MODE15_Msk);
    GPIOD->MODER |= GPIO_MODER_MODE0_1 |                // AF для PD0
                    GPIO_MODER_MODE1_1 |                // AF для PD1
                    GPIO_MODER_MODE8_1 |                // AF для PD8
                    GPIO_MODER_MODE9_1 |                // AF для PD9
                    GPIO_MODER_MODE10_1 |               // AF для PD10
                    GPIO_MODER_MODE14_1 |               // AF для PD14
                    GPIO_MODER_MODE15_1;                // AF для PD15
                    
    // Скорость: Very high speed (11)
    GPIOD->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0_Msk | GPIO_OSPEEDR_OSPEED1_Msk | GPIO_OSPEEDR_OSPEED8_Msk |
                        GPIO_OSPEEDR_OSPEED9_Msk | GPIO_OSPEEDR_OSPEED10_Msk | GPIO_OSPEEDR_OSPEED14_Msk |
                        GPIO_OSPEEDR_OSPEED15_Msk);
    GPIOD->OSPEEDR |= GPIO_OSPEEDR_OSPEED0_0 | GPIO_OSPEEDR_OSPEED0_1 |
                      GPIO_OSPEEDR_OSPEED1_0 | GPIO_OSPEEDR_OSPEED1_1 |
                      GPIO_OSPEEDR_OSPEED8_0 | GPIO_OSPEEDR_OSPEED8_1 |
                      GPIO_OSPEEDR_OSPEED9_0 | GPIO_OSPEEDR_OSPEED9_1 |
                      GPIO_OSPEEDR_OSPEED10_0 | GPIO_OSPEEDR_OSPEED10_1 |
                      GPIO_OSPEEDR_OSPEED14_0 | GPIO_OSPEEDR_OSPEED14_1 |
                      GPIO_OSPEEDR_OSPEED15_0 | GPIO_OSPEEDR_OSPEED15_1 ;
                      
    // Тип: Push-pull
    GPIOD->OTYPER &= ~(GPIO_OTYPER_OT0_Msk | GPIO_OTYPER_OT1_Msk | GPIO_OTYPER_OT8_Msk | 
                      GPIO_OTYPER_OT9_Msk | GPIO_OTYPER_OT10_Msk | GPIO_OTYPER_OT14_Msk |
                      GPIO_OTYPER_OT15_Msk); 
   
    // Подтяжка: No pull-up/pull-down
    GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPD0_Msk | GPIO_PUPDR_PUPD1_Msk | GPIO_PUPDR_PUPD8_Msk |
                     GPIO_PUPDR_PUPD9_Msk | GPIO_PUPDR_PUPD10_Msk | GPIO_PUPDR_PUPD14_Msk |
                     GPIO_PUPDR_PUPD15_Msk);
   
    // Устанавливаем альтернативную функцию AF12 (FMC)
    // AF12 = 1100 в бинарном = 0xC в шестнадцатеричном
    GPIOD->AFR[0] &= ~(GPIO_AFRL_AFSEL0_Msk | GPIO_AFRL_AFSEL1_Msk);
    GPIOD->AFR[1] &= ~(GPIO_AFRH_AFSEL8_Msk | GPIO_AFRH_AFSEL9_Msk | GPIO_AFRH_AFSEL10_Msk |
                       GPIO_AFRH_AFSEL14_Msk |GPIO_AFRH_AFSEL15_Msk);
    GPIOD->AFR[0] |= GPIO_AFRL_AFSEL0_2 | GPIO_AFRL_AFSEL0_3 |
                     GPIO_AFRL_AFSEL1_2 | GPIO_AFRL_AFSEL1_3;
    GPIOD->AFR[1] |= GPIO_AFRH_AFSEL8_2 | GPIO_AFRH_AFSEL8_3 |   
                     GPIO_AFRH_AFSEL9_2 | GPIO_AFRH_AFSEL9_3 |
                     GPIO_AFRH_AFSEL10_2 | GPIO_AFRH_AFSEL10_3 |
                     GPIO_AFRH_AFSEL14_2 | GPIO_AFRH_AFSEL14_3 |
                     GPIO_AFRH_AFSEL15_2 | GPIO_AFRH_AFSEL15_3 ;    
    USART1_SendString("  - Port D: D0-D3, D13-D15\n\r");
    
    // GPIOE  
    
    GPIOE->MODER &= ~(GPIO_MODER_MODE0_Msk | GPIO_MODER_MODE1_Msk | GPIO_MODER_MODE7_Msk |
                      GPIO_MODER_MODE8_Msk | GPIO_MODER_MODE9_Msk | GPIO_MODER_MODE10_Msk | 
                      GPIO_MODER_MODE11_Msk | GPIO_MODER_MODE12_Msk | GPIO_MODER_MODE13_Msk | 
                      GPIO_MODER_MODE14_Msk | GPIO_MODER_MODE15_Msk);
    GPIOE->MODER |= GPIO_MODER_MODE0_1 |                // AF для PE0
                    GPIO_MODER_MODE1_1 |                // AF для PE1
                    GPIO_MODER_MODE7_1 |                // AF для PE7
                    GPIO_MODER_MODE8_1 |                // AF для PE8
                    GPIO_MODER_MODE9_1 |                // AF для PE9
                    GPIO_MODER_MODE10_1 |               // AF для PE10
                    GPIO_MODER_MODE11_1 |               // AF для PE11
                    GPIO_MODER_MODE12_1 |               // AF для PE12
                    GPIO_MODER_MODE13_1 |               // AF для PE13
                    GPIO_MODER_MODE14_1 |               // AF для PE14
                    GPIO_MODER_MODE15_1 ;               // AF для PE15
                    
    // Скорость: Very high speed (11)
    GPIOE->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0_Msk | GPIO_OSPEEDR_OSPEED1_Msk | GPIO_OSPEEDR_OSPEED7_Msk |
                        GPIO_OSPEEDR_OSPEED8_Msk | GPIO_OSPEEDR_OSPEED9_Msk | GPIO_OSPEEDR_OSPEED10_Msk |
                        GPIO_OSPEEDR_OSPEED11_Msk | GPIO_OSPEEDR_OSPEED12_Msk | GPIO_OSPEEDR_OSPEED13_Msk |
                        GPIO_OSPEEDR_OSPEED14_Msk | GPIO_OSPEEDR_OSPEED15_Msk);
    GPIOE->OSPEEDR |= GPIO_OSPEEDR_OSPEED0_0 | GPIO_OSPEEDR_OSPEED0_1 |
                      GPIO_OSPEEDR_OSPEED1_0 | GPIO_OSPEEDR_OSPEED1_1 |
                      GPIO_OSPEEDR_OSPEED7_0 | GPIO_OSPEEDR_OSPEED7_1 |
                      GPIO_OSPEEDR_OSPEED8_0 | GPIO_OSPEEDR_OSPEED8_1 |
                      GPIO_OSPEEDR_OSPEED9_0 | GPIO_OSPEEDR_OSPEED9_1 |
                      GPIO_OSPEEDR_OSPEED10_0 | GPIO_OSPEEDR_OSPEED10_1 |
                      GPIO_OSPEEDR_OSPEED11_0 | GPIO_OSPEEDR_OSPEED11_1 |
                      GPIO_OSPEEDR_OSPEED12_0 | GPIO_OSPEEDR_OSPEED12_1 |
                      GPIO_OSPEEDR_OSPEED13_0 | GPIO_OSPEEDR_OSPEED13_1 |
                      GPIO_OSPEEDR_OSPEED14_0 | GPIO_OSPEEDR_OSPEED14_1 |
                      GPIO_OSPEEDR_OSPEED15_0 | GPIO_OSPEEDR_OSPEED15_1 ;
                      
    // Тип: Push-pull
    GPIOE->OTYPER &= ~(GPIO_OTYPER_OT0_Msk | GPIO_OTYPER_OT1_Msk | GPIO_OTYPER_OT7_Msk | 
                      GPIO_OTYPER_OT8_Msk | GPIO_OTYPER_OT9_Msk | GPIO_OTYPER_OT10_Msk |
                      GPIO_OTYPER_OT11_Msk | GPIO_OTYPER_OT12_Msk | GPIO_OTYPER_OT13_Msk |
                      GPIO_OTYPER_OT14_Msk | GPIO_OTYPER_OT15_Msk); 
   
    // Подтяжка: No pull-up/pull-down
    GPIOE->PUPDR &= ~(GPIO_PUPDR_PUPD0_Msk | GPIO_PUPDR_PUPD1_Msk | GPIO_PUPDR_PUPD7_Msk |
                     GPIO_PUPDR_PUPD8_Msk | GPIO_PUPDR_PUPD9_Msk | GPIO_PUPDR_PUPD10_Msk |
                     GPIO_PUPDR_PUPD11_Msk | GPIO_PUPDR_PUPD12_Msk | GPIO_PUPDR_PUPD13_Msk |
                     GPIO_PUPDR_PUPD14_Msk | GPIO_PUPDR_PUPD15_Msk);
   
    // Устанавливаем альтернативную функцию AF12 (FMC)
    // AF12 = 1100 в бинарном = 0xC в шестнадцатеричном
    GPIOE->AFR[0] &= ~(GPIO_AFRL_AFSEL0_Msk | GPIO_AFRL_AFSEL1_Msk | GPIO_AFRL_AFSEL7_Msk); 
    GPIOE->AFR[1] &= ~(GPIO_AFRH_AFSEL8_Msk | GPIO_AFRH_AFSEL9_Msk | GPIO_AFRH_AFSEL10_Msk |
                       GPIO_AFRH_AFSEL11_Msk | GPIO_AFRH_AFSEL12_Msk | GPIO_AFRH_AFSEL13_Msk |
                       GPIO_AFRH_AFSEL14_Msk | GPIO_AFRH_AFSEL15_Msk);
    GPIOE->AFR[0] |= GPIO_AFRL_AFSEL0_2 | GPIO_AFRL_AFSEL0_3 |
                     GPIO_AFRL_AFSEL1_2 | GPIO_AFRL_AFSEL1_3 |
                     GPIO_AFRL_AFSEL7_2 | GPIO_AFRL_AFSEL7_3 ;   
    GPIOE->AFR[1] |= GPIO_AFRH_AFSEL8_2 | GPIO_AFRH_AFSEL8_3 |
                     GPIO_AFRH_AFSEL9_2 | GPIO_AFRH_AFSEL9_3 |
                     GPIO_AFRH_AFSEL10_2 | GPIO_AFRH_AFSEL10_3 |
                     GPIO_AFRH_AFSEL11_2 | GPIO_AFRH_AFSEL11_3 ;
    USART1_SendString("  - Port E: D4-D12, NBL0-1\n\r");
                    
    // GPIOF
    
    GPIOF->MODER &= ~(GPIO_MODER_MODE0_Msk | GPIO_MODER_MODE1_Msk | GPIO_MODER_MODE2_Msk |
                      GPIO_MODER_MODE3_Msk | GPIO_MODER_MODE4_Msk | GPIO_MODER_MODE5_Msk | 
                      GPIO_MODER_MODE11_Msk | GPIO_MODER_MODE12_Msk | GPIO_MODER_MODE13_Msk | 
                      GPIO_MODER_MODE14_Msk | GPIO_MODER_MODE15_Msk);
    GPIOF->MODER |= GPIO_MODER_MODE0_1 |                // AF для PF0
                    GPIO_MODER_MODE1_1 |                // AF для PF1
                    GPIO_MODER_MODE2_1 |                // AF для PF2
                    GPIO_MODER_MODE3_1 |                // AF для PF3
                    GPIO_MODER_MODE4_1 |                // AF для PF4
                    GPIO_MODER_MODE5_1 |                // AF для PF5
                    GPIO_MODER_MODE11_1 |               // AF для PF11
                    GPIO_MODER_MODE12_1 |               // AF для PF12
                    GPIO_MODER_MODE13_1 |               // AF для PF13
                    GPIO_MODER_MODE14_1 |               // AF для PF14
                    GPIO_MODER_MODE15_1 ;               // AF для PF15
                    
    // Скорость: Very high speed (11)
    GPIOF->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0_Msk | GPIO_OSPEEDR_OSPEED1_Msk | GPIO_OSPEEDR_OSPEED2_Msk |
                        GPIO_OSPEEDR_OSPEED3_Msk | GPIO_OSPEEDR_OSPEED4_Msk | GPIO_OSPEEDR_OSPEED5_Msk |
                        GPIO_OSPEEDR_OSPEED11_Msk | GPIO_OSPEEDR_OSPEED12_Msk | GPIO_OSPEEDR_OSPEED13_Msk |
                        GPIO_OSPEEDR_OSPEED14_Msk | GPIO_OSPEEDR_OSPEED15_Msk);
    GPIOF->OSPEEDR |= GPIO_OSPEEDR_OSPEED0_0 | GPIO_OSPEEDR_OSPEED0_1 |
                      GPIO_OSPEEDR_OSPEED1_0 | GPIO_OSPEEDR_OSPEED1_1 |
                      GPIO_OSPEEDR_OSPEED2_0 | GPIO_OSPEEDR_OSPEED2_1 |
                      GPIO_OSPEEDR_OSPEED3_0 | GPIO_OSPEEDR_OSPEED3_1 |
                      GPIO_OSPEEDR_OSPEED4_0 | GPIO_OSPEEDR_OSPEED4_1 |
                      GPIO_OSPEEDR_OSPEED5_0 | GPIO_OSPEEDR_OSPEED5_1 |
                      GPIO_OSPEEDR_OSPEED11_0 | GPIO_OSPEEDR_OSPEED11_1 |
                      GPIO_OSPEEDR_OSPEED12_0 | GPIO_OSPEEDR_OSPEED12_1 |
                      GPIO_OSPEEDR_OSPEED13_0 | GPIO_OSPEEDR_OSPEED13_1 |
                      GPIO_OSPEEDR_OSPEED14_0 | GPIO_OSPEEDR_OSPEED14_1 |
                      GPIO_OSPEEDR_OSPEED15_0 | GPIO_OSPEEDR_OSPEED15_1 ;
                      
    // Тип: Push-pull
    GPIOF->OTYPER &= ~(GPIO_OTYPER_OT0_Msk | GPIO_OTYPER_OT1_Msk | GPIO_OTYPER_OT2_Msk | 
                      GPIO_OTYPER_OT3_Msk | GPIO_OTYPER_OT4_Msk | GPIO_OTYPER_OT5_Msk |
                      GPIO_OTYPER_OT11_Msk | GPIO_OTYPER_OT12_Msk | GPIO_OTYPER_OT13_Msk |
                      GPIO_OTYPER_OT14_Msk | GPIO_OTYPER_OT15_Msk); 
   
    // Подтяжка: No pull-up/pull-down
    GPIOF->PUPDR &= ~(GPIO_PUPDR_PUPD0_Msk | GPIO_PUPDR_PUPD1_Msk | GPIO_PUPDR_PUPD2_Msk |
                     GPIO_PUPDR_PUPD3_Msk | GPIO_PUPDR_PUPD4_Msk | GPIO_PUPDR_PUPD5_Msk |
                     GPIO_PUPDR_PUPD11_Msk | GPIO_PUPDR_PUPD12_Msk | GPIO_PUPDR_PUPD13_Msk |
                     GPIO_PUPDR_PUPD14_Msk | GPIO_PUPDR_PUPD15_Msk);
   
    // Устанавливаем альтернативную функцию AF12 (FMC)
    // AF12 = 1100 в бинарном = 0xC в шестнадцатеричном
    GPIOF->AFR[0] &= ~(GPIO_AFRL_AFSEL0_Msk | GPIO_AFRL_AFSEL1_Msk | GPIO_AFRL_AFSEL2_Msk |
                       GPIO_AFRL_AFSEL3_Msk | GPIO_AFRL_AFSEL4_Msk | GPIO_AFRL_AFSEL5_Msk);
    GPIOF->AFR[1] &= ~(GPIO_AFRH_AFSEL11_Msk | GPIO_AFRH_AFSEL12_Msk | GPIO_AFRH_AFSEL13_Msk |
                       GPIO_AFRH_AFSEL14_Msk | GPIO_AFRH_AFSEL15_Msk); 
    GPIOF->AFR[0] |= GPIO_AFRL_AFSEL0_2 | GPIO_AFRL_AFSEL0_3 |
                     GPIO_AFRL_AFSEL1_2 | GPIO_AFRL_AFSEL1_3 |
                     GPIO_AFRL_AFSEL2_2 | GPIO_AFRL_AFSEL2_3 |   
                     GPIO_AFRL_AFSEL3_2 | GPIO_AFRL_AFSEL3_3 |
                     GPIO_AFRL_AFSEL4_2 | GPIO_AFRL_AFSEL4_3 |
                     GPIO_AFRL_AFSEL5_2 | GPIO_AFRL_AFSEL5_3 ;
    GPIOF->AFR[1] |= GPIO_AFRH_AFSEL11_2 | GPIO_AFRH_AFSEL11_3 |
                     GPIO_AFRH_AFSEL12_2 | GPIO_AFRH_AFSEL12_3 |
                     GPIO_AFRH_AFSEL13_2 | GPIO_AFRH_AFSEL13_3 |
                     GPIO_AFRH_AFSEL14_2 | GPIO_AFRH_AFSEL14_3 | 
                     GPIO_AFRH_AFSEL15_2 | GPIO_AFRH_AFSEL15_3 ;
    USART1_SendString("  - Port F: A0-A9, RAS\n\r");

    // GPIOG

    GPIOG->MODER &= ~(GPIO_MODER_MODE0_Msk | GPIO_MODER_MODE1_Msk | GPIO_MODER_MODE2_Msk |
                      GPIO_MODER_MODE4_Msk | GPIO_MODER_MODE5_Msk | 
                      GPIO_MODER_MODE8_Msk | GPIO_MODER_MODE15_Msk); 
                      
    GPIOG->MODER |= GPIO_MODER_MODE0_1 |                // AF для PG0
                    GPIO_MODER_MODE1_1 |                // AF для PG1
                    GPIO_MODER_MODE2_1 |                // AF для PG2
                    GPIO_MODER_MODE4_1 |                // AF для PG4
                    GPIO_MODER_MODE5_1 |                // AF для PG5
                    GPIO_MODER_MODE8_1 |                // AF для PG8
                    GPIO_MODER_MODE15_1;                // AF для PG15
                    
    // Скорость: Very high speed (11)
    GPIOG->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0_Msk | GPIO_OSPEEDR_OSPEED1_Msk | GPIO_OSPEEDR_OSPEED2_Msk |
                        GPIO_OSPEEDR_OSPEED4_Msk | GPIO_OSPEEDR_OSPEED5_Msk |
                        GPIO_OSPEEDR_OSPEED8_Msk | GPIO_OSPEEDR_OSPEED15_Msk);
    GPIOG->OSPEEDR |= GPIO_OSPEEDR_OSPEED0_0 | GPIO_OSPEEDR_OSPEED0_1 |
                      GPIO_OSPEEDR_OSPEED1_0 | GPIO_OSPEEDR_OSPEED1_1 |
                      GPIO_OSPEEDR_OSPEED2_0 | GPIO_OSPEEDR_OSPEED2_1 |
                      GPIO_OSPEEDR_OSPEED4_0 | GPIO_OSPEEDR_OSPEED4_1 |
                      GPIO_OSPEEDR_OSPEED5_0 | GPIO_OSPEEDR_OSPEED5_1 |
                      GPIO_OSPEEDR_OSPEED8_0 | GPIO_OSPEEDR_OSPEED8_1 |
                      GPIO_OSPEEDR_OSPEED15_0 | GPIO_OSPEEDR_OSPEED15_1;                      
                      
    // Тип: Push-pull
    GPIOG->OTYPER &= ~(GPIO_OTYPER_OT0_Msk | GPIO_OTYPER_OT1_Msk | GPIO_OTYPER_OT2_Msk | 
                      GPIO_OTYPER_OT4_Msk | GPIO_OTYPER_OT5_Msk |
                      GPIO_OTYPER_OT8_Msk | GPIO_OTYPER_OT15_Msk);  
   
    // Подтяжка: No pull-up/pull-down
    GPIOG->PUPDR &= ~(GPIO_PUPDR_PUPD0_Msk | GPIO_PUPDR_PUPD1_Msk | GPIO_PUPDR_PUPD2_Msk |
                     GPIO_PUPDR_PUPD4_Msk | GPIO_PUPDR_PUPD5_Msk |
                     GPIO_PUPDR_PUPD8_Msk | GPIO_PUPDR_PUPD15_Msk);
   
    // Устанавливаем альтернативную функцию AF12 (FMC)
    // AF12 = 1100 в бинарном = 0xC в шестнадцатеричном
    GPIOG->AFR[0] &= ~(GPIO_AFRL_AFSEL0_Msk | GPIO_AFRL_AFSEL1_Msk | GPIO_AFRL_AFSEL2_Msk |
                       GPIO_AFRL_AFSEL4_Msk | GPIO_AFRL_AFSEL5_Msk);
    GPIOG->AFR[1] &= ~(GPIO_AFRH_AFSEL8_Msk | GPIO_AFRH_AFSEL15_Msk);
    GPIOG->AFR[0] |= GPIO_AFRL_AFSEL0_2 | GPIO_AFRL_AFSEL0_3 |
                     GPIO_AFRL_AFSEL1_2 | GPIO_AFRL_AFSEL1_3 |
                     GPIO_AFRL_AFSEL2_2 | GPIO_AFRL_AFSEL2_3 |
                     GPIO_AFRL_AFSEL4_2 | GPIO_AFRL_AFSEL4_3 |
                     GPIO_AFRL_AFSEL5_2 | GPIO_AFRL_AFSEL5_3 ;
    GPIOG->AFR[1] |= GPIO_AFRH_AFSEL8_2 | GPIO_AFRH_AFSEL8_3 |
                     GPIO_AFRH_AFSEL15_2 | GPIO_AFRH_AFSEL15_3;
    USART1_SendString("  - Port G: A10-A12, BA0-1, CLK, CAS\n\r");                 
   
    // GPIOH
    
    GPIOH->MODER &= ~(GPIO_MODER_MODE2_Msk | GPIO_MODER_MODE3_Msk | GPIO_MODER_MODE5_Msk); 
    GPIOH->MODER |= GPIO_MODER_MODE2_1 |                // AF для PH2
                    GPIO_MODER_MODE3_1 |                // AF для PH3
                    GPIO_MODER_MODE5_1 ;                // AF для PH5
                    
    // Скорость: Very high speed (11)
    GPIOH->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED2_Msk | GPIO_OSPEEDR_OSPEED3_Msk | GPIO_OSPEEDR_OSPEED3_Msk);
    GPIOH->OSPEEDR |= GPIO_OSPEEDR_OSPEED2_0 | GPIO_OSPEEDR_OSPEED2_1 |
                      GPIO_OSPEEDR_OSPEED3_0 | GPIO_OSPEEDR_OSPEED3_1 |
                      GPIO_OSPEEDR_OSPEED5_0 | GPIO_OSPEEDR_OSPEED5_1;                      
                      
    // Тип: Push-pull
    GPIOH->OTYPER &= ~(GPIO_OTYPER_OT2_Msk | GPIO_OTYPER_OT3_Msk | GPIO_OTYPER_OT5_Msk); 
   
    // Подтяжка: No pull-up/pull-down
    GPIOH->PUPDR &= ~(GPIO_PUPDR_PUPD2_Msk | GPIO_PUPDR_PUPD3_Msk | GPIO_PUPDR_PUPD5_Msk);
   
    // Устанавливаем альтернативную функцию AF12 (FMC)
    // AF12 = 1100 в бинарном = 0xC в шестнадцатеричном
    GPIOH->AFR[0] &= ~(GPIO_AFRL_AFSEL2_Msk | GPIO_AFRL_AFSEL3_Msk | GPIO_AFRL_AFSEL5_Msk);
    
    GPIOH->AFR[0] |= GPIO_AFRL_AFSEL2_2 | GPIO_AFRL_AFSEL2_3 |
                     GPIO_AFRL_AFSEL3_2 | GPIO_AFRL_AFSEL3_3 |
                     GPIO_AFRL_AFSEL5_2 | GPIO_AFRL_AFSEL5_3;
    USART1_SendString("  - Port H: CKE, CE, WE\n\r");

}  
/* USER CODE END 0 */    

/* USER CODE 1 */

//------------------------------------------------------------------------------
//                      Настройка SDCR/SDTR     
//------------------------------------------------------------------------------

    
void SDRAM_InitFMC_Registers(void)
{
    USART1_SendString("SDCR/SDTR_Timing...");
    RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;
    __DSB(); __ISB();
    
    FMC_Bank1_R->BTCR[0] |= FMC_BCR1_FMCEN;
    FMC_Bank1_R->BTCR[0] |= FMC_BCR1_CCLKEN;
    
    // SDCR
    uint32_t sdcr0 = 0;
    sdcr0 |= FMC_SDCRx_NC_0;                        // NC=01 (9 col)
    sdcr0 |= FMC_SDCRx_NR_1;                        // NR=10 (13 row)
    sdcr0 |= FMC_SDCRx_MWID_0;                      // MWID=01 (16-bit)
    sdcr0 |= FMC_SDCRx_NB;                          // 4 banks
    sdcr0 |= FMC_SDCRx_CAS_0 | FMC_SDCRx_CAS_1;     // CAS=3
    sdcr0 |= FMC_SDCRx_SDCLK_1;                     // SDCLK = FMC/2
    sdcr0 |= FMC_SDCRx_RBURST;                      // Burst Read
    sdcr0 &= ~FMC_SDCRx_RPIPE;                      // RPIPE=0
    FMC_Bank5_6_R->SDCR[0] = sdcr0;
    
    // SDTR
    uint32_t sdtr0 = 0;
    sdtr0 |= (2U << FMC_SDTRx_TMRD_Pos);  // TMRD=2
    sdtr0 |= (7U << FMC_SDTRx_TXSR_Pos);  // TXSR=7
    sdtr0 |= (5U << FMC_SDTRx_TRAS_Pos);  // TRAS=5
    sdtr0 |= (6U << FMC_SDTRx_TRC_Pos);   // TRC =6
    sdtr0 |= (2U << FMC_SDTRx_TWR_Pos);   // TWR =2
    sdtr0 |= (2U << FMC_SDTRx_TRP_Pos);   // TRP =2
    sdtr0 |= (2U << FMC_SDTRx_TRCD_Pos);  // TRCD=2
    FMC_Bank5_6_R->SDTR[0] = sdtr0;
    USART1_SendString("Ok\n\r");
}
/* USER CODE END 1 */

/* USER CODE 2 */

//------------------------------------------------------------------------------
//                         Последовательность SDCMR
//------------------------------------------------------------------------------
void SDRAM_InitializationSequence(void)
{
    USART1_SendString("SDRAM_Sequence...");
    // 1) Clock Configuration Enable
    FMC_Bank5_6_R->SDCMR = FMC_SDCMR_MODE_0 | FMC_SDCMR_CTB1; // MODE=001
    delay_ms(1);  // >=100мкс

    // 2) Precharge All
    FMC_Bank5_6_R->SDCMR = FMC_SDCMR_MODE_1 | FMC_SDCMR_CTB1; // MODE=010
    delay_ms(1);

    // 3) Auto-refresh 8 раз
    FMC_Bank5_6_R->SDCMR =
        (FMC_SDCMR_MODE_0 | FMC_SDCMR_MODE_1)  // MODE=011
        | FMC_SDCMR_CTB1
        | (8U << FMC_SDCMR_NRFS_Pos);
    delay_ms(1);

    // 4) Load Mode Register
    uint32_t mode_reg = 0;
    mode_reg |= (0U << 0);  // BL=1
    mode_reg |= (0U << 3);  // Sequential
    mode_reg |= (3U << 4);  // CAS=3
    mode_reg |= (0U << 9);  // Write burst=0 (burst write)

    FMC_Bank5_6_R->SDCMR =
        FMC_SDCMR_MODE_2        // MODE=100
      | FMC_SDCMR_CTB1
      | (mode_reg << FMC_SDCMR_MRD_Pos);
    delay_ms(1);

    // 5) Refresh timer
    double trow_s = (64.0 / 8192.0) * 1e-3;
    double cycles = trow_s * SDRAM_CLK_HZ;
    double count  = cycles - 20.0;
    if (count < 41.0) count = 41.0;
    uint32_t refresh_count = (uint32_t)(count + 0.5) & 0x1FFF;
    FMC_Bank5_6_R->SDRTR = (refresh_count << FMC_SDRTR_COUNT_Pos);
    USART1_SendString("Ok\n\r");
}
/* USER CODE END 2 */

/* USER CODE 4 */

/* USER CODE END 4 */

/* USER CODE 5 */

/* USER CODE END 5 */

/* USER CODE 6 */

/* USER CODE END 6 */

/* fmc_sdram.c END */
