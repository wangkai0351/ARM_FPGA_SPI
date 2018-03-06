/*
*********************************************************************************************************
*           
*
*Hardware platform : CM-TAS28 , ATSAM9XE512
*
* File             : pio.c
* version          : v1.0
* By               : lp
* Modify         : lp
* Date             : 
*********************************************************************************************************
*/
 #include"global.h"
#include "pio.h"
#include "pio\pio_sam9.h"
#include "includes.h"

#include "at_cfg.h"
#include "pmc.h"
#include"wdt.h"
#include"at_cfg.h"

/*-----------------------------------------------------------------------
�������ƣ�      PIOInit()
����˵����      PIO��ʼ��
��ڲ���˵����  void
���ز���˵����  ��
�޸�˵����      ��
��ע��          ���PIO�ĳ�ʼ��
-------------------------------------------------------------------------*/
void PIOInit(void)
{
  Pin pinList[] = 
                {

                //{1 << 0, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_PULLUP},
                //{1 << 1, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},
                //{1 << 2, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},
                //{1 << 3, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},
                {PIOA_FUC, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},      //PIOA��Ϊ����1������ EMAC
                //{PIOA_FUC_SPI,AT91C_BASE_PIOA,AT91C_ID_PIOA,PIO_PERIPH_A,PIO_DEFAULT},       //PIOA��Ϊ��SPI������
                //{PIOA_FUC_TXD, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT},      //PIOA��Ϊ����2������
                //{PIOA_INPUT, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT},       //PIOA��Ϊ���������
               //{PIOA_OUTPUT, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT},   //PIOA��Ϊ���������

//               {PIOB_FUC, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT},      //PIOB��Ϊ����1������
//              {PIOB_FUC_TXD, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_PULLUP},      //PIOB��Ϊ����1������
                {PIOB_INPUT, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT},       //PIOB��Ϊ���������
                {PIOB_OUTPUT, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT},   //PIOB��Ϊ���������

                {PIOC_FUCA, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT},      //PIOC��Ϊ����1������
                {PIOC_FUCB, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT},      //PIOC��Ϊ����2������
                {PIOC_INPUT, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_INPUT, PIO_DEFAULT},       //PIOC��Ϊ���������
                {PIOC_OUTPUT, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}   //PIOC��Ϊ���������
                
                };
            
    //��ʼ��               
        PIO_Configure(pinList, 7);/*****************************************************************************************
	/*����Ϊ����*/
	//IOSET(AT91C_BASE_PIOC,EN1);
	//IOSET(AT91C_BASE_PIOC,EN2);
//	WDT_SetTime(WDT_DISABLEWDT);  //�Ƚ�ֹ���Ź���λ(�ⲿ���Ź�)

	delay(1000000);
	IOCLR(AT91C_BASE_PIOC,MCTCLR);
	delay(1000000);
	IOSET(AT91C_BASE_PIOC, MCTCLR);
	
	
	/*��λPHYоƬ��GSM*/	
//	IOCLR(AT91C_BASE_PIOA,PHY_RST);
//	delay(1000000);
//	IOSET(AT91C_BASE_PIOA,PHY_RST);			
	
	
    //ʹ��PIOB10���ж�
    //AT91C_BASE_PIOB->PIO_IER = REC_PORT_EN;
    //AT91C_BASE_PIOB->PIO_IDR = ~REC_PORT_EN;
 
}

/*-----------------------------------------------------------------------
�������ƣ�      ()
����˵����      �����
��ڲ���˵����  
���ز���˵����  ��
�޸�˵����      ��
��ע��          �˴�ֻ������PIOB����ΪĿǰ���������PIOB��
-------------------------------------------------------------------------*/
void IOClr(Uint32 io)
{
    AT91C_BASE_PIOB->PIO_CODR = io;
}

/*-----------------------------------------------------------------------
�������ƣ�      ()
����˵����      
��ڲ���˵����  
���ز���˵����  ��
�޸�˵����      ��
��ע��          �˴�ֻ������PIOB����ΪĿǰ���������PIOB��
-------------------------------------------------------------------------*/
void IOSet(Uint32 io)
{
   AT91C_BASE_PIOB->PIO_SODR = io;
}

