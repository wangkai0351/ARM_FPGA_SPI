/*
*********************************************************************************************************
*         DW Project       
*
*     All Rights Reserved
*
* Hardware platform : EPA_MainBoard_V3.0,AT91R40008(ARM7TDMI)
* Description       : 
* File              : timer.c
* version           : v1.0
* By                : tjs
* Date              : 
*********************************************************************************************************
*/
#include "tc.h"
//#include "cardConfig.h"
#include "timer.h"
#include "Global.h"
//#include "csme_out.h"
#include "irq.h"
//#include "cardConfig.h"


/*定义晶振数值以及分频设置*/
#define TC_CLKS_MCK 90000000        /* 90M MCK */
#define SYS_MCK     32
#define SYS_CLK_MCK TC_CLKS_MCK32   /*32分频*/

#define EPA_MCK     2                    
#define EPA_CLK_MCK TC_CLKS_MCK2    /*2分频*/

/*定义10ms时间，系统时钟使用到的tick数*/
#define SYSTICK_10MS    (10 * (TC_CLKS_MCK / SYS_MCK) / 1000)   /* 10MS有多少个系统定时器的Tick */

/*定义10ms时间，epa时钟使用到的tick数*/
#define EPATICK_10MS    (10 * (TC_CLKS_MCK / EPA_MCK) / 1000)   /* 10MS有多少个EPA定时器的Tick */

/*定义EPATick_Times的值，防止分母子小于分母?*/

#define EPATick_Times   90  /*180MHZ 2分频*/

/*定义一个tick相当于多少ns或者us*/
#define SYSTICK_NSUNIT  (((SECOND_NSUNIT / SECOND_USUNIT) * SYS_MCK) / (TC_CLKS_MCK / SECOND_USUNIT))   /* 系统的1个Tick相当于多少ns */
#define EPATICK_USUNIT  ((EPATick_Times * EPA_MCK) / (TC_CLKS_MCK / SECOND_USUNIT))                     /* CSME定时器的EPATick_Times个Tick相当于多少us*/
#define EPATICK_NSUNIT  (((SECOND_NSUNIT / SECOND_USUNIT) * EPA_MCK) / (TC_CLKS_MCK / SECOND_USUNIT))  /* CSME定时器的EPATick_Times个Tick相当于多少ns*/

/*定义2的16次方*/
#define MAXDELAY    65535
Uint32  highDelayTime;
Uint32  lowDelayTime;
Uint32  tcStatus = 0; //用以记录定时器TC0状态的全局变量
TIMESTAMP  sendPtpTime; //用以记录定时器TC0状态的全局变量
REC_PTP_TIME *pRecPtpTime = NULL;
    
/* 系统时钟，作为PTP和UCOS-II的时间驱动 */
void EPAPTPSYSTimeInit(void)
{
    
//配置作为系统时钟,RA加载中断使能
    TCBlock.TC_BMR |= TC_NONEXC0;                   /* non extern clock signal */
    TCBlock.TC[0].TC_CMR = SYS_CLK_MCK | TC_CPCTRG | TC_CAPT | TC_ABETRG_TIOA | TC_LDRA_RISING_EDGE; /* clock selection RC compare trigger enable */
    TCBlock.TC[0].TC_RC = SYSTICK_10MS*SYS_TICK_MS;             /* set RC value */
    TCBlock.TC[0].TC_IDR = TC_COVFS | TC_LOVRS | TC_LDRBS | TC_ETRGS;    /* disable unused interrupt */
    TCBlock.TC[0].TC_IER = TC_CPCS | TC_LDRAS;             /* enable RC compare interrupt */
  
    TCBlock.TC_BMR |= TC_NONEXC1;                   /* non extern clock signal */
    TCBlock.TC[1].TC_CMR = SYS_CLK_MCK | TC_CAPT | TC_ABETRG_TIOA | TC_LDRA_RISING_EDGE; /* clock selection RC compare trigger enable */
    TCBlock.TC[1].TC_IDR = TC_COVFS | TC_LOVRS | TC_LDRBS | TC_ETRGS;    /* disable unused interrupt */
    TCBlock.TC[1].TC_IER = TC_LDRAS;             

    TCBlock.TC_BMR |= TC_NONEXC2;                   /* non extern clock signal */
    TCBlock.TC[2].TC_CMR = SYS_CLK_MCK | TC_CAPT | TC_ABETRG_TIOA | TC_LDRA_RISING_EDGE; /* clock selection RC compare trigger enable */
    TCBlock.TC[2].TC_IDR = TC_COVFS | TC_LOVRS | TC_LDRBS | TC_ETRGS;    /* disable unused interrupt */
    TCBlock.TC[2].TC_IER = TC_LDRAS;             

//一起开始计时
    TCBlock.TC[0].TC_CCR = TC_CLKEN;// | TC_SWTRG; /* enable and software reset TC */
    TCBlock.TC[1].TC_CCR = TC_CLKEN;// | TC_SWTRG; /* enable and software reset TC */
    TCBlock.TC[2].TC_CCR = TC_CLKEN;// | TC_SWTRG; /* enable and software reset TC */
    TCBlock.TC_BCR |= TC_SYNC; 
}

