/*
 * mpu.c    Board STM32H7_CORE
 * 08.01.2026
 */
 
/* Includes */

#include "mpu.h"
#include <stdint.h>
#include <stdio.h>  // Для sprintf
#include "usart1.h"

/* Function prototypes */
    
/* Function prototypes END */

/* USER CODE 0 */
void MPU_Config_External(void)
{
    USART1_SendString("MPU Phase 2: External Memory...");
    
    /* Сохраняем текущее состояние MPU */
    uint32_t mpu_ctrl = MPU->CTRL;
    
    /* Отключаем MPU для реконфигурации */
    MPU->CTRL = 0;
    
    /* Регион 4: SDRAM W9825G6KH (32MB @ 0xC0000000) */
    MPU->RNR = 4;
    MPU->RBAR = 0xC0000000UL;  /* FMC Bank 1 для SDRAM */
    MPU->RASR = (0UL << MPU_RASR_XN_Pos) |      /* Разрешить исполнение */
                (0x3UL << MPU_RASR_AP_Pos) |    /* Full access */
                (0x1UL << MPU_RASR_TEX_Pos) |   /* Normal memory */
                (1UL << MPU_RASR_S_Pos) |       /* Shareable (DMA!) */
                (0UL << MPU_RASR_C_Pos) |       /* Not Cacheable (изначально) */
                (1UL << MPU_RASR_B_Pos) |       /* Bufferable */
                ((25UL - 1UL) << MPU_RASR_SIZE_Pos) | /* 32MB (2^25) */
                (1UL << MPU_RASR_ENABLE_Pos);
    
    /* Регион 5: QSPI Flash W25Q128JV (16MB @ 0x90000000) */
    MPU->RNR = 5;
    MPU->RBAR = QSPI_BASE;  /* 0x90000000 */
    MPU->RASR = (0UL << MPU_RASR_XN_Pos) |      /* Разрешить исполнение (XIP) */
                (0x1UL << MPU_RASR_AP_Pos) |    /* Read only (Flash) */
                (0x0UL << MPU_RASR_TEX_Pos) |   /* Device memory */
                (0UL << MPU_RASR_S_Pos) |       /* Not Shareable */
                (0UL << MPU_RASR_C_Pos) |       /* Not Cacheable (изначально) */
                (0UL << MPU_RASR_B_Pos) |       /* Not Bufferable */
                ((24UL - 1UL) << MPU_RASR_SIZE_Pos) | /* 16MB (2^24) */
                (1UL << MPU_RASR_ENABLE_Pos);
    
    /* Включаем MPU обратно */
    MPU->CTRL = mpu_ctrl;
    
    /* Барьеры памяти */
    __DSB();
    __ISB();
    
    USART1_SendString("OK\n\r");
}
/* USER CODE END 0 */

/* USER CODE 1 */
void MPU_Enable_SDRAM_Cache(void)
{
    USART1_SendString("Enable SDRAM Cache...");
    
    /* Временно отключаем MPU */
    uint32_t mpu_ctrl = MPU->CTRL;
    MPU->CTRL = 0;
    
    /* Модифицируем регион SDRAM для включения кэширования */
    MPU->RNR = 4;
    
    uint32_t rasr = MPU->RASR;
    rasr &= ~(1UL << MPU_RASR_C_Pos);  /* Очищаем C бит */
    rasr |= (1UL << MPU_RASR_C_Pos);   /* Устанавливаем Cacheable */
    MPU->RASR = rasr;
    
    /* Включаем MPU обратно */
    MPU->CTRL = mpu_ctrl;
    
    /* Сбрасываем кэш данных для SDRAM региона */
    SCB_InvalidateDCache_by_Addr((uint32_t*)0xC0000000, 32 * 1024 * 1024);
    
    __DSB();
    __ISB();
    
    USART1_SendString("OK\n\r");
}

/* USER CODE END 1 */

/* USER CODE 2 */
void MPU_Enable_QSPI_Cache(void)
{
    USART1_SendString("Enable QSPI Cache...");
    
    /* Временно отключаем MPU */
    uint32_t mpu_ctrl = MPU->CTRL;
    MPU->CTRL = 0;
    
    /* Модифицируем регион QSPI для включения кэширования */
    MPU->RNR = 5;
    
    uint32_t rasr = MPU->RASR;
    rasr &= ~(1UL << MPU_RASR_C_Pos);
    rasr |= (1UL << MPU_RASR_C_Pos);
    MPU->RASR = rasr;
    
    /* Включаем MPU обратно */
    MPU->CTRL = mpu_ctrl;
    
    /* Сбрасываем кэш инструкций для QSPI региона */
    SCB_InvalidateICache();
    
    __DSB();
    __ISB();
    
    USART1_SendString("OK\n\r");
}
/* USER CODE END 2 */

/* USER CODE 3 */

/* USER CODE END 3 */

/* USER CODE 4 */

/* USER CODE END 4 */

/* mpu.c END */
