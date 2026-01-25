/*
 * main.h   Board STM32H7_CORE
 * 03.01.2026
 */
#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx.h"

#define FMC_KER_CLK_HZ      200000000UL
#define SDRAM_CLK_HZ        (FMC_KER_CLK_HZ / 2U)  // 100 МГц
#define SDRAM_BASE          0xC0000000UL
#define SDRAM_SIZE          (32U * 1024U * 1024U)

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__

