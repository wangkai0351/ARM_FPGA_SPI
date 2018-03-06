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
#include "global.h"
#include <board.h>
#include "emac.h"
#include <utility/trace.h>
#include <utility/assert.h>
//#include <string.h>
#include "includes.h"
//#include "cardconfig.h"
//#include "netif.h"
//#include "epa_ethernet.h"
#include "pio.h"
#include "irq.h"

#ifdef __ICCARM__          // IAR
#pragma data_alignment=8   // IAR
#endif                     // IAR
/// Descriptors for RX (required aligned by 8)
// Receive Transfer Descriptor buffer
RxTd rxTd; 

#ifdef __ICCARM__          // IAR
#pragma data_alignment=8   // IAR
#endif                     // IAR
/// Descriptors for TX (required aligned by 8)
// Transmit Transfer Descriptor buffer
TxTd txTd; 

static unsigned char pTxBuffer[TX_BUFFERS * EMAC_TX_UNITSIZE] __attribute__((aligned(8)));

#ifdef __ICCARM__          // IAR
#pragma data_alignment=8   // IAR
#endif                     // IAR
/// Receive Buffer
static unsigned char pRxBuffer[RX_BUFFERS * EMAC_RX_UNITSIZE] __attribute__((aligned(8)));
/// Statistics
EmacStats EmacStatistics;

unsigned char EmacSendTemp[256]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22
	,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62};

//------------------------------------------------------------------------------
//         External variables
//------------------------------------------------------------------------------
extern unsigned char * __pbFrom;
extern unsigned int gPktRcvFlag;
extern void delay(INT32U num);
extern OS_EVENT    *OSBusSem;

//-----------------------------------------------------------------------------
//         Internal functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Wait PHY operation complete.
/// Return 1 if the operation completed successfully.
/// May be need to re-implemented to reduce CPU load.
/// \param retry: the retry times, 0 to wait forever until complete.
//-----------------------------------------------------------------------------
static unsigned char EMAC_WaitPhy( unsigned int retry )
{
    unsigned int retry_count = 0;

    while((AT91C_BASE_EMAC->EMAC_NSR & AT91C_EMAC_IDLE) == 0) {

        // Dead LOOP!
        if (retry == 0) {

            continue;
        }

        // Timeout check
        retry_count++;
        if(retry_count >= retry) {

            //QQQ TRACE_ERROR("E: Wait PHY time out\n\r");
            return 0;
        }
    }

    return 1;
}

//-----------------------------------------------------------------------------
//         Exported functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//          PHY management functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Set MDC clock according to current board clock. Per 802.3, MDC should be 
/// less then 2.5MHz.
/// Return 1 if successfully, 0 if MDC clock not found.
//-----------------------------------------------------------------------------
unsigned char EMAC_SetMdcClock( unsigned int mck )
{
    int clock_dividor;

    if (mck <= 20000000) {
        clock_dividor = AT91C_EMAC_CLK_HCLK_8;          /// MDC clock = MCK/8
    }
    else if (mck <= 40000000) {
        clock_dividor = AT91C_EMAC_CLK_HCLK_16;         /// MDC clock = MCK/16
    }
    else if (mck <= 80000000) {
        clock_dividor = AT91C_EMAC_CLK_HCLK_32;         /// MDC clock = MCK/32
    }
    else if (mck <= 160000000) {
        clock_dividor = AT91C_EMAC_CLK_HCLK_64;         /// MDC clock = MCK/64
    }
    else {
        //QQQ TRACE_ERROR("E: No valid MDC clock.\n\r");
        return 0;
    }
    AT91C_BASE_EMAC->EMAC_NCFGR = (AT91C_BASE_EMAC->EMAC_NCFGR & (~AT91C_EMAC_CLK))
                                 | clock_dividor;
    return 1;
}

//-----------------------------------------------------------------------------
/// Enable MDI with PHY
//-----------------------------------------------------------------------------
void EMAC_EnableMdio( void )
{
    AT91C_BASE_EMAC->EMAC_NCR |= AT91C_EMAC_MPE;
}

//-----------------------------------------------------------------------------
/// Enable MDI with PHY
//-----------------------------------------------------------------------------
void EMAC_DisableMdio( void )
{
    AT91C_BASE_EMAC->EMAC_NCR &= ~AT91C_EMAC_MPE;
}

//-----------------------------------------------------------------------------
/// Enable MII mode for EMAC, called once after autonegotiate
//-----------------------------------------------------------------------------
void EMAC_EnableMII( void )
{
    AT91C_BASE_EMAC->EMAC_USRIO = AT91C_EMAC_CLKEN;
}

//-----------------------------------------------------------------------------
/// Enable RMII mode for EMAC, called once after autonegotiate
//-----------------------------------------------------------------------------
void EMAC_EnableRMII( void )
{
    AT91C_BASE_EMAC->EMAC_USRIO = AT91C_EMAC_CLKEN | AT91C_EMAC_RMII;
}