/*-----------------------------------------------------------------------
�������ƣ�      ()
����˵����      �����
��ڲ���˵����  void
���ز���˵����  ��
�޸�˵����      ��
��ע��          �˴�ֻ������PIOB����ΪĿǰ���������PIOB��
-------------------------------------------------------------------------*/
void IOChange(Uint32 io)//AT91C_BASE_PIOB
{
    unsigned int  IOStat;

    //�õ���ǰ��Ҫ����״̬��IO�ܽŵ�״̬
    IOStat = AT91C_BASE_PIOA->PIO_ODSR & io;  	
    //���ݵ�ǰ״̬�������ǰ״̬Ϊ�ߵ�ƽ�Ĺܽš���Ϊ�ص�ƽ��
    AT91C_BASE_PIOA->PIO_CODR = IOStat & io;
    AT91C_BASE_PIOA->PIO_SODR = (~IOStat) & io;
}

/*-----------------------------------------------------------------------
�������ƣ�      IOChangeABC()
����˵����     �ı�PIO״̬
��ڲ���˵����  void
���ز���˵����  ��
�޸�˵����      ��
��ע��    �ɿ���PIOA��PIOB��PIOC����IO��      
-------------------------------------------------------------------------*/
void IOChangeABC(AT91S_PIO * PIOControler,Uint32 io)
{
    unsigned int  IOStat;

    //�õ���ǰ��Ҫ����״̬��IO�ܽŵ�״̬
    IOStat = PIOControler->PIO_ODSR & io;  	
    //���ݵ�ǰ״̬�������ǰ״̬Ϊ�ߵ�ƽ�Ĺܽš���Ϊ�ص�ƽ��
    PIOControler->PIO_CODR = IOStat & io;
    PIOControler->PIO_SODR = (~IOStat) & io;
}
/*-----------------------------------------------------------------------
�������ƣ�      IOBITStat()
����˵����      
��ڲ���˵����  io
���ز���˵����  ��
�޸�˵����      ��
��ע��          �˴�ֻ������PIOB
-------------------------------------------------------------------------*/
Uint8 IOBITStat(Uint32 io)
{
    Uint32  IOStat;

    IOStat = AT91C_BASE_PIOB->PIO_PDSR;
    if(IOStat & io)
    {
        return PIO_HIGH;
    }
    else
    {
        return PIO_LOW;
    }
}

void LedFlashErr(Uint32 num1, Uint32 num2)
{
    Uint32  i = 0;
   // OS_ENTER_CRITICAL();
    LED_ON(RUNLAMP);
    while(1)
    {
        for(i = 0; i < num1; i++)
        {

        }
        //QQQ WDT_FeedDog();
        LED_OFF(RUNLAMP);

        for(i = 0; i<num2; i++ )
        {

        }
        LED_ON(RUNLAMP);
        //QQQ WDT_FeedDog();
    }
}




/*-----------------------------------------------------------------------
�������ƣ�      ()
����˵����      
��ڲ���˵����  void
���ز���˵����  ��
�޸�˵����      ��
��ע��          �˴�ֻ������PIOB����ΪĿǰ���������PIOB��
-------------------------------------------------------------------------*/
void PioTest(int flag)
{
    while(flag--)
    {   
      IOCLR(AT91C_BASE_PIOA, (LED_ACOM|LED_AFAIL|LED_ARUN));
      delay(500000);//100000
      IOSET(AT91C_BASE_PIOA, (LED_ACOM|LED_AFAIL|LED_ARUN));
      delay(500000);
    }   
}

