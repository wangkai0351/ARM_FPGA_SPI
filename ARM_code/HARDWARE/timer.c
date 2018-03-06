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


/*���徧����ֵ�Լ���Ƶ����*/
#define TC_CLKS_MCK 90000000        /* 90M MCK */
#define SYS_MCK     32
#define SYS_CLK_MCK TC_CLKS_MCK32   /*32��Ƶ*/

#define EPA_MCK     2                    
#define EPA_CLK_MCK TC_CLKS_MCK2    /*2��Ƶ*/

/*����10msʱ�䣬ϵͳʱ��ʹ�õ���tick��*/
#define SYSTICK_10MS    (10 * (TC_CLKS_MCK / SYS_MCK) / 1000)   /* 10MS�ж��ٸ�ϵͳ��ʱ����Tick */

/*����10msʱ�䣬epaʱ��ʹ�õ���tick��*/
#define EPATICK_10MS    (10 * (TC_CLKS_MCK / EPA_MCK) / 1000)   /* 10MS�ж��ٸ�EPA��ʱ����Tick */

/*����EPATick_Times��ֵ����ֹ��ĸ��С�ڷ�ĸ?*/

#define EPATick_Times   90  /*180MHZ 2��Ƶ*/

/*����һ��tick�൱�ڶ���ns����us*/
#define SYSTICK_NSUNIT  (((SECOND_NSUNIT / SECOND_USUNIT) * SYS_MCK) / (TC_CLKS_MCK / SECOND_USUNIT))   /* ϵͳ��1��Tick�൱�ڶ���ns */
#define EPATICK_USUNIT  ((EPATick_Times * EPA_MCK) / (TC_CLKS_MCK / SECOND_USUNIT))                     /* CSME��ʱ����EPATick_Times��Tick�൱�ڶ���us*/
#define EPATICK_NSUNIT  (((SECOND_NSUNIT / SECOND_USUNIT) * EPA_MCK) / (TC_CLKS_MCK / SECOND_USUNIT))  /* CSME��ʱ����EPATick_Times��Tick�൱�ڶ���ns*/

/*����2��16�η�*/
#define MAXDELAY    65535
Uint32  highDelayTime;
Uint32  lowDelayTime;
Uint32  tcStatus = 0; //���Լ�¼��ʱ��TC0״̬��ȫ�ֱ���
TIMESTAMP  sendPtpTime; //���Լ�¼��ʱ��TC0״̬��ȫ�ֱ���
REC_PTP_TIME *pRecPtpTime = NULL;
    
/* ϵͳʱ�ӣ���ΪPTP��UCOS-II��ʱ������ */
void EPAPTPSYSTimeInit(void)
{
    
//������Ϊϵͳʱ��,RA�����ж�ʹ��
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

//һ��ʼ��ʱ
    TCBlock.TC[0].TC_CCR = TC_CLKEN;// | TC_SWTRG; /* enable and software reset TC */
    TCBlock.TC[1].TC_CCR = TC_CLKEN;// | TC_SWTRG; /* enable and software reset TC */
    TCBlock.TC[2].TC_CCR = TC_CLKEN;// | TC_SWTRG; /* enable and software reset TC */
    TCBlock.TC_BCR |= TC_SYNC; 
}

/*
���PTP��ʱ�����ֵ�ʱ�亯������ֱ���ṩ��ֻ��PTP���ʱ��ĺ���ֱ��ʹ��
*/
Uint32 GetSysTickTime(void) /* ��λΪns */
{
    Uint32  i;
    i = TCBlock.TC[0].TC_CV;    /* ��ȡ��ʱ��ֵ */
    i = i * SYSTICK_NSUNIT;     /* ת����ʱ����λ */
    return i;
}

Uint32 GetPtpTickTime(int channel) /* ��λΪns */
{
    Uint32  i;
    switch(channel)
    {
        case 1:
            i = TCBlock.TC[0].TC_RA;    /* ��ȡ��ʱ��ֵ */
            i = i * SYSTICK_NSUNIT;     /* ת����ʱ����λ */
            break;
        case 2:
            i = TCBlock.TC[1].TC_RA;    /* ��ȡ��ʱ��ֵ */
            i = i * SYSTICK_NSUNIT;     /* ת����ʱ����λ */
            break;
        case 3:
            i = TCBlock.TC[2].TC_RA;    /* ��ȡ��ʱ��ֵ */
            i = i * SYSTICK_NSUNIT;     /* ת����ʱ����λ */
            break;
        default:
            break;
    }
    return i;
}