//-----------------------------------------------------------------------------
/// Read PHY register.
/// Return 1 if successfully, 0 if timeout.
/// \param PhyAddress PHY Address
/// \param Address Register Address
/// \param pValue Pointer to a 32 bit location to store read data
/// \param retry The retry times, 0 to wait forever until complete.
//-----------------------------------------------------------------------------
unsigned char EMAC_ReadPhy(unsigned char PhyAddress,
                           unsigned char Address,
                           unsigned int *pValue,
                           unsigned int retry)
{
    AT91C_BASE_EMAC->EMAC_MAN = (AT91C_EMAC_SOF & (0x01 << 30))
                              | (AT91C_EMAC_CODE & (2 << 16))
                              | (AT91C_EMAC_RW & (2 << 28))
                              | (AT91C_EMAC_PHYA & ((PhyAddress & 0x1f) << 23))
                              | (AT91C_EMAC_REGA & (Address << 18));

    if ( EMAC_WaitPhy(retry) == 0 ) {

        //QQQ TRACE_ERROR("TimeOut EMAC_ReadPhy\n\r");
        return 0;
    }
    *pValue = ( AT91C_BASE_EMAC->EMAC_MAN & 0x0000ffff );
    return 1;
}

//-----------------------------------------------------------------------------
/// Write PHY register
/// Return 1 if successfully, 0 if timeout.
/// \param PhyAddress PHY Address
/// \param Address Register Address
/// \param Value Data to write ( Actually 16 bit data )
/// \param retry The retry times, 0 to wait forever until complete.
//-----------------------------------------------------------------------------
unsigned char EMAC_WritePhy(unsigned char PhyAddress,
                            unsigned char Address,
                            unsigned int  Value,
                            unsigned int  retry)
{
    AT91C_BASE_EMAC->EMAC_MAN = (AT91C_EMAC_SOF & (0x01 << 30))
                              | (AT91C_EMAC_CODE & (2 << 16))
                              | (AT91C_EMAC_RW & (1 << 28))
                              | (AT91C_EMAC_PHYA & ((PhyAddress & 0x1f) << 23))
                              | (AT91C_EMAC_REGA & (Address << 18))
                              | (AT91C_EMAC_DATA & Value) ;      
                              
    if ( EMAC_WaitPhy(retry) == 0 ) {

        //QQQ TRACE_ERROR("TimeOut EMAC_WritePhy\n\r");
        return 0;
    }

    return 1;
}

//-----------------------------------------------------------------------------
/// Setup the EMAC for the link : speed 100M/10M and Full/Half duplex
/// \param speed        Link speed, 0 for 10M, 1 for 100M
/// \param fullduplex   1 for Full Duplex mode
//-----------------------------------------------------------------------------
void EMAC_SetLinkSpeed(unsigned char speed, unsigned char fullduplex)
{
    unsigned int ncfgr;

    ncfgr = AT91C_BASE_EMAC->EMAC_NCFGR;
    ncfgr &= ~(AT91C_EMAC_SPD | AT91C_EMAC_FD);
    if (speed) {

        ncfgr |= AT91C_EMAC_SPD;
    }

    if (fullduplex) {

        ncfgr |= AT91C_EMAC_FD;
    }
    AT91C_BASE_EMAC->EMAC_NCFGR = ncfgr;
}



