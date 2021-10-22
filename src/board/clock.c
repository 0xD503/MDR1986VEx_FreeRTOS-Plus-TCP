#include "clock.h"

#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_eth.h"
#include "MDR32F9Qx_eeprom.h"
#include "MDR32F9Qx_power.h"
#include "MDR32F9Qx_timer.h"

#include "FreeRTOS.h"




void System_ClockInit (void)
{
    RST_CLK_HSEconfig(RST_CLK_HSE_ON);          /// set up HSE (8 MHz quarz rezonator)
    while (RST_CLK_HSEstatus() != SUCCESS)  { } /// wait for HSE readiness

    if (RST_CLK_HSEstatus() == SUCCESS)
    {
        /// select HSE clock as CPU_PLL input clock source. Set PLL multiplier to 5 (40 MHz)
        RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul5);
        RST_CLK_CPU_PLLcmd(ENABLE);                     /// enable PLL
        while (RST_CLK_CPU_PLLstatus() != SUCCESS)  { } /// wait fo PLL readiness

        if (RST_CLK_CPU_PLLstatus() == SUCCESS)
        {
            /// set delay after branch during program execution before increase clock frequency
            /// Enables the EEPROM and Battery clocks
            RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM | RST_CLK_PCLK_BKP, ENABLE);

            EEPROM_SetLatency(EEPROM_Latency_1);    /// latency = 1 - MCU clk up to the 50 MHz
            POWER_DUccMode(POWER_DUcc_upto_80MHz);  /// Configures the LDO

            /// disable clocks to save power
            RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM | RST_CLK_PCLK_BKP, DISABLE);

            /// don't care about CPU_C1
            /// set CPU_C2_SEL to CPU_PLL output instead of CPU_C1 clock
            RST_CLK_CPU_PLLuse(ENABLE);
            /// set CPU_C3_prescaler to 1
            RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);
            /// set core clock (HCLK) to CPU_C3 (which in its turn was set to PLL_CPU / 1)
            RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);

            /// update CMSIS variable for correct FreeRTOS work
            SystemCoreClockUpdate();//SystemCoreClock = 8000000;//SystemCoreClock = 5333333;
        }   else    {
            /// PLL error handling
        }
    }   else    {
        /// HSE setting error handling
    }


    //
}


void Ports_ClockInit (void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE);
}

void Ethernet_ClockInit (void)
{
    ETH_ClockDeInit();

    /// set up HSE2 (25 MHz quarz rezonator)
    RST_CLK_HSE2config(RST_CLK_HSE2_ON);
    while (RST_CLK_HSE2status() != SUCCESS) { } /// wait for HSE2 readiness

    if (RST_CLK_HSE2status() == SUCCESS)
    {
        RST_CLK_PCLKcmd(RST_CLK_PCLK_DMA, ENABLE);  /// enable DMA clock

        /// initialize ETH PHY clock
        ETH_PHY_ClockConfig(ETH_PHY_CLOCK_SOURCE_HSE2, ETH_PHY_HCLKdiv1);
        ETH_BRGInit(ETH_HCLKdiv1);      /// set clock divider (HSE2 / 1)
        ETH_ClockCMD(ETH_CLK1, ENABLE); /// enable clock for ETH1
    }   else    {
        /// HSE2 error handling
    }
}



/*
 *  void vPortSetupTimerInterrupt (void) port
 *
 *  RTOS timer IRQ init
*/
void vPortSetupTimerInterrupt (void)
{
    TIMER_CntInitTypeDef countConfig;

    /* Configure Timer4 Peripheral Clock and Divider. */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER4, ENABLE);
    TIMER_BRGInit(MDR_TIMER4, TIMER_HCLKdiv1);

    TIMER_CntStructInit(&countConfig);
    countConfig.TIMER_Prescaler = TIMER_PRESCALER_1;
    countConfig.TIMER_Period = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
    countConfig.TIMER_IniCounter = countConfig.TIMER_Period;
    countConfig.TIMER_CounterDirection = TIMER_CntDir_Dn;

    TIMER_CntInit(MDR_TIMER4, &countConfig);

    TIMER_ITConfig(MDR_TIMER4, TIMER_IT_CNT_ZERO_EVENT, ENABLE);
    NVIC_EnableIRQ(TIMER4_IRQn);

    TIMER_Cmd(MDR_TIMER4, ENABLE);

/*	MDR_TIMER4->CNTRL = 0x00000000; /// reset timer

	MDR_TIMER4->IE = TIMER_IE_CNT_ZERO_EVENT_IE;
	MDR_TIMER4->ARR = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;

	MDR_TIMER4->CNTRL = TIMER_CNTRL_DIR | TIMER_CNTRL_CNT_EN;   /// enable down count*/
}