/*
获得PTP定时器部分的时间函数，不直接提供，只供PTP获得时间的函数直接使用
*/
Uint32 GetSysTickTime(void) /* 单位为ns */
{
    Uint32  i;
    i = TCBlock.TC[0].TC_CV;    /* 读取定时器值 */
    i = i * SYSTICK_NSUNIT;     /* 转换定时器单位 */
    return i;
}

Uint32 GetPtpTickTime(int channel) /* 单位为ns */
{
    Uint32  i;
    switch(channel)
    {
        case 1:
            i = TCBlock.TC[0].TC_RA;    /* 读取定时器值 */
            i = i * SYSTICK_NSUNIT;     /* 转换定时器单位 */
            break;
        case 2:
            i = TCBlock.TC[1].TC_RA;    /* 读取定时器值 */
            i = i * SYSTICK_NSUNIT;     /* 转换定时器单位 */
            break;
        case 3:
            i = TCBlock.TC[2].TC_RA;    /* 读取定时器值 */
            i = i * SYSTICK_NSUNIT;     /* 转换定时器单位 */
            break;
        default:
            break;
    }
    return i;
}

/* 
不能通过直接读取定时器的标志位来实现

定时中断方式: 上升沿触发

读取定时器的标志位虽然不会对中断造成影响，但是
在第一次读取中断标志位后，再次读取该标志位时，就会被丢失一个定时时间。

比如:

    i = TCBlock.TC[0].TC_SR; 
    i = (i&0x10) == 0x10)?1:0 		判断是否出现了定时中断 


在用户程序中读取了时间，此时已经中断，用户程序自己补充上漏掉的一个定时时间
此时用户读取的时间是正确的

在读取时间结束后，此时来了网卡中断(包括定时中断，但网卡中断优先级高)，并在中断中读取时间，
此时网卡中断中的读取时间就会丢失一个定时时间。
这样网卡中断中读取的时间就丢失了一个定时时间

处理定时中断处理。

输入参数:
无

返回值
1:	定时中断挂起
0:	定时中断已近处理完毕，并没有新发生

*/
Uint32 ISPTPSysTimePend(void)
{
    Uint32  i;

    i = ReadIntPnd();
    i = ((i & AT91C_ID_TC0) == AT91C_ID_TC0) ? 1 : 0;

    return i;
}

