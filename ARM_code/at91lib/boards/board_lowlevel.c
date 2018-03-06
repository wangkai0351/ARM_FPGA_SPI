/*
*********************************************************************************************************
*         dw Project       
*
* (c) Copyright 2008, Atmel Corporation
*     All Rights Reserved
*
*Hardware platform : CM-TAS28 , ATSAM9XE512
*
* File             : emac.c
* version          : v1.0
* By               : Atmel Corporation
* Modify         : lp
* Date             : 
*********************************************************************************************************
*/

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "board.h"
#include "board_memories.h"
#include <pmc/pmc.h>
#include <memories/flash/flashd.h>

//------------------------------------------------------------------------------
//         Local definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "SAM9XE - Oscillator & PLL Parameters"
/// This page lists the parameters which are set for the PLL and main
/// oscillator configuration.
///
/// !Parameters
/// - BOARD_OSCOUNT
/// - BOARD_CKGR_PLLA
/// - BOARD_PLLACOUNT
/// - BOARD_MULA
/// - BOARD_DIVA
/// - BOARD_CKGR_PLLB
/// - BOARD_PLLBCOUNT
/// - BOARD_MULB
/// - BOARD_DIVB
/// - BOARD_USBDIV
/// - BOARD_PRESCALER

/// Main oscillator startup time (in number of slow clock ticks). 
#define BOARD_OSCOUNT           (AT91C_CKGR_OSCOUNT & (64 << 8))

/// PLLA frequency range.
#define BOARD_CKGR_PLLA         (AT91C_CKGR_SRCA | AT91C_CKGR_OUTA_2)
/// PLLA startup time (in number of slow clock ticks).
#define BOARD_PLLACOUNT         (63 << 8)
/// PLLA MUL value.
#define BOARD_MULA              (AT91C_CKGR_MULA & (PLLA_MULA << 16))  // 设大了PHY配置不了，MDC最多2.5M
//#define BOARD_MULA              (AT91C_CKGR_MULA & (96 << 16))
/// PLLA DIV value.
#define BOARD_DIVA              (AT91C_CKGR_DIVA & 1)
//#define BOARD_DIVA              (AT91C_CKGR_DIVA & 9)

/// PLLB frequency range
#define BOARD_CKGR_PLLB         AT91C_CKGR_OUTB_1
/// PLLB startup time (in number of slow clock ticks).
#define BOARD_PLLBCOUNT         BOARD_PLLACOUNT
/// PLLB MUL value.
#define BOARD_MULB              (124 << 16)
/// PLLB DIV value.
#define BOARD_DIVB              12

/// USB PLL divisor value to obtain a 48MHz clock.
#define BOARD_USBDIV            AT91C_CKGR_USBDIV_2
/// Master clock prescaler value.
#define BOARD_PRESCALER         AT91C_PMC_PRES_CLK_2
/// Master clock DIV value.
#define BOARD_DIV               AT91C_PMC_MDIV_2
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Local functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Default spurious interrupt handler
//------------------------------------------------------------------------------
void DefaultSpuriousHandler(void)
{
    while (1);
}

//------------------------------------------------------------------------------
/// Default handler for fast interrupt requests.
//------------------------------------------------------------------------------
void DefaultFiqHandler(void)
{
    while (1);
}

//------------------------------------------------------------------------------
/// Default handler for standard interrupt requests.
//------------------------------------------------------------------------------
void DefaultIrqHandler(void)
{
    while (1);
}

//------------------------------------------------------------------------------
/// Copy Vectors To Ram. ICF file has done it.
//------------------------------------------------------------------------------
void CopyVectorsToRam(void)
{
    unsigned int i;

    unsigned int *cp1;

    unsigned int *cp2;

    extern void resetVector(void);

    /* 拷贝向量表，保证在flash和ram中程序均可正确运行 */

    cp1 = (unsigned int *)resetVector;

    cp2 = (unsigned int *)0x300000;

    for (i = 0; i < 2 * 8; i++)

    {

        *cp2++ = *cp1++;

    }
}