/* 
����ͨ��ֱ�Ӷ�ȡ��ʱ���ı�־λ��ʵ��

��ʱ�жϷ�ʽ: �����ش���

��ȡ��ʱ���ı�־λ��Ȼ������ж����Ӱ�죬����
�ڵ�һ�ζ�ȡ�жϱ�־λ���ٴζ�ȡ�ñ�־λʱ���ͻᱻ��ʧһ����ʱʱ�䡣

����:

    i = TCBlock.TC[0].TC_SR; 
    i = (i&0x10) == 0x10)?1:0 		�ж��Ƿ�����˶�ʱ�ж� 


���û������ж�ȡ��ʱ�䣬��ʱ�Ѿ��жϣ��û������Լ�������©����һ����ʱʱ��
��ʱ�û���ȡ��ʱ������ȷ��

�ڶ�ȡʱ������󣬴�ʱ���������ж�(������ʱ�жϣ��������ж����ȼ���)�������ж��ж�ȡʱ�䣬
��ʱ�����ж��еĶ�ȡʱ��ͻᶪʧһ����ʱʱ�䡣
���������ж��ж�ȡ��ʱ��Ͷ�ʧ��һ����ʱʱ��

����ʱ�жϴ���

�������:
��

����ֵ
1:	��ʱ�жϹ���
0:	��ʱ�ж��ѽ�������ϣ���û���·���

*/
Uint32 ISPTPSysTimePend(void)
{
    Uint32  i;

    i = ReadIntPnd();
    i = ((i & AT91C_ID_TC0) == AT91C_ID_TC0) ? 1 : 0;

    return i;
}

/* 
�����¼�ʱ�ӣ���ΪCSME���ȵ����� 
*/
void EPACSMETimeInit(void)
{
//QQQ ������ʱ������
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

    TCBlock1.TC_BMR |= TC_TIOA0XC1;    /* �ⲿʱ���ź�ѡ�� */
    TCBlock1.TC[1].TC_CMR = TC_CLKS_XC1 | TC_CPCTRG | TC_WAVE | TC_ACPC_TOGGLE_OUTPUT;
    TCBlock1.TC[1].TC_RC = 500;//QQQ CSME��ʱ ;
    TCBlock1.TC[1].TC_IDR = TC_COVFS | TC_LOVRS | TC_LDRAS | TC_LDRBS | TC_ETRGS;
    TCBlock1.TC[1].TC_IER = TC_CPCS;
    TCBlock1.TC[1].TC_CCR = TC_CLKEN;// | TC_SWTRG;

    TCBlock1.TC_BCR |= TC_SYNC;
#endif    
}

Uint32 GetCSMETickTime(void)    /* ��λΪus */
{
    Uint32  i;
    i = TCBlock1.TC[1].TC_CV;    /* ��ȡ��ʱ��ֵ */
    i = i * EPATICK_USUNIT / EPATick_Times;
    return i;
}

/*
����CSME��ʱ�¼��Ķ�ʱ����
*/
void SetCSMETime(Uint32 timeDelay)      /*��λΪus*/
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
    ticks = (timeDelay*1000) / EPATICK_NSUNIT;      /*ת��Ϊ���룬�ټ�����Ҫ����ticks*/

    /*����������ʱ������ʱ����CSME��ʱ�ж�*/
    highDelayTime = ticks / MAXDELAY;
    lowDelayTime = ticks % MAXDELAY;
    if(ticks >= MAXDELAY)
    {      
        TCBlock1.TC[0].TC_RC = 0x7fff;          //��Ϊ����RC�Ų���һ�������ػ����½��أ�������0xffff��һ��
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

void SetCSMEoldTime(Uint32 timeDelay)   /*��λΪus*/
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