/* 
调度事件时钟，作为CSME调度的驱动 
*/
void EPACSMETimeInit(void)
{
//QQQ 两个定时器级联
#if 0
    TCBlock.TC_BMR |= TC_NONEXC2;   /* refer to TC1Init */
    TCBlock.TC[2].TC_CMR = SYS_CLK_MCK | TC_WAVE | TC_CPCTRG | TC_ACPC_TOGGLE_OUTPUT;
    TCBlock.TC[2].TC_RC = 0xff;
    //TCBlock.TC[2].TC_RA = 0xff;
    TCBlock.TC[2].TC_IDR = TC_COVFS | TC_LOVRS | TC_LDRAS | TC_LDRBS | TC_ETRGS;
    //TCBlock.TC[2].TC_IER = TC_CPCS;
    TCBlock.TC[2].TC_CCR = TC_CLKEN | TC_SWTRG;

    TCBlock.TC_BMR |= TC_TC1XC1S;    /* External Clock Signal 2 Selection */
    TCBlock.TC[1].TC_CMR = TC_CLKS_XC1 | TC_CPCTRG /*| TC_WAVE | TC_ACPC_TOGGLE_OUTPUT*/;
    TCBlock.TC[1].TC_RC = 15;//EPATICK_10MS;
    TCBlock.TC[1].TC_IDR = TC_COVFS | TC_LOVRS | TC_LDRAS | TC_LDRBS | TC_ETRGS;
    TCBlock.TC[1].TC_IER = TC_CPCS;
    TCBlock.TC[1].TC_CCR = TC_CLKEN | TC_SWTRG;
#endif    
#if 1
    TCBlock1.TC_BMR |= TC_NONEXC0;   /* refer to TC1Init */
    TCBlock1.TC[0].TC_CMR = EPA_CLK_MCK | TC_CPCTRG | TC_WAVE | TC_ACPC_TOGGLE_OUTPUT;
    TCBlock1.TC[0].TC_RC = EPATICK_10MS/10;
    //TCBlock1.TC[0].TC_RA = 0xff;
    TCBlock1.TC[0].TC_IDR = TC_COVFS | TC_LOVRS | TC_LDRAS | TC_LDRBS | TC_ETRGS;
    TCBlock1.TC[0].TC_IER = TC_CPCS;
    TCBlock1.TC[0].TC_CCR = TC_CLKEN;// | TC_SWTRG;

    TCBlock1.TC_BMR |= TC_TIOA0XC1;    /* 外部时钟信号选择 */
    TCBlock1.TC[1].TC_CMR = TC_CLKS_XC1 | TC_CPCTRG | TC_WAVE | TC_ACPC_TOGGLE_OUTPUT;
    TCBlock1.TC[1].TC_RC = 500;//QQQ CSME定时 ;
    TCBlock1.TC[1].TC_IDR = TC_COVFS | TC_LOVRS | TC_LDRAS | TC_LDRBS | TC_ETRGS;
    TCBlock1.TC[1].TC_IER = TC_CPCS;
    TCBlock1.TC[1].TC_CCR = TC_CLKEN;// | TC_SWTRG;

    TCBlock1.TC_BCR |= TC_SYNC;
#endif    
}

Uint32 GetCSMETickTime(void)    /* 单位为us */
{
    Uint32  i;
    i = TCBlock1.TC[1].TC_CV;    /* 读取定时器值 */
    i = i * EPATICK_USUNIT / EPATick_Times;
    return i;
}

/*
设置CSME定时事件的定时函数
*/
void SetCSMETime(Uint32 timeDelay)      /*单位为us*/
{
    Uint32 ticks;
    if(timeDelay < MS_USUNIT)
    {
        timeDelay = MS_USUNIT;
    }

    if(timeDelay > SECOND_USUNIT)
    {
        timeDelay = SECOND_USUNIT;
    }
/*
    TCBlock1.TC[0].TC_RC = timeDelay * EPATick_Times / EPATICK_USUNIT;
    TCBlock1.TC[0].TC_CCR = TC_CLKEN | TC_SWTRG;
*/    
    ticks = (timeDelay*1000) / EPATICK_NSUNIT;      /*转换为纳秒，再计算需要几个ticks*/

    /*用以两个定时器级联时设置CSME定时中断*/
    highDelayTime = ticks / MAXDELAY;
    lowDelayTime = ticks % MAXDELAY;
    if(ticks >= MAXDELAY)
    {      
        TCBlock1.TC[0].TC_RC = 0x7fff;          //因为两个RC才产生一个上升沿或者下降沿，所以是0xffff的一半
        TCBlock1.TC[0].TC_CCR = TC_CLKEN | TC_SWTRG;
        TCBlock1.TC[1].TC_RC = highDelayTime;
        TCBlock1.TC[1].TC_CCR = TC_CLKEN | TC_SWTRG;
    }
    else
    {
        TCBlock1.TC[0].TC_RC = lowDelayTime;
        TCBlock1.TC[0].TC_CCR = TC_CLKEN | TC_SWTRG;
        TCBlock1.TC[1].TC_CCR = TC_CLKDIS;
    }
    
    DiagCSME.test4++;
}

void SetCSMEoldTime(Uint32 timeDelay)   /*单位为us*/
{
    if(timeDelay < MS_USUNIT)
    {
        timeDelay = MS_USUNIT;
    }

    if(timeDelay > SECOND_USUNIT)
    {
        timeDelay = SECOND_USUNIT;
    }

    TCBlock1.TC[1].TC_RC = timeDelay * EPATick_Times / EPATICK_USUNIT;
    DiagCSME.test3++;

    //TCBlock.TC[1].TC_CCR = TC_CLKEN | TC_SWTRG;
}