//-----------------------------------------------------------------------------
//          EMAC functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// EMAC Interrupt handler
//-----------------------------------------------------------------------------
void EMAC_ISR(void)
{
    EmacTxTDescriptor *pTxTd;
    EMAC_TxCallback   *pTxCb;
    unsigned int isr;
    unsigned int rsr;
    unsigned int tsr;
   // unsigned int rxStatusFlag;
    //unsigned int txStatusFlag;
    unsigned int temp ;
    temp= AT91C_BASE_EMAC->EMAC_IMR;
   // isr = AT91C_BASE_EMAC->EMAC_ISR & AT91C_BASE_EMAC->EMAC_IMR;
    isr = AT91C_BASE_EMAC->EMAC_ISR & temp;
    rsr = AT91C_BASE_EMAC->EMAC_RSR;
    tsr = AT91C_BASE_EMAC->EMAC_TSR;

    // RX packet
    if ((isr & AT91C_EMAC_RCOMP) || (rsr & AT91C_EMAC_REC)) 
    {
     //   rxStatusFlag = AT91C_EMAC_REC;

        // Frame received
       // EmacStatistics.rx_packets++;

	if(rxTd.td[rxTd.idx].addr&0x1)
	{

		//pEMAC=(rxTd.td[rxTd.idx].addr&0xfffffffc)+12;
		rxTd.td[rxTd.idx].addr&=0xfffffffe;
		rxTd.idx ++;
		if(rxTd.idx>=RX_BUFFERS)
		rxTd.idx=0;
		//OSSemPost(OSBusSem);
	}
	else
	return;
#if 0
	//rxTd.td[0].addr&=0xfffffffe;
        // Check OVR
        if (rsr & AT91C_EMAC_OVR) 
        {
            //QQQ 溢出处理    
            //rxStatusFlag |= AT91C_EMAC_OVR;
            EmacStatistics.rx_ovrs++;
        }
        // Check BNA
        if (rsr & AT91C_EMAC_BNA) 
        {
            //QQQ 异常处理
           // rxStatusFlag |= AT91C_EMAC_BNA;
            EmacStatistics.rx_bnas++;
        }
        // Clear status
        AT91C_BASE_EMAC->EMAC_RSR = rsr;

        // Invoke callbacks
        /*
        if (rxTd.rxCb) 
        {
            rxTd.rxCb(rxStatusFlag);
        }
        */
        //未设置回调函数，直接调用接收函数
   /*     gPktRcvFlag = 0;
        SAM9EMC.input(&SAM9EMC);
        if(1 == (gPktRcvFlag & 1))          //有数据发送到接收队列，则接收处理任务开始处理。
        {
            OSSemPost(OSPktSem);
        }

        if(2 == (gPktRcvFlag & 2))          //有数据发送到接收队列，则接收处理任务开始处理。
        {
            CSMEPktSemCount++;
            OSSemPost(OSTickSem);
        }*/
#endif        
    }


    // TX packet
    if ((isr & AT91C_EMAC_TCOMP) || (tsr & AT91C_EMAC_COMP)) 
    {

       // txStatusFlag = AT91C_EMAC_COMP;
        EmacStatistics.tx_comp ++;
#if  0       
        if (tsr & AT91C_EMAC_RLES) 
        {
           // txStatusFlag |= AT91C_EMAC_RLES;
            EmacStatistics.tx_errors++;
        }
        // Check COL
        if (tsr & AT91C_EMAC_COL) 
        {
           // txStatusFlag |= AT91C_EMAC_COL;
            EmacStatistics.collisions++;
        }
        // Check BEX
        if (tsr & AT91C_EMAC_BEX) 
        {
            //txStatusFlag |= AT91C_EMAC_BEX;
            EmacStatistics.tx_exausts++;
        }
        // Check UND
        if (tsr & AT91C_EMAC_UND) 
        {
            //txStatusFlag |= AT91C_EMAC_UND;
            EmacStatistics.tx_underruns++;
        }
        // Clear status
        AT91C_BASE_EMAC->EMAC_TSR = tsr;
#endif
        // Sanity check: Tx buffers have to be scheduled
        ASSERT(!CIRC_EMPTY(&txTd),
            "-F- EMAC Tx interrupt received meanwhile no TX buffers has been scheduled\n\r");
        
        // Check the buffers
        while (CIRC_CNT(txTd.head, txTd.tail, TX_BUFFERS)) 
        {
            pTxTd = txTd.td + txTd.tail;
            pTxCb = txTd.txCb + txTd.tail;
            pTxCb = pTxCb;//just for dealing with compiling error

            // Exit if buffer has not been sent yet
            if ((pTxTd->status & (unsigned int)EMAC_TX_USED_BIT) == 0) {
                break;
            }            
            // Notify upper layer that packet has been sent
           // if (*pTxCb) {
             //   (*pTxCb)(txStatusFlag);
            //}
            
            CIRC_INC( txTd.tail, TX_BUFFERS );
        }

        // If a wakeup has been scheduled, notify upper layer that it can send 
        // other packets, send will be successfull.
       /* if( (CIRC_SPACE(txTd.head, txTd.tail, TX_BUFFERS) >= txTd.wakeupThreshold)
         &&  txTd.wakeupCb) 
        {
            txTd.wakeupCb();
        }*/
    }
}

//-----------------------------------------------------------------------------
/// Rest PHY(FPGA)
//-----------------------------------------------------------------------------
/*void PHY_REST(void)
{
    int i;
    IOClr(FPGARST);
    for(i = 0; i < 100000; i++);
    IOSet(FPGARST);
    for(i = 0; i < 100000; i++);
}*/

