/********************************************************************
 *文件说明: PIO配置头文件
 *文件作者:
 *文件版本: V1.0
 *修改日期: 2010-12-2
 *版权所有: 
 ********************************************************************/

//主要作用：对IO操作的原始级接口功能描述。
#ifndef _PIO_H_
#define _PIO_H_

#include "global.h"
//#include "epaDebugConfig.h"
#include "at91sam9xe_out.h"

#define at91_reg    volatile unsigned int
#define IOCLR(a,b)  a->PIO_CODR = b
#define IOSET(a,b)  a->PIO_SODR = b
    
#define  READY                  1  
/*------------------------------------------------------------*/

/* User Interface Parallel I/O Interface Structure Definition */

/*------------------------------------------------------------*/
typedef struct
{
    at91_reg    PIO_PER;                /* PIO Enable Register */
    at91_reg    PIO_PDR;                /* PIO Disable Register */
    at91_reg    PIO_PSR;                /* PIO Status Register */
    at91_reg    Reserved0;
    at91_reg    PIO_OER;                /* Output Enable Register */
    at91_reg    PIO_ODR;                /* Output Disable Register */
    at91_reg    PIO_OSR;                /* Output Status Register */
    at91_reg    Reserved1;
    at91_reg    PIO_IFER;               /* Input Filter Enable Register */
    at91_reg    PIO_IFDR;               /* Input Filter Disable Register */
    at91_reg    PIO_IFSR;               /* Input Filter Status Register */
    at91_reg    Reserved2;
    at91_reg    PIO_SODR;               /* Set Output Data Register */
    at91_reg    PIO_CODR;               /* Clear Output Data Register */
    at91_reg    PIO_ODSR;               /* Output Data Status Register */
    at91_reg    PIO_PDSR;               /* Pin Data Status Register */
    at91_reg    PIO_IER;                /* Interrupt Enable Register */
    at91_reg    PIO_IDR;                /* Interrupt Disable Register */
    at91_reg    PIO_IMR;                /* Interrupt Mask Register */
    at91_reg    PIO_ISR;                /* Interrupt Status Register */
    at91_reg    PIO_MDER;               /* Multi Driver Enable Register */
    at91_reg    PIO_MDDR;               /* Multi Driver Disable Register */
    at91_reg    PIO_MDSR;               /* Multi Driver Status Register */
} StructPIO;

extern AT91S_PIO    PIO;

#define PIO0            (0x01 << 0)     //	
#define PIO1            (0x01 << 1)     // 	
#define PIO2            (0x01 << 2)     // 	
#define PIO3            (0x01 << 3)     //  	
#define PIO4            (0x01 << 4)     //    	
#define PIO5            (0x01 << 5)     // 	
#define PIO6            (0x01 << 6)     //  	
#define PIO7            (0x01 << 7)     //    	
#define PIO8            (0x01 << 8)     // 	
#define PIO9            (0x01 << 9)     // 	
#define PIO10           (0x01 << 10)
#define PIO11           (0x01 << 11)
#define PIO12           (0x01 << 12)
#define PIO13           (0x01 << 13)
#define PIO14           (0x01 << 14)
#define PIO15           (0x01 << 15)
#define PIO16           (0x01 << 16)    //
#define PIO17           (0x01 << 17)    //
#define PIO18           (0x01 << 18)    //
#define PIO19           (0x01 << 19)
#define PIO20           (0x01 << 20)
#define PIO21           (0x01 << 21)
#define PIO22           (0x01 << 22)
#define PIO23           (0x01 << 23)
#define PIO24           (0x01 << 24)
#define PIO25           (0x01 << 25)
#define PIO26           (0x01 << 26)
#define PIO27           (0x01 << 27)
#define PIO28           (0x01 << 28)
#define PIO29           (0x01 << 29)
#define PIO30           (0x01 << 30)
#define PIO31           (0x01 << 31)

//PIOA                  
#define EMAC             (PIO0 | PIO1 | PIO2)
//#define EMAC       (PIO12 | PIO13 | PIO14 | PIO15 | PIO16 | PIO17 | PIO18 | PIO19 \
                        //| PIO20 | PIO21|PIO26  ) //| PIO27 | PIO28

