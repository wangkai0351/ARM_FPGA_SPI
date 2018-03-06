/*
*********************************************************************************************************
*         DW Project       
*
* File             : irq.c
* version          : v1.0
* By               : lp
* Modify         : lp
* Date             : 
*********************************************************************************************************
*/
#include "global.h"
//#include "includes.h"
#include "irq.h"

//#include "PIO.h"
//#include "led.h"
#include "includes.h"
#include "aic/aic.h"
#include "tc.h"

#include "main.h"
//#include "timer.h"
#include "AT91SAM9XE_out.H"
#include "emac/emac.h"

#include "aic.h"
#include "pio.h"
#include "At_task.h"
#include "Bu61580.h"


extern void NetNIC_Setup_Int (void);
void PIOC_Handler(void);
/*
****************************************************************************************************
* The sp value in the time running user program, used for User program timeout control
* install interrupt handler, move the handlers to the correspondent registers. This function is 
* related to handware resource(CPU)
****************************************************************************************************
*/
void Int_Install(void)
{    
//QQQ TC0/TC1/TC2可能作为系统时钟，TC3/TC4级联成32位CSME定时器
   // AIC_ConfigureIT(AT91C_ID_TC0, AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED | 0x4, TC0_Handler);
   // AIC_ConfigureIT(AT91C_ID_TC1, AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED | 0x4, TC1_Handler);
    //AIC_ConfigureIT(AT91C_ID_TC2, AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED | 0x4, TC2_Handler);
   // AIC_ConfigureIT(AT91C_ID_TC3, AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED | 0x5, TC3_Handler);
   // AIC_ConfigureIT(AT91C_ID_TC4, AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED | 0x5, TC4_Handler);

    AIC_ConfigureIT(AT91C_ID_PIOC, AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED | 0x3, PIOC_Handler);


    /*start 2013-12-20 modified by xjx, */
    // Setup EMAC buffers and interrupts
    //AIC_ConfigureIT(AT91C_ID_EMAC, AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL | 0x5, NetNIC_ISR_Handler); //AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL
    /*end 2013-12-20 modified by xjx*/
     AIC_ConfigureIT(AT91C_ID_IRQ1,AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED |0x6, RT_Handler);//AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE
	
     //Setup IRQ1 BU61580中断
//    AIC_ConfigureIT(AT91C_ID_IRQ1,AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED |0x6, BM_Handler);//AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE

    NetNIC_Setup_Int();
	
    /*start 2014-02-11 modified by cjf  */
    // Configure USART0 rxbuffer interrupts
    //AIC_ConfigureIT(AT91C_ID_US0, AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE | 0x4, USART0_ISR_Handler);//AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE
    //USART0_Setup_Int();
//    PIOA_Setup_Int();	
//    PIOB_Setup_Int();
    //AIC_EnableIT(AT91C_ID_US0);
    /*end 2014-02-11 modified by cjf */

//    IRQ1_Setup_Int();
//    IRQ2_Setup_Int();


}

/* 
 * set interrupt
 */
void SetInt(unsigned char n)
{
    AIC.AIC_ISCR = 1 << n;
}

/*
 * clear interrupt
 */
void ClrInt(unsigned char n)
{
    AIC.AIC_ICCR = 1 << n;
}

// Manual operation ,used for redundance switch
void FIQ_Handler(void)
{
}

// Sofeware interrupt
void SWI_IRQ_Handler(void)
{
}

// USRAT0 interrupt
void USART0_Handler(void)
{

}

// USRAT1 interrupt
void USART1_Handler(void)
{

}

typedef  void (*BSP_PFNCT)(void);
void  OS_CPU_IRQ_ISR_Handler (void) {
    BSP_PFNCT  pfnct;
#if 1
    pfnct = (BSP_PFNCT)AT91C_BASE_AIC->AIC_IVR;     // 从AIC中读取中断向量
    if (pfnct != (BSP_PFNCT)0) {                    // 确保没有 NULL 指针
        (*pfnct)();                                 // 执行中断函数
    }
#else
    pfnct = (BSP_PFNCT)AT91C_BASE_AIC->AIC_IVR;     // 从AIC中读取中断向量
    while (pfnct != (BSP_PFNCT)0) {                 // 确保没有 NULL 指针
      (*pfnct)();                                   // 执行中断函数
        pfnct = (BSP_PFNCT)AT91C_BASE_AIC->AIC_IVR; // 从AIC中读取中断向量
    }
#endif
}


void PIOC_Handler(void)
{
    //QQQ 记录接收的端口，保存到内存中，以保证每个报文的接收端口都能获取
    //Uint32 i;
    //Uint8  ioState;

    //读取状态寄存器
    //i = AT91C_BASE_PIOC->PIO_ISR;

    //是否是判断接收端口ID的引脚
    //if(i & INT61580)
}
void EMAC_Handler(void)
{
#if OS_CRITICAL_METHOD == 3 /* Allocate storage for CPU status register      */
    OS_CPU_SR   cpu_sr;
#endif
    OS_ENTER_CRITICAL();


    EMAC_ISR();
    OS_EXIT_CRITICAL();
}
void RT_Handler(void)
{
  Uint16 INT_STATUS=0;

 //AIC_DisableIT(AT91C_ID_IRQ1);   //关IRQ1中断
 INT_STATUS=RREG(REG_INT_STAT); 
 if(INT_STATUS)
  {
  
   //OSSemPost(OSBusSem);
//   BusTaskCNT++;

		 if(INT_STATUS&0x0001)
		    {
//		     BM_EOM_Flag++;	  
			OSSemPost(AtTask_TCPSend_SignalPtr);
		    }	
	
//	    	 if(INT_STATUS&0x0040)
//		    {
//		     BM_TIME_TAG_ROLLOVER_Flag++;	    
//		    }
/*
BM_CNT++;
	if(BM_CNT==500)
	{
		IOCLR(AT91C_BASE_PIOA, LED_ACOM);
	}
	else if(BM_CNT==1000)
	{
		 IOSET(AT91C_BASE_PIOA, LED_ACOM);
		 BM_CNT=0;
	}	 */
  }

}

// Spurious interrupt,declare int,but be compelled the int
void SPU_Handler(void)
{
    //    bug.INTspuC++;
}

// Enable interrupt
void EnableInt(unsigned char n)
{
    AIC.AIC_IECR = 1 << n;
}

// Disable interrupt
void DisableInt(unsigned char n)
{
    AIC.AIC_IDCR = 1 << n;
}

// Disable all current interrupt source
unsigned int MaskGlobalInt(void)
{
    unsigned int    i;

    i = AIC.AIC_IMR;    //read current interrupt enable status
    return(i);
}

// resume interrupt
void RestoreGlobalInt(unsigned int i)
{
    AIC.AIC_IDCR = ~i;
    AIC.AIC_IECR = i;
}

// Read current interrupt info
unsigned int ReadIntPnd(void)
{
    return(AIC.AIC_IPR);
}