//-----------------------------------------------------------------------------
/// Initialize PHY83849
//-----------------------------------------------------------------------------
void PHY83849_Init(void)
{
    //int i;
    
    unsigned char phyA;
    unsigned char phyB;

    unsigned char bmcr; 
    unsigned char rbr;
    unsigned char phycr;

    phyA = 0x00;
    phyB = 0x01;

    bmcr = 0x00;
    rbr = 0x17;
    phycr = 0x19;

    //PHY reset
    //EMAC_WritePhy(phyA, rbr, 0x00008000, 0);
    //EMAC_WritePhy(phyB, rbr, 0x00008000, 0);
    //PHY_REST();  lp

    EMAC_SetMdcClock(160000000);
    EMAC_EnableMdio();

   // for(i = 0; i < 200; i++);
    
    //PHY 100 Mbit/s and full duplex
    EMAC_WritePhy(phyA, bmcr, 0x00002100, 0);
    EMAC_WritePhy(phyB, bmcr, 0x00002100, 0);

    //RMII
    EMAC_WritePhy(phyA, rbr, 0x00000030, 0);
    EMAC_WritePhy(phyB, rbr, 0x00000030, 0);

    //LED MOD3
    EMAC_WritePhy(phyA, phycr, 0x00000040, 0);
    EMAC_WritePhy(phyB, phycr, 0x00000041, 0);
  
    EMAC_DisableMdio();
}
//unsigned long lptmp[3];
void PHY83848_Init(void)
{
	//int i;
	//unsigned long temp;
	//PHY reset
	//EMAC_WritePhy(phyA, rbr, 0x00008000, 0);
	//EMAC_WritePhy(phyB, rbr, 0x00008000, 0);
	// PHY_REST(); 
	EMAC_SetMdcClock(160000000);
	EMAC_EnableMdio();

	// for(i = 0; i < 200; i++);
	EMAC_WritePhy(PHY_ADDR, BMCR, 0x8000, 0);//reset //
	delay(10000);
	EMAC_WritePhy(PHY_ADDR, BMCR, 0x1000, 0);
	//PHY 100 Mbit/s and full duplex
	//EMAC_WritePhy(PHY_ADDR, BMCR, 0x3100, 0);//PHY_ADDR, BMCR, 0x2100, 0

	//RMII
	EMAC_WritePhy(PHY_ADDR, RBR, 0x0020, 0); //?
	//EMAC_ReadPhy(PHY_ADDR, PHYSTS, &i, 0);
	//LED MOD3
	EMAC_WritePhy(PHY_ADDR, PHYCR, 0x8041, 0);//PHY_ADDR, PHYCR, 0x0040, 0	
	
	/*start 2014-11-7 modified by cjf */
        //Make sure that no led is on if there's no cable
        EMAC_WritePhy(PHY_ADDR, LEDCR, 0x0024, 0);
    
	//Enable Interrupt and Interrupt Output
	EMAC_WritePhy(PHY_ADDR, MICR, 0x0003, 0);

        //Enable interrupt on change of speed and duplex status
	EMAC_WritePhy(PHY_ADDR, MISR, 0x0038, 0);
        /*end 2014-11-7 modified by cjf*/
	
	//EMAC_ReadPhy(PHY_ADDR, PHYCR, &lptmp[0], 0);
	//EMAC_ReadPhy(PHY_ADDR, RBR, &lptmp[1], 0);
	//EMAC_ReadPhy(PHY_ADDR, BMCR, &lptmp[2], 0);
	//EMAC_ReadPhy(PHY_ADDR, ANAR, &temp, 0);
       // if(temp);
	//EMAC_DisableMdio();
}

void EmacAddrMatchInit(Uint8 *MACAddress)
{
    //Uint8 MACAddress[6]={0x99,0x51,0xc2,0x25,0x62,0x31};

#if 0
#if EMAC_CFG_MAC_ADDR_SEL == EMAC_MAC_ADDR_SEL_CFG
    NetIF_MAC_Addr[0] = 0x00;
    NetIF_MAC_Addr[1] = 0x50;
    NetIF_MAC_Addr[2] = 0xC2;
    NetIF_MAC_Addr[3] = 0x25;
    NetIF_MAC_Addr[4] = 0x60;
    NetIF_MAC_Addr[5] = 0x01;
#endif
#endif

    Mem_Copy((void *)NetIF_MAC_Addr, (void *)MACAddress, (CPU_SIZE_T)sizeof(NetIF_MAC_Addr));

    AT91C_BASE_EMAC->EMAC_SA1L = ((Uint32) MACAddress[3] << 24) | ((Uint32) MACAddress[2] << 16) | ((Uint32) MACAddress[1] << 8) | MACAddress[0];
    AT91C_BASE_EMAC->EMAC_SA1H = ((Uint32) MACAddress[5] << 8) | MACAddress[4];
}

//-----------------------------------------------------------------------------
/// EMAC AutoLink
//-----------------------------------------------------------------------------
void EMAC_AutoLink()
{
    unsigned int read_phy_status;
    unsigned char speed,duplex;

    EMAC_ReadPhy(PHY_ADDR, PHYSTS, &read_phy_status, 0);
    if(read_phy_status & 0x02)
    	{
    		speed = EMAC_TEN_M;
    	}
    else
	{
		speed =EMAC_HUNDRED_M;
	}
	
     if(read_phy_status & 0x04)
     	{
		duplex=EMAC_FULL_DUPLEX;
	 }
	 else
	 {
		duplex = EMAC_HALF_DUPLEX;
	 }
    EMAC_SetLinkSpeed(speed, duplex);
}