void CopyFlashToSDRam(void)
{
    unsigned int i;
    //unsigned int j;

    unsigned int *cp1;

    unsigned int *cp2;

    cp1 = (unsigned int *)0x00200000;

    cp2 = (unsigned int *)0x20000000;

    for (i = 0; i < 512 * 1024; i++)

    {

        *cp2++ = *cp1++;

    }

    //j = i;
    
}



//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Performs the low-level initialization of the chip.
//------------------------------------------------------------------------------
void LowLevelInit(void)
{
    unsigned char i;

    // Set flash wait states
    //----------------------
    AT91C_BASE_EFC->EFC_FMR = 6 << 8;
        
#if !defined(sdram)
    // Initialize main oscillator
    //---------------------------
    AT91C_BASE_PMC->PMC_MOR = BOARD_OSCOUNT | AT91C_CKGR_MOSCEN;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCS));

    // Initialize PLLA at 180MHz ,18倍频
    AT91C_BASE_PMC->PMC_PLLAR = BOARD_CKGR_PLLA
                                | BOARD_PLLACOUNT
                                | BOARD_MULA
                                | BOARD_DIVA;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKA));

    // Initialize PLLB for USB usage (if not already locked)
    if (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKB)) {
        AT91C_BASE_PMC->PMC_PLLBR = BOARD_USBDIV
                                    | BOARD_CKGR_PLLB
                                    | BOARD_PLLBCOUNT
                                    | BOARD_MULB
                                    | BOARD_DIVB;
        while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKB));
    }

    // Wait for the master clock if it was already initialized
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

    // Switch to fast clock
    //---------------------
    // Switch to main oscillator + prescaler
    AT91C_BASE_PMC->PMC_MCKR = BOARD_DIV;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

    // Switch to PLL + prescaler
    AT91C_BASE_PMC->PMC_MCKR |= AT91C_PMC_CSS_PLLA_CLK;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

#endif //#if !defined(sdram)

    // Initialize sdram
    BOARD_ConfigureSdram(BOARD_SDRAM_BUSWIDTH);
    SmcInit(16);	
    // Initialize DPRAM
   // BOARD_ConfigureDPRAM(BOARD_DPRAM_BUSWIDTH);
    
    FLASHD_SetGPNVM(3);
    //QQQ 复位引脚设为输入
    AT91C_BASE_RSTC->RSTC_RMR = 0xA5000001;     //重要
  
    // Power ON
    AT91C_BASE_PMC->PMC_PCER = 0xffffffff;
    // 设置系统时钟使能
    AT91C_BASE_PMC->PMC_SCER |= AT91C_PMC_PCK0;
    // 设置PCK0
    AT91C_BASE_PMC->PMC_PCKR[0] = 0;

    // Initialize AIC
    //---------------
    //QQQ 中断设置
    AT91C_BASE_AIC->AIC_IDCR = 0xFFFFFFFF;
    AT91C_BASE_AIC->AIC_SVR[0] = (unsigned int) DefaultFiqHandler;
    for (i = 1; i < 31; i++) {

        AT91C_BASE_AIC->AIC_SVR[i] = (unsigned int) DefaultIrqHandler;
    }
    AT91C_BASE_AIC->AIC_SPU = (unsigned int) DefaultSpuriousHandler;
	
    // Unstack nested interrupts
    for (i = 0; i < 8 ; i++) {

        AT91C_BASE_AIC->AIC_EOICR = 0;
    }

    // Watchdog initialization
    //------------------------
    AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;

    // Remap
    //------
   // *(unsigned long *)0x300000=0;//0xf0b803d0;
    BOARD_RemapRam();    
	//*(unsigned long *)0=0xf0b803d0;
	//*(unsigned long *)0x4=0xf0b803d0;
    
    // Disable RTT and PIT interrupts (potential problem when program A
    // configures RTT, then program B wants to use PIT only, interrupts
    // from the RTT will still occur since they both use AT91C_ID_SYS)
    AT91C_BASE_RTTC->RTTC_RTMR &= ~(AT91C_RTTC_ALMIEN | AT91C_RTTC_RTTINCIEN);
    AT91C_BASE_PITC->PITC_PIMR &= ~AT91C_PITC_PITIEN;
    
}

void SpuriousHandler()
{
}