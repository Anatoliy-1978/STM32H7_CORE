/*
 * rcc.c    Board STM32H7_CORE
 * 08.01.2026
 */
 
/* Includes */
#include "rcc.h"
#include <stdbool.h>
#include "usart1.h"
/* Function prototypes */
 
/* Function prototypes END */

/* USER CODE 0 */
/**
  * @brief  System Clock Initialization for STM32H743IIT6
  * @note   Конфигурация:
  *         - CPU: 400 MHz (VOS1, PLL1P)
  *         - HCLK: 200 MHz (AHB)
  *         - PCLK1/2/3/4: 100 MHz (APB)
  *         - FMC: 160 MHz (PLL1Q)
  *         - QUADSPI: 100 MHz (PLL2_R)
  *         - USART1: 50 MHz (PCLK2 с делителем 2)
  *         - LTDC: 25 MHz (PLL3R с делителем 5)
  *         - HSE: 25 MHz внешний кварц
  * @param  None
  * @retval None
  */
void System_Clock_Init(void)
{
    /* ==================== РАЗДЕЛ 1: НАСТРОЙКА ПИТАНИЯ ==================== */
    
    /* Шаг 1.1: Конфигурация внутреннего LDO-стабилизатора
     * PWR_CR3 регистр:
     * Примечание: После сброса,POR PWR->CR3 = 0x00000046 (LDOEN=1, SCUEN=1)
     * Очищаем LDOEN=0, BYPASS=0, 
     * - LDOEN=1: Включаем внутренний LDO стабилизатор
     * - BYPASS=0: Режим работы через стабилизатор (не bypass)
     * - SCUEN: Автоматически сбрасывается при первой записи в PWR_CR3
     */
    USART1_SendString("1. PWR Config...");
    PWR->CR3 = (PWR->CR3 & ~(PWR_CR3_LDOEN | PWR_CR3_BYPASS)) | PWR_CR3_LDOEN;
    USART1_SendString("OK\n\r");
    
    /* Шаг 1.2: Ожидание стабилизации питания (Run* > Run mode)
     * PWR_CSR1 регистр, бит 13 (ACTVOSRDY):
     * - 0: Напряжение нестабильно, система в Run* режиме
     * - 1: Напряжение стабильно, система в нормальном Run режиме
     * Важно: Пока ACTVOSRDY=0, запись в RAM запрещена!
     */
    USART1_SendString("2. Wait ACTVOSRDY...");
    while (!(PWR->CSR1 & PWR_CSR1_ACTVOSRDY)) {
        // Пустой цикл ожидания, можно добавить таймаут
    }
    USART1_SendString("OK\n\r");
    
    /* Шаг 1.3: Установка VOS1 (масштаб напряжения 1) для 400 МГц
     * PWR_D3CR регистр, биты 15:14 (VOS[1:0]):
     * - 00: Зарезервировано
     * - 01: VOS3 (масштаб 3) - по умолчанию после сброса
     * - 10: VOS2 (масштаб 2)
     * - 11: VOS1 (масштаб 1) - наш выбор для 400 МГц
     * Примечание: Для частот выше 400 МГц нужен VOS0 через SYSCFG->PWRCR
     */
    USART1_SendString("3. Set VOS1...");
    MODIFY_REG(PWR->D3CR, 
               PWR_D3CR_VOS_Msk, 
               PWR_D3CR_VOS_0 | PWR_D3CR_VOS_1); // 0b11 = VOS1
    USART1_SendString("OK\n\r");
    
    /* Шаг 1.4: Ожидание готовности VOS1
     * PWR_D3CR регистр, бит 13 (VOSRDY):
     * - 0: Напряжение Vcore ниже требуемого уровня для VOS1
     * - 1: Напряжение Vcore достигло уровня для VOS1
     * Критически важно ждать этого флага перед увеличением частоты!
     */
    USART1_SendString("4. Wait VOSRDY...");
    while (!(PWR->D3CR & PWR_D3CR_VOSRDY)) {
        // Критическое ожидание - без этого увеличение частоты опасно!
    }
    USART1_SendString("OK\n\r");
    
    MPU_Config_Internal();
    
    /* ==================== РАЗДЕЛ 2: НАСТРОЙКА FLASH ==================== */
    
    /* Шаг 2.1: Настройка латентности Flash-памяти
     * FLASH_ACR регистр:
     * - LATENCY[2:0] (биты 2:0): Количество циклов ожидания (Wait States)
     * - WRHIGHFREQ[1:0] (биты 5:4): Режим записи для высоких частот
     * Согласно таблице 17 документации Flash:
     * Для VOS1 и частоты HCLK=200 МГц (диапазон ]185 МГц; 210 МГц])
     * требуется LATENCY = 2 WS (3 FLASH clock cycles)
     */
    USART1_SendString("5. FLASH ACR...");
    MODIFY_REG(FLASH->ACR,
               FLASH_ACR_LATENCY_Msk | FLASH_ACR_WRHIGHFREQ_Msk,
               FLASH_ACR_LATENCY_2WS |      // 2 Wait States для 200 МГц при VOS1
               FLASH_ACR_WRHIGHFREQ_1);     // Режим высокой скорости записи
    USART1_SendString("OK\n\r");
    
    
     MPU_Config_Flash();
    
    /* ==================== РАЗДЕЛ 3: ВНЕШНИЙ ГЕНЕРАТОР HSE ==================== */
    
    /* Шаг 3.1: Включение внешнего кварцевого генератора 25 МГц
     * RCC_CR регистр, бит 16 (HSEON):
     * - 0: HSE выключен
     * - 1: HSE включен
     * Примечание: На этом этапе система работает на HSI 64 МГц
     */
    USART1_SendString("6. HSE ON...");
    SET_BIT(RCC->CR, RCC_CR_HSEON);
    USART1_SendString("OK\n\r");
    
    /* Шаг 3.2: Ожидание стабилизации HSE
     * RCC_CR регистр, бит 17 (HSERDY):
     * - 0: HSE нестабилен или выключен
     * - 1: HSE стабилен и готов к использованию
     * Таймаут добавлен для защиты от зависания при отсутствии кварца
     */
    USART1_SendString("7. HSE RDY...");
    volatile uint32_t hse_timeout = 0;
    #define HSE_TIMEOUT_MAX 10000000  // Эмпирическое значение для 64 МГц
    
    while (!READ_BIT(RCC->CR, RCC_CR_HSERDY)) {
        hse_timeout++;
        if (hse_timeout > HSE_TIMEOUT_MAX) {
            USART1_SendString("TIMEOUT! HSE not responding\n\r");
            while(1);  // Аварийная остановка
        }
    }
    USART1_SendString("OK\n\r");
    
    /* ==================== РАЗДЕЛ 4: НАСТРОЙКА ДЕЛИТЕЛЕЙ ЧАСТОТЫ ==================== */
    
    /* Шаг 4.1: Настройка делителей частоты для доменов D1, D2, D3
     * Все делители настраиваются ДО включения PLL!
     */
  
    USART1_SendString("8. Clock Dividers...");
    
    /* Домен D1: CPU, Flash, AXI, AHB1-3, APB3
     * RCC_D1CFGR регистр:
     * - HPRE[3:0] (биты 7:4): Делитель HCLK (AHB шина)
     *   DIV2: 400 МГц / 2 = 200 МГц
     * - D1PPRE[2:0] (биты 10:8): Делитель PCLK3 (APB3)
     *   DIV2: 200 МГц / 2 = 100 МГц
     * - D1CPRE[3:0] (биты 3:0): Делитель Cortex System Timer
     *   DIV1: 400 МГц (без деления)
     */
    MODIFY_REG(RCC->D1CFGR,
               RCC_D1CFGR_HPRE_Msk | RCC_D1CFGR_D1PPRE_Msk | RCC_D1CFGR_D1CPRE_Msk,
               RCC_D1CFGR_HPRE_DIV2 |    // HCLK = 200 МГц
               RCC_D1CFGR_D1PPRE_DIV2 |  // PCLK3 = 100 МГц
               RCC_D1CFGR_D1CPRE_DIV1);  // Cortex = 400 МГц
    
    /* Домен D2: APB1, APB2, AHB2
     * RCC_D2CFGR регистр:
     * - D2PPRE2[2:0] (биты 13:11): Делитель PCLK2 (APB2)
     *   DIV4: 200 МГц / 4 = 50 МГц (для USART1, SPI, I2C)
     * - D2PPRE1[2:0] (биты 10:8): Делитель PCLK1 (APB1)
     *   DIV2: 200 МГц / 4 = 50 МГц (для I2C4, UART)
     */
    MODIFY_REG(RCC->D2CFGR,
               RCC_D2CFGR_D2PPRE2_Msk | RCC_D2CFGR_D2PPRE1_Msk,
               RCC_D2CFGR_D2PPRE2_DIV4 |  // PCLK2 = 50 МГц
               RCC_D2CFGR_D2PPRE1_DIV4);  // PCLK1 = 50 МГц
    
    /* Домен D3: APB4, AHB4
     * RCC_D3CFGR регистр:
     * - D3PPRE[2:0] (биты 10:8): Делитель PCLK4 (APB4)
     *   DIV2: 200 МГц / 2 = 100 МГц
     */
    MODIFY_REG(RCC->D3CFGR,
               RCC_D3CFGR_D3PPRE_Msk,
               RCC_D3CFGR_D3PPRE_DIV16);  // PCLK4 = 100 МГц
               
    USART1->CR1 &= ~USART_CR1_UE;
    while (USART1->CR1 & USART_CR1_UE);
    RCC->D2CCIP2R = (3 << RCC_D2CCIP2R_USART16SEL_Pos);
    USART1->CR1 |= USART_CR1_UE;
    
    USART1_SendString("OK\n\r");
    
    /* ==================== РАЗДЕЛ 5: НАСТРОЙКА ИСТОЧНИКОВ PLL ==================== */
    
    /* Шаг 5.1: Выбор источника и делителя для PLL1
     * RCC_PLLCKSELR регистр:
     * - PLLSRC[1:0] (биты 1:0): Источник для PLL1
     *   10: HSE (наш выбор)
     * - DIVM1[5:0] (биты 13:8): Делитель входа PLL1 (M коэффициент)
     *   5: 25 МГц / 5 = 5 МГц (вход PLL)
     */
    USART1_SendString("9. PLL Source...");
    MODIFY_REG(RCC->PLLCKSELR,
               RCC_PLLCKSELR_PLLSRC_Msk |           // Общий источник для PLL
               RCC_PLLCKSELR_DIVM1_Msk |            // Делитель для PLL1
               RCC_PLLCKSELR_DIVM2_Msk |            // Делитель для PLL2 
               RCC_PLLCKSELR_DIVM3_Msk,             // Делитель для PLL3
               RCC_PLLCKSELR_PLLSRC_HSE |           // Источник: HSE 25 МГц
               (5U << RCC_PLLCKSELR_DIVM1_Pos) |    // DIVM1 = 5 (для PLL1)
               (5U << RCC_PLLCKSELR_DIVM2_Pos) |    // DIVM2 = 5 (для PLL2)
               (5U << RCC_PLLCKSELR_DIVM3_Pos));    // DIVM3 = 5 (для PLL3)
    USART1_SendString("OK\n\r");
    
    /* ==================== РАЗДЕЛ 6: НАСТРОЙКА PLL1 (400 МГц CPU) ==================== */
    
    /* Шаг 6.1: Отключение PLL1 перед настройкой */
    USART1_SendString("10. PLL1 Config...");
    CLEAR_BIT(RCC->CR, RCC_CR_PLL1ON);
    
    /* Шаг 6.2: Ожидание полного отключения PLL1 */
    while (READ_BIT(RCC->CR, RCC_CR_PLL1RDY)) {
        // Ожидание, пока PLL1 полностью остановится
    }
    
    /* Шаг 6.3: Настройка параметров PLL1 в RCC_PLLCFGR
     * RCC_PLLCFGR регистр для PLL1:
     * - PLL1RGE[1:0] (биты 3:2): Диапазон входной частоты PLL1
     *   10 (RGE_2): 4-8 МГц (наш вход 5 МГц попадает в этот диапазон)
     * - PLL1VCOSEL (бит 1): Диапазон VCO
     *   1: Широкий диапазон (Wide VCO)
     * - DIVP1EN (бит 16): Включение выхода P (для CPU)
     * - DIVQ1EN (бит 17): Включение выхода Q (для FMC)
     * - DIVR1EN (бит 18): Включение выхода R (резерв)
     */
    MODIFY_REG(RCC->PLLCFGR,
               RCC_PLLCFGR_PLL1RGE_Msk | RCC_PLLCFGR_PLL1VCOSEL_Msk |
               RCC_PLLCFGR_DIVP1EN_Msk | RCC_PLLCFGR_DIVQ1EN_Msk | RCC_PLLCFGR_DIVR1EN_Msk,
               RCC_PLLCFGR_PLL1RGE_2 |              // Диапазон входа 4-8 МГц
              (0 << RCC_PLLCFGR_PLL1VCOSEL_Pos) |   // Широкий диапазон VCO  PLL1VCOSEL = 0 (WIDE: 192-960 MHz)
               RCC_PLLCFGR_DIVP1EN |                // Включаем выход P
               RCC_PLLCFGR_DIVQ1EN |                // Включаем выход Q
               RCC_PLLCFGR_DIVR1EN);                // Включаем выход R
    
    /* Шаг 6.4: Настройка делителей PLL1
     * RCC_PLL1DIVR регистр:
     * - N1[8:0] (биты 8:0): Коэффициент умножения (N)
     *   160: 5 МГц ? 160 = 800 МГц (частота VCO)
     * - P1[6:0] (биты 16:10): Делитель выхода P
     *   2: 800 МГц / 2 = 400 МГц (частота CPU)
     * - Q1[6:0] (биты 24:18): Делитель выхода Q
     *   5: 800 МГц / 4 = 200 МГц (для FMC)
     * - R1[6:0] (биты 32:26): Делитель выхода R
     *   1: 800 МГц / 1 = 800 МГц (резерв)
     */
    RCC->PLL1DIVR = (160U << RCC_PLL1DIVR_N1_Pos) |  // N = 160
                    (1U << RCC_PLL1DIVR_P1_Pos)   |  // P = 2
                    (4U << RCC_PLL1DIVR_Q1_Pos)   |  // Q = 4
                    (1U << RCC_PLL1DIVR_R1_Pos);     // R = 1
    
    /* Шаг 6.5: Включение PLL1 */
    SET_BIT(RCC->CR, RCC_CR_PLL1ON);
    USART1_SendString("OK\n\r");
    
    /* Шаг 6.6: Ожидание готовности PLL1 */
    USART1_SendString("11. PLL1 RDY...");
    while (!READ_BIT(RCC->CR, RCC_CR_PLL1RDY)) {
        // Ожидание блокировки PLL (lock)
    }
    USART1_SendString("OK\n\r");
    
    /* ==================== РАЗДЕЛ 7: НАСТРОЙКА PLL2 (100 МГц QUADSPI) ==================== */
    
    /* Шаг 7.1: Настройка PLL2 */
    USART1_SendString("12. PLL2 Config...");
    CLEAR_BIT(RCC->CR, RCC_CR_PLL2ON);
    while (READ_BIT(RCC->CR, RCC_CR_PLL2RDY)) {}
    
    /* Шаг 7.3: Настройка PLL2 в RCC_PLLCFGR */
    MODIFY_REG(RCC->PLLCFGR,
               RCC_PLLCFGR_PLL2RGE_Msk | RCC_PLLCFGR_PLL2VCOSEL_Msk | RCC_PLLCFGR_DIVR2EN_Msk,
               RCC_PLLCFGR_PLL2RGE_2 |       // Диапазон 4-8 МГц
               RCC_PLLCFGR_PLL2VCOSEL |      // Medium VCO (150-420 МГц)
               RCC_PLLCFGR_DIVR2EN);         // Включаем выход R
    
    /* Шаг 7.4: Делители PLL2
     * RCC_PLL2DIVR регистр:
     * - N2=80: 5 МГц ? 80 = 400 МГц (VCO)
     * - R2=4: 400 МГц / 4 = 100 МГц (для QUADSPI)
     */
    RCC->PLL2DIVR = (80U << RCC_PLL2DIVR_N2_Pos) |  // N = 80
                    (4U << RCC_PLL2DIVR_R2_Pos);    // R = 4
    
    /* Шаг 7.5: Включение и ожидание PLL2 */
    SET_BIT(RCC->CR, RCC_CR_PLL2ON);
    while (!READ_BIT(RCC->CR, RCC_CR_PLL2RDY)) {}
    USART1_SendString("OK\n\r");
    
    /* ==================== РАЗДЕЛ 8: НАСТРОЙКА PLL3 (125 МГц LTDC) ==================== */
    
    /* Шаг 8.1: Настройка PLL3 для LTDC */
    USART1_SendString("13. PLL3 Config...");
    CLEAR_BIT(RCC->CR, RCC_CR_PLL3ON);
    while (READ_BIT(RCC->CR, RCC_CR_PLL3RDY)) {}
    
    /* Шаг 8.3: Настройка PLL3 в RCC_PLLCFGR */
    MODIFY_REG(RCC->PLLCFGR,
               RCC_PLLCFGR_PLL3RGE_Msk | RCC_PLLCFGR_PLL3VCOSEL_Msk | RCC_PLLCFGR_DIVR3EN_Msk,
               RCC_PLLCFGR_PLL3RGE_2 |       // Диапазон 4-8 МГц
               RCC_PLLCFGR_PLL3VCOSEL |      // Medium VCO (150-420 МГц)
               RCC_PLLCFGR_DIVR3EN);         // Включаем выход R
    
    /* Шаг 8.4: Делители PLL3
     * RCC_PLL3DIVR регистр:
     * - N3=50: 5 МГц ? 50 = 250 МГц (VCO)
     * - R3=2: 250 МГц / 2 = 125 МГц
     * Примечание: Для LTDC 25 МГц нужен дополнительный делитель 5 в LTDC
     */
    RCC->PLL3DIVR = (60U << RCC_PLL3DIVR_N3_Pos) |   // N = 60
                    (12U << RCC_PLL3DIVR_R3_Pos);    // R = 12
    
    /* Шаг 8.5: Включение и ожидание PLL3 */
    SET_BIT(RCC->CR, RCC_CR_PLL3ON);
    while (!READ_BIT(RCC->CR, RCC_CR_PLL3RDY)) {}
    USART1_SendString("OK\n\r");
    
    /* ==================== РАЗДЕЛ 9: ПЕРЕКЛЮЧЕНИЕ СИСТЕМНОЙ ЧАСТОТЫ ==================== */
    
    /* Шаг 9.1: Переключение системной частоты на PLL1
     * RCC_CFGR регистр, биты 1:0 (SW[1:0]):
     * - 00: HSI как системная частота (по умолчанию)
     * - 01: CSI как системная частота
     * - 10: HSE как системная частота
     * - 11: PLL1 как системная частота (наш выбор)
     */
    USART1_SendString("14. Switch to PLL1...");
    MODIFY_REG(RCC->CFGR,
               RCC_CFGR_SW_Msk,
               RCC_CFGR_SW_PLL1);  // Выбираем PLL1 как источник
    
    /* Шаг 9.2: Ожидание переключения
     * RCC_CFGR регистр, биты 3:2 (SWS[1:0]):
     * Отражают текущий источник системной частоты
     */
    volatile uint32_t switch_timeout = 1000000;
    while (((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL1) &&
           (switch_timeout-- > 0)) {
        // Ожидание завершения переключения
    }
    
    if (switch_timeout == 0) {
        USART1_SendString("ERROR: PLL Switch Timeout\n\r");
        while(1);
    }
    USART1_SendString("OK\n\r");
    
    /* ==================== РАЗДЕЛ 10: НАСТРОЙКА ИСТОЧНИКОВ ПЕРИФЕРИИ ==================== */
    
    /* Шаг 10.1: Настройка источников тактирования для периферии */
    USART1_SendString("15. Periph Clock Sel...");
    
    /* FMC: тактирование от PLL1Q (160 МГц)
     * RCC_D1CCIPR регистр, биты 1:0 (FMCSEL[1:0]):
     * - 00: rcc_hclk3 (по умолчанию)
     * - 01: pll1_q_ck (наш выбор, 160 МГц)
     * - 10: pll2_r_ck
     * - 11: per_ck
     */
    MODIFY_REG(RCC->D1CCIPR,
               RCC_D1CCIPR_FMCSEL_Msk,
               RCC_D1CCIPR_FMCSEL_0);  // 01: PLL1Q
    
    /* QUADSPI: тактирование от PLL2_R (100 МГц)
     * RCC_D1CCIPR регистр, биты 5:4 (QSPISEL[1:0]):
     * - 00: rcc_hclk3 (по умолчанию)
     * - 01: pll1_q_ck
     * - 10: pll2_r_ck (наш выбор, 100 МГц)
     * - 11: per_ck
     */
    MODIFY_REG(RCC->D1CCIPR,
               RCC_D1CCIPR_QSPISEL_Msk,
               RCC_D1CCIPR_QSPISEL_1);  // 10: PLL2_R
    
    /* I2C4: тактирование от PCLK4 (100 МГц) - уже по умолчанию
     * RCC_D3CCIPR регистр, биты 9:8 (I2C4SEL[1:0]):
     * - 00: rcc_pclk4 (по умолчанию, наш выбор)
     * - 01: pll3_r_ck
     * - 10: hsi_ker_ck
     * - 11: csi_ker_ck
     */
    MODIFY_REG(RCC->D3CCIPR, RCC_D3CCIPR_I2C4SEL_Msk, 0x00); // Уже по умолчанию
    
    /* USART1: тактирование от PCLK2 (100 МГц)
     * RCC_D2CCIP2R регистр, биты 5:3 (USART16SEL[2:0]):
     * - 000: rcc_pclk2 (по умолчанию, наш выбор)
     * - 001: pll2_q_ck
     * - 010: pll3_q_ck
     * - 011: hsi_ker_ck
     * - 100: csi_ker_ck
     * - 101: lse_ck
     * Примечание: Для получения 50 МГц нужно настроить USART1->BRR на делитель 2
     */
    USART1->CR1 &= ~USART_CR1_UE;
    while (USART1->CR1 & USART_CR1_UE);
    RCC->D2CCIP2R = (0 << RCC_D2CCIP2R_USART16SEL_Pos);
    
    /* Устанавливаем BRR для 50 МГц и 115200 бод
     * Расчет: USARTDIV = 50,000,000 / (16 * 115200) = 27.12673611111111
     * Mantissa = 27(0x1B), Fraction = 2 (0,12673611111111 * 16 = 2,027777 ~ 2)
     * BRR = (0x1B << 4) | 2 = 0x1B2
     */
    
    USART1->BRR = (0x1B << USART_BRR_DIV_MANTISSA_Pos) |  // DIV_Mantissa = 27
                  (0x2 << USART_BRR_DIV_FRACTION_Pos);    // DIV_Fraction = 2
    USART1->CR1 |= USART_CR1_UE; 
    while (!(USART1->CR1 & USART_CR1_UE));
    USART1_SendString("OK\n\r");
    
    /* ==================== РАЗДЕЛ 11: ФИНАЛИЗАЦИЯ ==================== */
    
    /* Шаг 11.1: Обновление системной переменной частоты
     * SystemCoreClock используется библиотеками CMSIS для SysTick и других функций
     */
    SystemCoreClock = 400000000;  // 400 МГц
    
    /* Шаг 11.2: Обновлённый отчёт с правильными частотами */
    USART1_SendString("========================================\n\r");
    USART1_SendString("=== FINAL Clock Configuration ===\n\r");
    USART1_SendString("========================================\n\r");
    USART1_SendString("- CPU Core:       400 MHz (PLL1P, VOS1)\n\r");
    USART1_SendString("- AHB Bus (HCLK): 200 MHz\n\r");
    USART1_SendString("- APB1 (PCLK1):    50 MHz (I2C4, UART) /4\n\r");
    USART1_SendString("- APB2 (PCLK2):    50 MHz (USART1) /4\n\r");
    USART1_SendString("- APB3 (PCLK3):   100 MHz /2\n\r");
    USART1_SendString("- APB4 (PCLK4):   100 MHz /2\n\r");
    USART1_SendString("- FMC:            160 MHz (PLL1Q)\n\r");
    USART1_SendString("- QUADSPI:        100 MHz (PLL2_R)\n\r");
    USART1_SendString("- I2C4:            50 MHz (PCLK1)\n\r");
    USART1_SendString("- USART1:      115200 baud (PCLK2=50 MHz)\n\r");
    USART1_SendString("- LTDC:            25 MHz (PLL3R)\n\r");
    USART1_SendString("========================================\n\r");
}

/* USER CODE END 0 */

/* USER CODE 2 */
    /* Приватная функция настройки MPU для внутренней памяти */
void MPU_Config_Internal(void)
{
    USART1_SendString("MPU Phase 1: Internal Memory...");
    
    /* 1. Отключаем MPU */
    MPU->CTRL = 0;
    
    /* 2. Регион 0: ITCM RAM (64KB) - инструкции */
    MPU->RNR = 0;
    MPU->RBAR = D1_ITCMRAM_BASE;  /* 0x00000000 */
    MPU->RASR = (0UL << MPU_RASR_XN_Pos) |      /* Разрешить исполнение */
                (0x3UL << MPU_RASR_AP_Pos) |    /* Privileged: RW, User: RW */
                (0x1UL << MPU_RASR_TEX_Pos) |   /* TEX=001 (Normal) */
                (1UL << MPU_RASR_S_Pos) |       /* Shareable */
                (1UL << MPU_RASR_C_Pos) |       /* Cacheable */
                (0UL << MPU_RASR_B_Pos) |       /* Not Bufferable */
                (0x15UL << MPU_RASR_SIZE_Pos) | /* 64KB (2^(15+1)) */
                (1UL << MPU_RASR_ENABLE_Pos);   /* Enable region */
    
    /* 3. Регион 1: DTCM RAM (128KB) - данные и стек */
    MPU->RNR = 1;
    MPU->RBAR = D1_DTCMRAM_BASE;  /* 0x20000000 */
    MPU->RASR = (0UL << MPU_RASR_XN_Pos) |
                (0x3UL << MPU_RASR_AP_Pos) |
                (0x1UL << MPU_RASR_TEX_Pos) |
                (1UL << MPU_RASR_S_Pos) |
                (1UL << MPU_RASR_C_Pos) |
                (0UL << MPU_RASR_B_Pos) |
                (0x10UL << MPU_RASR_SIZE_Pos) | /* 128KB (2^(16+1)) */
                (1UL << MPU_RASR_ENABLE_Pos);
    
    /* 4. Включаем MPU с привилегированным доступом по умолчанию */
    MPU->CTRL = MPU_CTRL_ENABLE_Msk | MPU_CTRL_PRIVDEFENA_Msk;
    
    /* 5. Барьеры памяти (критически важно!) */
    __DSB();
    __ISB();
    
    USART1_SendString("OK\n\r");
}
/* USER CODE END 2 */

/* USER CODE 3 */
/* Приватная функция настройки MPU для Flash памяти */
void MPU_Config_Flash(void)
{
    USART1_SendString("MPU Phase 1: Flash Memory...");
    
    /* Временно отключаем MPU */
    uint32_t mpu_ctrl = MPU->CTRL;
    MPU->CTRL = 0;
    
    /* 6. Регион 2: Flash Bank 1 (1MB) */
    MPU->RNR = 2;
    MPU->RBAR = FLASH_BANK1_BASE;  /* 0x08000000 */
    MPU->RASR = (0UL << MPU_RASR_XN_Pos) |      /* Разрешить исполнение кода */
                (0x1UL << MPU_RASR_AP_Pos) |    /* Privileged: RO, User: RO */
                (0x0UL << MPU_RASR_TEX_Pos) |   /* TEX=000 (Device-like) */
                (0UL << MPU_RASR_S_Pos) |       /* Not Shareable */
                (1UL << MPU_RASR_C_Pos) |       /* Cacheable (инструкции) */
                (0UL << MPU_RASR_B_Pos) |       /* Not Bufferable */
                (0x13UL << MPU_RASR_SIZE_Pos) | /* 1MB (2^(19+1)) */
                (1UL << MPU_RASR_ENABLE_Pos);
    
    /* 7. Регион 3: Flash Bank 2 (1MB) */
    MPU->RNR = 3;
    MPU->RBAR = FLASH_BANK2_BASE;  /* 0x08100000 */
    MPU->RASR = (0UL << MPU_RASR_XN_Pos) |
                (0x1UL << MPU_RASR_AP_Pos) |
                (0x0UL << MPU_RASR_TEX_Pos) |
                (0UL << MPU_RASR_S_Pos) |
                (1UL << MPU_RASR_C_Pos) |
                (0UL << MPU_RASR_B_Pos) |
                (0x13UL << MPU_RASR_SIZE_Pos) | /* 1MB */
                (1UL << MPU_RASR_ENABLE_Pos);
    
    /* Включаем MPU обратно */
    MPU->CTRL = mpu_ctrl;
    
    __DSB();
    __ISB();
    
    USART1_SendString("OK\n\r");
}

/* USER CODE END 3 */

/* USER CODE 4 */

/* USER CODE END 4 */

/* rcc.c END */