//-----------------------------------------------------------------------------
/// Initialize the EMAC with the emac controller address
/// \param id     HW ID for power management
/// \param pTxWakeUpfct Thresold TX Wakeup Callback
/// \param pRxfct       RX Wakeup Callback
/// \param pMacAddress  Mac Address
/// \param enableCAF    enable AT91C_EMAC_CAF if needed by application
/// \param enableNBC    AT91C_EMAC_NBC if needed by application
//-----------------------------------------------------------------------------
void EMAC_Init( unsigned char id, const unsigned char *pMacAddress, 
                unsigned char enableCAF, unsigned char enableNBC )
{
    int Index;
    unsigned int Address;
    //unsigned int read_phy_status;
    //unsigned char speed,duplex;
//    Uint8 MACAddress[6]={0x99,0x51,0xc2,0x25,0x62,0x31};
	Uint8 MACAddress[6]={0x60,0x66,0xcc,0x99,0x99,0x01};

    // Check parameters
    ASSERT(RX_BUFFERS * EMAC_RX_UNITSIZE > EMAC_FRAME_LENTGH_MAX,
           "E: RX buffers too small\n\r");

    //QQQ TRACE_DEBUG("EMAC_Init\n\r");

    // Power ON
    AT91C_BASE_PMC->PMC_PCER = 1 << id;

    //Enable RMII
    EMAC_EnableRMII();

    //PHY init 83849();
    PHY83848_Init();

//EMAC_WritePhy(PHY_ADDR, 0x12, 0x20, 0);
//EMAC_ReadPhy(PHY_ADDR, 0x12, &read_phy_status, 0);
    delay(10000000);
    EMAC_AutoLink();

    // Disable TX & RX and more
    AT91C_BASE_EMAC->EMAC_NCR = 0;

    // disable 
    AT91C_BASE_EMAC->EMAC_IDR = ~0;

 // EMAC_SetLinkSpeed(speed, duplex);
   
    //EMAC 100 Mbit/s and full duplex
   // EMAC_SetLinkSpeed(EMAC_HUNDRED_M, EMAC_FULL_DUPLEX);//EMAC_HUNDRED_M

    rxTd.idx = 0;
    CIRC_CLEAR(&txTd);

    // Setup the RX descriptors.
    for(Index = 0; Index < RX_BUFFERS; Index++) 
    {
        Address = (unsigned int)(&(pRxBuffer[Index * EMAC_RX_UNITSIZE]));
        // Remove EMAC_RX_OWNERSHIP_BIT and EMAC_RX_WRAP_BIT
        rxTd.td[Index].addr = Address & EMAC_ADDRESS_MASK;
        rxTd.td[Index].status = 0;
    }
    rxTd.td[RX_BUFFERS - 1].addr |= EMAC_RX_WRAP_BIT;

    // Setup the TX descriptors.
    for(Index = 0; Index < TX_BUFFERS; Index++) 
    {
        Address = (unsigned int)(&(pTxBuffer[Index * EMAC_TX_UNITSIZE]));
        txTd.td[Index].addr = Address;
        txTd.td[Index].status = (unsigned int)EMAC_TX_USED_BIT;
    }
    txTd.td[TX_BUFFERS - 1].status = EMAC_TX_USED_BIT | EMAC_TX_WRAP_BIT;

    // Set the MAC address，这里需要修改，配置MAC地址
    /****************///EmacAddrMatchInit(DEVICE_PROPERTY.mac_addr);
    
    EmacAddrMatchInit(MACAddress);
 
    // Now setup the descriptors
    // Receive Buffer Queue Pointer Register
    AT91C_BASE_EMAC->EMAC_RBQP = (unsigned int) (rxTd.td);
    // Transmit Buffer Queue Pointer Register
    AT91C_BASE_EMAC->EMAC_TBQP = (unsigned int) (txTd.td);

    AT91C_BASE_EMAC->EMAC_NCR = AT91C_EMAC_CLRSTAT;

    // Clear all status bits in the receive status register.
    AT91C_BASE_EMAC->EMAC_RSR = (AT91C_EMAC_OVR | AT91C_EMAC_REC | AT91C_EMAC_BNA);

    // Clear all status bits in the transmit status register
    AT91C_BASE_EMAC->EMAC_TSR = ( AT91C_EMAC_UBR | AT91C_EMAC_COL | AT91C_EMAC_RLES
                                | AT91C_EMAC_BEX | AT91C_EMAC_COMP
                                | AT91C_EMAC_UND );

    // Clear interrupts
    AT91C_BASE_EMAC->EMAC_ISR;

    // Enable the copy of data into the buffers
    // enable broadcasts, and don't copy FCS.
    AT91C_BASE_EMAC->EMAC_NCFGR |= (AT91C_EMAC_DRFCS | AT91C_EMAC_PAE);

	if( enableCAF == EMAC_CAF_ENABLE ) 
	{
	    AT91C_BASE_EMAC->EMAC_NCFGR |= AT91C_EMAC_CAF;
	}
	else
	{
		AT91C_BASE_EMAC->EMAC_NCFGR &=~ AT91C_EMAC_CAF;
	}

	if( enableNBC == EMAC_NBC_ENABLE ) 
	{
	    AT91C_BASE_EMAC->EMAC_NCFGR |= AT91C_EMAC_NBC;
	}
	else
	{
		AT91C_BASE_EMAC->EMAC_NCFGR &=~ AT91C_EMAC_NBC;
	}	
	//AT91C_BASE_EMAC->EMAC_NCFGR |= AT91C_EMAC_CAF;
//AT91C_BASE_EMAC->EMAC_NCFGR |= AT91C_EMAC_CAF|AT91C_EMAC_IRXFCS;
    // Enable Rx and Tx, plus the stats register.
    AT91C_BASE_EMAC->EMAC_NCR |= (AT91C_EMAC_TE | AT91C_EMAC_RE | AT91C_EMAC_WESTAT);

    // Setup the interrupts for TX and RX
    AT91C_BASE_EMAC->EMAC_IER = AT91C_EMAC_RCOMP
                              | AT91C_EMAC_RXUBR
                              | AT91C_EMAC_TCOMP;

    EMAC_EnableMdio();
    
}