/*start 2014.10.29 modified by cjf*/
/*-----------------------------------------------------------------------
�������ƣ�      PIOA_ISR_Handler()
����˵����      PIOA�жϴ�����̫������Ӧ
��ڲ���˵����  ��
���ز���˵����  ��
�޸�˵����      ��
��ע��        
-------------------------------------------------------------------------*/
/*
void PIOA_ISR_Handler()
{
    AIC_DisableIT(AT91C_ID_PIOA);
    
   register  Uint32 pio_int_status = 0;
   register  Uint32 pinlevel = ~0;
    
    Uint32 PHY_MII_INT_status = 0;
    Uint32 PHY_status = 0;
    Uint32 SPD_INT = (1<<12);
    Uint32 DUP_INT = (1<<11);
    Uint32 LINK_INT = (1<<13);

    //read PIO interrupt source
    pio_int_status= AT91C_BASE_PIOA->PIO_ISR;
    pio_int_status &= AT91C_BASE_PIOA->PIO_IMR;
    pinlevel = AT91C_BASE_PIOA->PIO_PDSR;

    //deal with external power 
//   if((pio_int_status & POWSTAT) && (pinlevel & POWSTAT))
//    {
//        IOChange(FAIL_LED);
//    }       

    //if PHY interrupts and PHY_INT pin level is low, continue
    if((pio_int_status & PHY_INT) && !(pinlevel & PHY_INT))
    {

        //read PHY interrupt source
        EMAC_ReadPhy(PHY_ADDR, MISR, &PHY_MII_INT_status, 0);

       //configure the EMAC, according to the status of speed and duplex
        if( (PHY_MII_INT_status & SPD_INT)
            || (PHY_MII_INT_status & DUP_INT) )
        {
            EMAC_AutoLink();             
        }

        //Configure PHY speed led out. Make sure that no led is on if there's no cable.
        if(PHY_MII_INT_status & LINK_INT)
        {
            EMAC_ReadPhy(PHY_ADDR, PHYSTS, &PHY_status, 0);

            if(!(PHY_status & 0x01))
            {
                EMAC_WritePhy(PHY_ADDR, LEDCR, 0x0024, 0);
            }
            else
            {
                EMAC_WritePhy(PHY_ADDR, LEDCR, 0, 0);
            }
        }
    }

    AIC_EnableIT(AT91C_ID_PIOA);
}

*/
/*-----------------------------------------------------------------------
�������ƣ�      PIOA_Setup_Int()
����˵����      PIOA�ж�����
��ڲ���˵����  ��
���ز���˵����  ��
�޸�˵����      ��
��ע��        
-------------------------------------------------------------------------*/

/*
void PIOA_Setup_Int()
{
    BSP_IntClr(BSP_PER_ID_PIOA);
    
    // Enable the peripheral clock in the PMC
    PMC_EnablePeripheral(AT91C_ID_PIOA);    
    //Enable PIOA0 Interrupt
    AT91C_BASE_PIOA->PIO_IDR = ~0;
    AT91C_BASE_PIOA->PIO_IFER = PHY_INT;// |POWSTAT;
    AT91C_BASE_PIOA->PIO_IER = PHY_INT;//|POWSTAT;
    
    BSP_IntVectSet((CPU_INT08U   )BSP_PER_ID_PIOA,
                   (CPU_INT08U   )0x3,
                   (CPU_INT08U   )BSP_INT_SCR_TYPE_INT_HIGH_LEVEL_SENSITIVE,
                   (CPU_FNCT_VOID)PIOA_ISR_Handler );

    BSP_IntEn(BSP_PER_ID_PIOA);


}
*/
/*-----------------------------------------------------------------------
�������ƣ�      PIOB_ISR_Handler()
����˵����      PIOB�жϴ���
��ڲ���˵����  ��
���ز���˵����  ��
�޸�˵����      ��
��ע��        
-------------------------------------------------------------------------*/
/*
void PIOB_ISR_Handler()
{
    AIC_DisableIT(AT91C_ID_PIOB);  


    AIC_EnableIT(AT91C_ID_PIOB);
}
*/
/*-----------------------------------------------------------------------
�������ƣ�      PIOB_Setup_Int()
����˵����      PIOB�ж�����
��ڲ���˵����  ��
���ز���˵����  ��
�޸�˵����      ��
��ע��        
-------------------------------------------------------------------------*/
/*
void PIOB_Setup_Int()
{
    BSP_IntClr(BSP_PER_ID_PIOB);
        
    // Enable the peripheral clock in the PMC
    PMC_EnablePeripheral(AT91C_ID_PIOB);    
    
    //Enable PIOB0 Interrupt
    AT91C_BASE_PIOB->PIO_IDR = ~0;
    AT91C_BASE_PIOB->PIO_IFER = DET_OPEN1 | DET_OPEN2;
    AT91C_BASE_PIOB->PIO_IER = DET_OPEN1 | DET_OPEN2;
    //AIC_EnableIT(AT91C_ID_PIOB);

    BSP_IntVectSet((CPU_INT08U   )BSP_PER_ID_PIOB,
                   (CPU_INT08U   )0x6,
                   (CPU_INT08U   )BSP_INT_SCR_TYPE_INT_HIGH_LEVEL_SENSITIVE,
                   (CPU_FNCT_VOID)PIOB_ISR_Handler );

    BSP_IntEn(BSP_PER_ID_PIOB);
    

}
*/
/*end 2014.10.29 modified by cjf*/


