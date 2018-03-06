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
函数名称：      PIOInit()
函数说明：      PIO初始化
入口参数说明：  void
返回参数说明：  无
修改说明：      无
备注：          完成PIO的初始化
-------------------------------------------------------------------------*/
void PIOInit(void)
{
  Pin pinList[] = 
                {

                //{1 << 0, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_PULLUP},
                //{1 << 1, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},
                //{1 << 2, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},
                //{1 << 3, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},
                {PIOA_FUC, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},      //PIOA作为外设1的引脚 EMAC
                //{PIOA_FUC_SPI,AT91C_BASE_PIOA,AT91C_ID_PIOA,PIO_PERIPH_A,PIO_DEFAULT},       //PIOA作为外SPI的引脚
                //{PIOA_FUC_TXD, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT},      //PIOA作为外设2的引脚
                //{PIOA_INPUT, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT},       //PIOA作为输入的引脚
               //{PIOA_OUTPUT, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT},   //PIOA作为输出的引脚

//               {PIOB_FUC, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT},      //PIOB作为外设1的引脚
//              {PIOB_FUC_TXD, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_PULLUP},      //PIOB作为外设1的引脚
                {PIOB_INPUT, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT},       //PIOB作为输入的引脚
                {PIOB_OUTPUT, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT},   //PIOB作为输出的引脚

                {PIOC_FUCA, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT},      //PIOC作为外设1的引脚
                {PIOC_FUCB, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT},      //PIOC作为外设2的引脚
                {PIOC_INPUT, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_INPUT, PIO_DEFAULT},       //PIOC作为输入的引脚
                {PIOC_OUTPUT, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}   //PIOC作为输出的引脚
                
                };
            
    //初始化               
        PIO_Configure(pinList, 7);/*****************************************************************************************
	/*设置为供电*/
	//IOSET(AT91C_BASE_PIOC,EN1);
	//IOSET(AT91C_BASE_PIOC,EN2);
//	WDT_SetTime(WDT_DISABLEWDT);  //先禁止看门狗复位(外部看门狗)

	delay(1000000);
	IOCLR(AT91C_BASE_PIOC,MCTCLR);
	delay(1000000);
	IOSET(AT91C_BASE_PIOC, MCTCLR);
	
	
	/*复位PHY芯片及GSM*/	
//	IOCLR(AT91C_BASE_PIOA,PHY_RST);
//	delay(1000000);
//	IOSET(AT91C_BASE_PIOA,PHY_RST);			
	
	
    //使能PIOB10的中断
    //AT91C_BASE_PIOB->PIO_IER = REC_PORT_EN;
    //AT91C_BASE_PIOB->PIO_IDR = ~REC_PORT_EN;
 
}

/*-----------------------------------------------------------------------
函数名称：      ()
函数说明：      输出低
入口参数说明：  
返回参数说明：  无
修改说明：      无
备注：          此处只设置了PIOB，因为目前的输出都在PIOB上
-------------------------------------------------------------------------*/
void IOClr(Uint32 io)
{
    AT91C_BASE_PIOB->PIO_CODR = io;
}

/*-----------------------------------------------------------------------
函数名称：      ()
函数说明：      
入口参数说明：  
返回参数说明：  无
修改说明：      无
备注：          此处只设置了PIOB，因为目前的输出都在PIOB上
-------------------------------------------------------------------------*/
void IOSet(Uint32 io)
{
   AT91C_BASE_PIOB->PIO_SODR = io;
}

/*-----------------------------------------------------------------------
函数名称：      ()
函数说明：      输出低
入口参数说明：  void
返回参数说明：  无
修改说明：      无
备注：          此处只设置了PIOB，因为目前的输出都在PIOB上
-------------------------------------------------------------------------*/
void IOChange(Uint32 io)//AT91C_BASE_PIOB
{
    unsigned int  IOStat;

    //得到当前需要更改状态的IO管脚的状态
    IOStat = AT91C_BASE_PIOA->PIO_ODSR & io;  	
    //根据当前状态，清除当前状态为高电平的管脚。成为地点平。
    AT91C_BASE_PIOA->PIO_CODR = IOStat & io;
    AT91C_BASE_PIOA->PIO_SODR = (~IOStat) & io;
}

/*-----------------------------------------------------------------------
函数名称：      IOChangeABC()
函数说明：     改变PIO状态
入口参数说明：  void
返回参数说明：  无
修改说明：      无
备注：    可控制PIOA、PIOB、PIOC三个IO口      
-------------------------------------------------------------------------*/
void IOChangeABC(AT91S_PIO * PIOControler,Uint32 io)
{
    unsigned int  IOStat;

    //得到当前需要更改状态的IO管脚的状态
    IOStat = PIOControler->PIO_ODSR & io;  	
    //根据当前状态，清除当前状态为高电平的管脚。成为地点平。
    PIOControler->PIO_CODR = IOStat & io;
    PIOControler->PIO_SODR = (~IOStat) & io;
}
/*-----------------------------------------------------------------------
函数名称：      IOBITStat()
函数说明：      
入口参数说明：  io
返回参数说明：  无
修改说明：      无
备注：          此处只设置了PIOB
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
函数名称：      ()
函数说明：      
入口参数说明：  void
返回参数说明：  无
修改说明：      无
备注：          此处只设置了PIOB，因为目前的输出都在PIOB上
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
函数名称：      PIOA_ISR_Handler()
函数说明：      PIOA中断处理，以太网自适应
入口参数说明：  无
返回参数说明：  无
修改说明：      无
备注：        
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
函数名称：      PIOA_Setup_Int()
函数说明：      PIOA中断设置
入口参数说明：  无
返回参数说明：  无
修改说明：      无
备注：        
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
函数名称：      PIOB_ISR_Handler()
函数说明：      PIOB中断处理
入口参数说明：  无
返回参数说明：  无
修改说明：      无
备注：        
-------------------------------------------------------------------------*/
/*
void PIOB_ISR_Handler()
{
    AIC_DisableIT(AT91C_ID_PIOB);  


    AIC_EnableIT(AT91C_ID_PIOB);
}
*/
/*-----------------------------------------------------------------------
函数名称：      PIOB_Setup_Int()
函数说明：      PIOB中断设置
入口参数说明：  无
返回参数说明：  无
修改说明：      无
备注：        
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