//-----------------------------------------------------------------------------
/// Get the statstic information & reset it
/// \param pStats   Pointer to EmacStats structure to copy the informations
/// \param reset    Reset the statistics after copy it
//-----------------------------------------------------------------------------
void EMAC_GetStatistics(EmacStats *pStats, unsigned char reset)
{
    unsigned int ncrBackup = 0;

    //QQQ TRACE_DEBUG("EMAC_GetStatistics\n\r");

    // Sanity check
    if (pStats == (EmacStats *) 0) {
        return;
    }
    
    ncrBackup = AT91C_BASE_EMAC->EMAC_NCR & (AT91C_EMAC_TE | AT91C_EMAC_RE);

    // Disable TX/RX
    AT91C_BASE_EMAC->EMAC_NCR = ncrBackup & ~(AT91C_EMAC_TE | AT91C_EMAC_RE);

    // Copy the informations
    memcpy(pStats, (void*)&EmacStatistics, sizeof(EmacStats));

    // Reset the statistics
    if (reset) {
        memset((void*)&EmacStatistics, 0x00, sizeof(EmacStats));
        AT91C_BASE_EMAC->EMAC_NCR = ncrBackup | AT91C_EMAC_CLRSTAT;
    }

    // restore NCR
    AT91C_BASE_EMAC->EMAC_NCR = ncrBackup;
}

//-----------------------------------------------------------------------------
/// Send a packet with EMAC.
/// If the packet size is larger than transfer buffer size error returned.
/// \param buffer   The buffer to be send
/// \param size     The size of buffer to be send
/// \param fEMAC_TxCallback Threshold Wakeup callback
/// \param fWakeUpCb   TX Wakeup
/// \return         OK, Busy or invalid packet
//-----------------------------------------------------------------------------
/*start 2013-12-20 modified by xjx, */
void EMAC_Send(void *pBuffer, 
                        unsigned int size, 
                        NET_ERR     *perr)
{
    volatile EmacTxTDescriptor *pTxTd;
//    volatile EMAC_TxCallback   *pTxCb;

    //TRACE_DEBUG("EMAC_Send\n\r");
    *perr = NET_NIC_ERR_NONE;
    // Check parameter
    if (size > EMAC_TX_UNITSIZE) {

        //QQQ TRACE_ERROR("EMAC driver does not split send packets.");
        //QQQ TRACE_ERROR(" It can send %d bytes max in one packet (%d bytes requested)\n\r",
        //QQQ    EMAC_TX_UNITSIZE, size);
        *perr = EMAC_TX_INVALID_PACKET;
        return;
    }

    // If no free TxTd, buffer can't be sent, schedule the wakeup callback
    if( CIRC_SPACE(txTd.head, txTd.tail, TX_BUFFERS) == 0) {
        *perr = EMAC_TX_BUFFER_BUSY;
        return;

    }

    // Pointers to the current TxTd
    pTxTd = txTd.td + txTd.head;
    //pTxCb = txTd.txCb + txTd.head;

    // Sanity check
    ASSERT((pTxTd->status & EMAC_TX_USED_BIT) != 0, 
        "-F- Buffer is still under EMAC control\n\r");

    // Setup/Copy data to transmition buffer
    if (pBuffer && size) {
        // Driver manage the ring buffer
        memcpy((void *)pTxTd->addr, pBuffer, size);
    }
    
    // Tx Callback
//    *pTxCb = fEMAC_TxCallback;

    // Update TD status
    // The buffer size defined is length of ethernet frame
    // so it's always the last buffer of the frame.
    if (txTd.head == TX_BUFFERS-1) {
        pTxTd->status = 
            (size & EMAC_LENGTH_FRAME) | EMAC_TX_LAST_BUFFER_BIT | EMAC_TX_WRAP_BIT;
    }
    else {
        pTxTd->status = (size & EMAC_LENGTH_FRAME) | EMAC_TX_LAST_BUFFER_BIT;
    }
    
    CIRC_INC(txTd.head, TX_BUFFERS)

    // Tx packets count
    EmacStatistics.tx_packets++;

    // Now start to transmit if it is not already done
    AT91C_BASE_EMAC->EMAC_NCR |= AT91C_EMAC_TSTART;
}
/*end 2013-12-20 modified by xjx*/



