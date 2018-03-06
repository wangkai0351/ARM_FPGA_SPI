/*----------------------------------------------------------------------*/

/*-File Description ��Exception Interrupt handle Function       */

/*-Hardware Platform    ��EPA_MainBoard_V3.0,AT91R40008(ARM7TDMI)   */

/*-Author       ��lhj                       */

/*-Modify by        : tjs                       */

/*-Modify Date      ��2004-3-17                 */

/*-Verson       ��V1.0                      */

//�����쳣���������񶼹��𣬳����ܵ�idle��ȥ��ͬʱ�رճ������ж���������жϡ�

/*----------------------------------------------------------------------*/
//#include "EPA_Ethernet.h"
#include "global.h"
#include "includes.h"

#include "main.h"
//#include "includes.h"
#include "irq.h"
#include "aic/aic.h"
#include "HARDWARE/arm.h"
//#include "cardConfig.h"
#include "wdt.h"
//#include "led.h"
#include "netDownload.h"
#include "pio.h"

extern const ExceptionBugData   FlashExpBug;
extern const ExceptionBugData   FlashExpBugZero;
//extern ExceptionBugData UserProFlashExpBug;

Uint32 rt_div0;
extern void LedFlashErr(Uint32 num1, Uint32 num2);
extern void suspendAllTask(void);
void Undefined_instruction_entry(unsigned int PCvalue)
{
    ExpBug.ARMErrFlag = 0x22222222;
            //QQQ test
             PioTest(5);
//LedFlashErr(100000, 100000); 
    suspendAllTask();
}

void Software_interrupt_entry(unsigned int SWI_number)
{
    ExpBug.ARMErrFlag = 0x11111111;
        //QQQ test
LedFlashErr(100000, 100000);    
    suspendAllTask();
}

void Abort_prefetch_entry(unsigned int PCvalue)
{
    ExpBug.ARMErrFlag = 0x33333333;
        //QQQ test
LedFlashErr(100000, 100000);
    suspendAllTask();
}

void Abort_data_entry(unsigned int PCvalue)
{
    ExpBug.ARMErrFlag = 0x44444444;
        //QQQ test
        PioTest(2);

LedFlashErr(100000, 100000);    
    suspendAllTask();
}

void __rt_div0(void)
{
    //Uint32  i;
            //QQQ test
LedFlashErr(100000, 100000);  
    ExpBug.ARMErrFlag = 0x55555555;
    __asm("ldr  r0, [pc, #0] \n"
          "b    .+8 \n"
          "dcd  rt_div0 \n"
          "mov  r0, lr  \n");
/*      
    {
        mov i, lr
    }
*/
    ExpBug.ExpPC = rt_div0 - 4;   //�����������ʱ��pc��ַ
    suspendAllTask();
}

//������������ж�ʼ�ն��ǹر��ŵģ����쳣���������á�
void suspendAllTask(void)
{
    Uint32  i;

#if 0
    if(BootData.resetErrContinueC>0)
    {
/*    
        //writeNetDataToProFlash((Uint32)&EPAUserProInf.LIBUSERPro_OKFlag, (Uint32*)&gEPALib_UserPro_Stat, 4);
        if(UserProFlashExpBug.ARMErrFlag == 0xffffffff) //ֻ�ܼ�¼һ��
        {
            writeNetDataToProFlash((Uint32) & UserProFlashExpBug, (Uint32 *) &ExpBug, sizeof(UserProFlashExpBug));
        }
*/        
    }  
    else
#endif
    {
        if(ExpBug.ARMErrFlag == 0x55555555)
        {
            if(FlashExpBugZero.ARMErrFlag == 0xffffffff)
            {
               //bylp 11.26 writeNetDataToProFlash((Uint32) & FlashExpBugZero, (Uint32 *) &ExpBug, sizeof(FlashExpBugZero));
            }
        }
        else
        {
            if(FlashExpBug.ARMErrFlag == 0xffffffff)    //ֻ�ܼ�¼һ��
            {
                //by lp 11.26 writeNetDataToProFlash((Uint32) & FlashExpBug, (Uint32 *) &ExpBug, sizeof(FlashExpBug));
            }
        }
    }

    OSIntNesting = 1;   //ģ������жϴ���ʹ�ĵ��ò���ϵͳ����ʱ����ֹ�ڷ���������л����쳣���

    WDT_SetTime(WDT_200MSWDT); //�����Ź���λ

    //    OSLockNesting = 1;  //��ֹ�����л��������Ͳ�����Ӧ��������ֱ�������������ˡ�ֻ�ܽ����ж�
    LED_ON(RUNLAMP);
    i = 0;
    while(1)
    {
        i++;
        if((i & 0xffff) == 0)
        {
            LED_ON(RUNLAMP);
        }

        if((i & 0xffff) == 0xefff)
        {
            LED_OFF(RUNLAMP);
        }
    }
}