//wangkai_SPI
//#define SPI0            (PIO0 | PIO1 | PIO2 |PIO3)//PA0~PA3
//wangkai_SPI
#define RUNLAMP         PIO2
#define LED_ARUN        PIO2
#define LED_AFAIL       PIO3
#define LED_ACOM        PIO4

//PIOB
#define WDTCS           PIO3           
#define WDTMISO         PIO0
#define WDTMOSI         PIO1
#define WDTSCK          PIO2

#define IOEN  PIO14
#define READY61580  PIO15
#define INCMD PIO16
#define ADDR_LAT PIO17 //缓存模式为输出
#define EXT_TRIG PIO18

///PIOC
#define EBI_PIO         (PIO16 | PIO17 | PIO18 | PIO19 | PIO20 | PIO21 | PIO22 | PIO23 | PIO24 \
                         | PIO25 | PIO26 | PIO27 | PIO28 | PIO29 | PIO30 | PIO31)

#define SMC_CS           PIO11

#define MCTCLR PIO4
#define RTAD0 PIO9
#define RTAD1 PIO8
#define RTAD2 PIO6
#define RTAD3 PIO7
#define RTAD4 PIO5
#define RTADP PIO10
#define INT61580 PIO15

#define B_TO_61580      EXT_TRIG|ADDR_LAT
#define B_FROM_61580  (IOEN|READY|INCMD)
#define C_TO_61580      (MCTCLR|RTAD0|RTAD1|RTAD2|RTAD3|RTAD4|RTADP)
//#define C_FROM_61580  
#define LED_OUTPUT      (LED_ARUN | LED_AFAIL)
#define WDT_OUTPUT      (WDTMOSI| WDTSCK| WDTCS)
#define WDT_INPUT       (WDTMISO)

//#define EMAC_PORT_A     PIO21
//#define EMAC_PORT_B     PIO20    
//#define EMAC_PORT_BOTH  (EMAC_PORT_A | EMAC_PORT_B)
//#define EMAC_SEND_PORT  (EMAC_PORT_A | EMAC_PORT_B)

//#define REC_PORT_EN     PIO10
//#define REC_PORT_ID     PIO11
//#define REC_PORT        (REC_PORT_EN | REC_PORT_ID)

//#define FPGARST         PIO24
#define MAS             PIO22

#define TXD1            PIO6
#define RXD1            PIO7
#define RTS1            PIO28
#define PCK0            PIO30


                         
//QQQ 
#define PIOA_OUTPUT     LED_OUTPUT 
#define PIOA_FUC        EMAC
//wangkai_SPI
#define PIOA_FUC_SPI    SPI0
//wangkai_SPI
//#define PIOA_FUCB       PIO26
#define PIOA_INPUT      ~(PIOA_FUC|EMAC)     //未定义和未使用的IO都定义为输入

#define PIOB_OUTPUT     ( WDT_OUTPUT | B_TO_61580)
//#define PIOB_FUC        (TXD1 | RXD1 | RTS1 | PCK0)   
#define PIOB_INPUT      ~(PIOB_OUTPUT) //(WDT_INPUT | ~PIOB_OUTPUT)     //未定义和未使用的IO都定义为输入

#define PIOC_OUTPUT     C_TO_61580   
#define PIOC_FUCA       (EBI_PIO | SMC_CS)
#define PIOC_FUCB       (PIO15)
#define PIOC_INPUT      ~(PIOC_OUTPUT|PIOC_FUCA|PIOC_FUCB )


#define PIO_HIGH        0x01
#define PIO_LOW         0x00
#define LED_ON(n)       IOCLR(AT91C_BASE_PIOA,n)//IOClr(n)
#define LED_OFF(n)      IOSET(AT91C_BASE_PIOA,n)
#define LED_Change(n)   IOChange(n)

extern void PIOInit(void);
extern void IOClr(Uint32 io);
extern void IOSet(Uint32 io);
extern void IOChange(Uint32 io);

//QQQ useless 
extern Uint8 IOBITStat(Uint32 io);
extern void PioTest(int flag);
extern void delay(Uint32 num) ;//3214544=1s

#endif