extern void  NetNIC_RxISR_Handler (void);
//-----------------------------------------------------------------------------
/// Receive a packet with EMAC
/// If not enough buffer for the packet, the remaining data is lost but right
/// frame length is returned.
/// \param pFrame           Buffer to store the frame
/// \param frameSize        Size of the frame
/// \param pRcvSize         Received size
/// \return                 OK, no data, or frame too small
//-----------------------------------------------------------------------------
unsigned char EMAC_Poll(unsigned char *pFrame,
                        unsigned int frameSize,
                        CPU_INT16U *pRcvSize)
{
    unsigned short bufferLength;
    unsigned int   tmpFrameSize=0;
    unsigned char  *pTmpFrame=0;
    unsigned int   tmpIdx = rxTd.idx;
    volatile EmacRxTDescriptor *pRxTd = rxTd.td + rxTd.idx;

//QQQ    ASSERT(pFrame, "F: EMAC_Poll\n\r");

    char isFrame = 0;
    // Set the default return value
    *pRcvSize = 0;

    // Process received RxTd
    while ((pRxTd->addr & EMAC_RX_OWNERSHIP_BIT) == EMAC_RX_OWNERSHIP_BIT) {

        // A start of frame has been received, discard previous fragments
        if ((pRxTd->status & EMAC_RX_SOF_BIT) == EMAC_RX_SOF_BIT) {
            // Skip previous fragment
            while (tmpIdx != rxTd.idx) {
                pRxTd = rxTd.td + rxTd.idx;
                pRxTd->addr &= ~(EMAC_RX_OWNERSHIP_BIT);
                CIRC_INC(rxTd.idx, RX_BUFFERS);
            }
            // Reset the temporary frame pointer
            pTmpFrame = pFrame;
            tmpFrameSize = 0;
            // Start to gather buffers in a frame
            isFrame = 1;
        }

        // Increment the pointer
        CIRC_INC(tmpIdx, RX_BUFFERS);

        // Copy data in the frame buffer
        if (isFrame) {
            if (tmpIdx == rxTd.idx) {
//QQQ                TRACE_INFO("no EOF (Invalid of buffers too small)\n\r");

                do {

                    pRxTd = rxTd.td + rxTd.idx;
                    pRxTd->addr &= ~(EMAC_RX_OWNERSHIP_BIT);
                    CIRC_INC(rxTd.idx, RX_BUFFERS);
                } while(tmpIdx != rxTd.idx);
                return EMAC_RX_NO_DATA;
            }
            // Copy the buffer into the application frame
            bufferLength = EMAC_RX_UNITSIZE;
            if ((tmpFrameSize + bufferLength) > frameSize) {
                bufferLength = frameSize - tmpFrameSize;
            }

            memcpy(pTmpFrame, (void*)(pRxTd->addr & EMAC_ADDRESS_MASK), bufferLength);
            pTmpFrame += bufferLength;
            tmpFrameSize += bufferLength;
            
            // An end of frame has been received, return the data
            if ((pRxTd->status & EMAC_RX_EOF_BIT) == EMAC_RX_EOF_BIT) {
                // Frame size from the EMAC
                *pRcvSize = (pRxTd->status & EMAC_LENGTH_FRAME);
                
                // Application frame buffer is too small all data have not been copied
                if (tmpFrameSize < *pRcvSize) {
                    printf("size req %d size allocated %d\n\r", *pRcvSize, frameSize);
                    
                    return EMAC_RX_FRAME_SIZE_TOO_SMALL;
                }
 
                // TRACE_DEBUG("packet %d-%d (%d)\n\r", rxTd.idx, tmpIdx, *pRcvSize);
                // All data have been copied in the application frame buffer => release TD
                while (rxTd.idx != tmpIdx) {
                    pRxTd = rxTd.td + rxTd.idx;
                    pRxTd->addr &= ~(EMAC_RX_OWNERSHIP_BIT);
                    CIRC_INC(rxTd.idx, RX_BUFFERS);
                }
                EmacStatistics.rx_packets++;
                NetNIC_RxISR_Handler();
                return EMAC_RX_OK;
            }
        }
        
        // SOF has not been detected, skip the fragment
        else {
           pRxTd->addr &= ~(EMAC_RX_OWNERSHIP_BIT);
           rxTd.idx = tmpIdx;
        }
       
        // Process the next buffer
        pRxTd = rxTd.td + tmpIdx;
    }
    
    //TRACE_DEBUG("E");
    return EMAC_RX_NO_DATA;
}

//-----------------------------------------------------------------------------
/// Registers pRxCb callback. Callback will be invoked after the next received
/// frame.
/// When EMAC_Poll() returns EMAC_RX_NO_DATA the application task call EMAC_Set_RxCb()
/// to register pRxCb() callback and enters suspend state. The callback is in charge 
/// to resume the task once a new frame has been received. The next time EMAC_Poll()
/// is called, it will be successfull.
/// \param pRxCb            Pointer to callback function
//-----------------------------------------------------------------------------
void EMAC_Set_RxCb(EMAC_RxCallback pRxCb)
{
    rxTd.rxCb = pRxCb;
    AT91C_BASE_EMAC->EMAC_IER = AT91C_EMAC_RCOMP; 
}

//-----------------------------------------------------------------------------
/// Remove the RX callback function.
/// This function is usually invoked from the RX callback itself. Once the callback 
/// has resumed the application task, there is no need to invoke the callback again.
//-----------------------------------------------------------------------------
void EMAC_Clear_RxCb(void)
{
    AT91C_BASE_EMAC->EMAC_IDR = AT91C_EMAC_RCOMP; 
    rxTd.rxCb = (EMAC_RxCallback) 0;
}

//-----------------------------------------------------------------------------
/// Registers TX wakeup callback callback. Callback will be invoked once several
/// transfer descriptors are available.
/// When EMAC_Send() returns EMAC_TX_BUFFER_BUSY (all TD busy) the application 
/// task calls EMAC_Set_TxWakeUpCb() to register pTxWakeUpCb() callback and 
/// enters suspend state. The callback is in charge to resume the task once 
/// several TD have been released. The next time EMAC_Send() will be called, it
/// shall be successfull.
/// \param pTxWakeUpCb   Pointer to callback function
/// \param threshold     Minimum number of available transfer descriptors before pTxWakeUpCb() is invoked
/// \return              0= success, 1 = threshold exceeds nuber of transfer descriptors
//-----------------------------------------------------------------------------
char EMAC_Set_TxWakeUpCb(EMAC_WakeupCallback pTxWakeUpCb, unsigned short threshold)
{
    if (threshold <= TX_BUFFERS) {
        txTd.wakeupCb = pTxWakeUpCb;
        txTd.wakeupThreshold = threshold;
        return 0;
    }
    return 1;
}

//-----------------------------------------------------------------------------
/// Remove the TX wakeup callback function.
/// This function is usually invoked from the TX wakeup callback itself. Once the callback 
/// has resumed the application task, there is no need to invoke the callback again.
//-----------------------------------------------------------------------------
void EMAC_Clear_TxWakeUpCb(void)
{
    txTd.wakeupCb = (EMAC_WakeupCallback) 0;
}


    
//*------------------------------------------------------------------------------------------------
//* 函数名称 : GetInputPacketLen
//* 功能描述 : 获取到达的信息包的长度
//* 入口参数 : 无
//* 出口参数 : 无
//*------------------------------------------------------------------------------------------------
INT16U GetInputPacketLen(void)
{
    INT16U  __u16Idx, __u16Len = 0;
    //unsigned int   tmpIdx = rxTd.idx;
    EmacRxTDescriptor *pRxTd = rxTd.td + rxTd.idx;

    //* 跳过碎片帧，判断依据是帧头位未被置位
    while((pRxTd->status & EMAC_RX_SOF_BIT) != EMAC_RX_SOF_BIT
            && (pRxTd->addr & EMAC_RX_OWNERSHIP_BIT))
    {
        //* 释放这个缓冲区
        pRxTd->addr &= ~(EMAC_RX_OWNERSHIP_BIT);
        CIRC_INC(rxTd.idx, RX_BUFFERS);
        pRxTd = rxTd.td + rxTd.idx;
    }

    __u16Idx = rxTd.idx;

    //* 只有最后一个缓冲区才保存帧的长度，其它均为0
    while((pRxTd->addr & EMAC_RX_OWNERSHIP_BIT) == EMAC_RX_OWNERSHIP_BIT)
    {
        __u16Len = (pRxTd->status & EMAC_LENGTH_FRAME);
        if(__u16Len > 0) break;

        __u16Idx++;
        if(__u16Idx >= RX_BUFFERS) __u16Idx = 0;
        pRxTd = rxTd.td + __u16Idx;
    }

    //* 保存信息包读取位置
    pRxTd = rxTd.td + rxTd.idx;
    __pbFrom = (INT8U *) (void*)(pRxTd->addr & EMAC_ADDRESS_MASK);

    return __u16Len;
}


