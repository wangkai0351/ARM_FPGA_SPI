/*
*********************************************************************************************************
*                                              uC/TCP-IP
*                                      The Embedded TCP/IP Suite
*
*                          (c) Copyright 2003-2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/TCP-IP is provided in source form for FREE evaluation, for educational
*               use or peaceful research.  If you plan on using uC/TCP-IP in a commercial
*               product you need to contact Micrium to properly license its use in your
*               product.  We provide ALL the source code for your convenience and to help
*               you experience uC/TCP-IP.  The fact that the source code is provided does
*               NOT mean that you can use it without paying a licensing fee.
*
*               Network Interface Card (NIC) port files provided, as is, for FREE and do
*               NOT require any additional licensing or licensing fee.
*
*               Knowledge of the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        NETWORK INTERFACE CARD
*
*                                           AT91RM9200 EMAC
*
* Filename      : net_nic.c
* Version       : V1.91
* Programmer(s) : JDH
*********************************************************************************************************
* Note(s)       : (1) Supports EMAC section of Atmel's AT91RM9200 microcontroller as described in
*
*                         Atmel Corporation (ATMEL; http://www.atmel.com).
*                         (a) AT91RM9200    (ATMEL AT91RM9200; Revision 1768B-ATARM-08/03).
*
*                 (2) REQUIREs Ethernet Network Interface Layer located in the following network directory :
*
*                         \<Network Protocol Suite>\IF\Ether\
*
*                              where
*                                      <Network Protocol Suite>    directory path for network protocol suite
*
*                 (3) Since the AT91RM9200 EMAC (NIC) module is integrated into the AT91RM9200 microcontroller, 
*                     the endianness of the registers is the same as the CPU, which is little-endian by default.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_NIC_MODULE
#include  <net.h>
#include "emac.h"
#include "board.h"


/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                              DATA TYPES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/






/*
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void        NetNIC_RxISR_Handler          (void);               /* ISR for RX interrupts.                              */
static  void        NetNIC_TxISR_Handler          (void);               /* ISR for TX interrupts.                              */
static  void        NetNIC_TxPktDiscard           (NET_ERR     *perr);




/*
*********************************************************************************************************
*                                             NetNIC_Init()
*
* Description : (1) Initialize Network Interface Card :
*                   (a) Perform NIC Layer OS initialization
*                   (b) Initialize NIC status
*                   (c) Initialize NIC statistics & error counters
*                   (d) Initialize AT91RM9200
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Net_Init().
*********************************************************************************************************
*/

void  NetNIC_Init (NET_ERR  *perr)
{
                                                                        /* --------------- PERFORM NIC/OS INIT --------------- */
    NetOS_NIC_Init(perr);                                               /* Create NIC objs.                                    */
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

    /*XJX 2013-12-26*/                                                                    /* ----------------- INIT NIC STATUS ----------------- */
    NetNIC_ConnStatus           =  DEF_ON;

                                                                        /* ------------- INIT NIC STAT & ERR CTRS ------------ */
#if (NET_CTR_CFG_STAT_EN        == DEF_ENABLED)
    NetNIC_StatRxPktCtr         =  0;
    NetNIC_StatTxPktCtr         =  0;
#endif

#if (NET_CTR_CFG_ERR_EN         == DEF_ENABLED)
    NetNIC_ErrRxPktDiscardedCtr =  0;
    NetNIC_ErrTxPktDiscardedCtr =  0;
#endif

   *perr = NET_NIC_ERR_NONE;
}


/*
*********************************************************************************************************
*                                            NetNIC_IntEn()
*
* Description : Enable NIC interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Net_Init().
*********************************************************************************************************
*/

void  NetNIC_IntEn (NET_ERR  *perr)
{
    //AT91RM9200_EMAC_RxIntEn();
    //AT91RM9200_EMAC_TxIntEn();

    *perr = NET_NIC_ERR_NONE;
}


/*
*********************************************************************************************************
*                                        NetNIC_ConnStatusGet()
*
* Description : Get NIC's network connection status.
*
*               (1) Obtaining the NIC's network connection status is encapsulated in this function for the
*                   possibility that obtaining a NIC's connection status requires a non-trivial procedure.
*
*
* Argument(s) : none.
*
* Return(s)   : NIC network connection status :
*
*                   DEF_OFF     Network connection DOWN.
*                   DEF_ON      Network connection UP.
*
* Caller(s)   : NetIF_Pkt_Tx().
*********************************************************************************************************
*/

CPU_BOOLEAN  NetNIC_ConnStatusGet (void)
{
    return (NetNIC_ConnStatus);
}


/*
*********************************************************************************************************
*                                         NetNIC_ISR_Handler()
*
* Description : (1) Decode ISR & call appropriate ISR handler :
*
*                   (a) AT91RM9200 Receive Buffer Not Available ISR     NetNIC_RxPktDiscard().
*                   (b) AT91RM9200 Receive  ISR                         NetNIC_RxISR_Handler().
*                   (c) AT91RM9200 Transmit ISR                         NetNIC_TxISR_Handler().
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_ISR() [see Note #2].
*
* Note(s)     : (2) AT91RM9200 ISR  MUST call NetNIC_ISR_Handler() & MUST be developer-implemented in
*
*                       \<Your Product Application>\net_isr*.*
*
*                           where
*                                   <Your Product Application>    directory path for Your Product's Application.
*
*               (3) This function clears the interrupt source(s) on an external interrupt controller &, if
*                   ENABLED, MUST be developer-implemented in
*
*                       \<Your Product Application>\net_isr.c
*
*                           where
*                                   <Your Product Application>    directory path for Your Product's Application.
*********************************************************************************************************
*/

/*start 2013-12-20 modified by xjx, */
static void  NetNIC_ISR_Handler (void) //@".firmware_update"
{
    volatile EmacTxTDescriptor *pTxTd;
    volatile EMAC_TxCallback   *pTxCb;
    unsigned int isr;
    unsigned int rsr;
    unsigned int tsr;
    unsigned int rxStatusFlag;
    unsigned int txStatusFlag;

//#if OS_CRITICAL_METHOD == 3 /* Allocate storage for CPU status register      */
//    OS_CPU_SR   cpu_sr;
//#endif
    //OS_ENTER_CRITICAL();

    isr = AT91C_BASE_EMAC->EMAC_ISR & AT91C_BASE_EMAC->EMAC_IMR;
    rsr = AT91C_BASE_EMAC->EMAC_RSR;
    tsr = AT91C_BASE_EMAC->EMAC_TSR;

    // RX packet
    if ((isr & AT91C_EMAC_RCOMP) || (rsr & AT91C_EMAC_REC)) 
    {
        rxStatusFlag = AT91C_EMAC_REC;

        // Frame received
        EmacStatistics.rx_packets++;

        // Check OVR
        if (rsr & AT91C_EMAC_OVR) 
        {
            //QQQ Òç³ö´¦Àí    
            rxStatusFlag |= AT91C_EMAC_OVR;
            EmacStatistics.rx_ovrs++;
        }
        // Check BNA
        if (rsr & AT91C_EMAC_BNA) 
        {
            //QQQ Òì³£´¦Àí
            rxStatusFlag |= AT91C_EMAC_BNA;
            EmacStatistics.rx_bnas++;
        }
        // Clear status
        AT91C_BASE_EMAC->EMAC_RSR = rxStatusFlag;

        // Invoke callbacks

        if (rxTd.rxCb) 
        {
            rxTd.rxCb(rxStatusFlag);
        }

        NetNIC_RxISR_Handler();
    }


    // TX packet
    if ((isr & AT91C_EMAC_TCOMP) || (tsr & AT91C_EMAC_COMP)) 
    {

        txStatusFlag = AT91C_EMAC_COMP;
        EmacStatistics.tx_comp ++;

        if (tsr & AT91C_EMAC_RLES) 
        {
            txStatusFlag |= AT91C_EMAC_RLES;
            EmacStatistics.tx_errors++;
        }
        // Check COL
        if (tsr & AT91C_EMAC_COL) 
        {
            txStatusFlag |= AT91C_EMAC_COL;
            EmacStatistics.collisions++;
        }
        // Check BEX
        if (tsr & AT91C_EMAC_BEX) 
        {
            txStatusFlag |= AT91C_EMAC_BEX;
            EmacStatistics.tx_exausts++;
        }
        // Check UND
        if (tsr & AT91C_EMAC_UND) 
        {
            txStatusFlag |= AT91C_EMAC_UND;
            EmacStatistics.tx_underruns++;
        }
        // Clear status
        AT91C_BASE_EMAC->EMAC_TSR = txStatusFlag;

        // Sanity check: Tx buffers have to be scheduled
        //ASSERT(!CIRC_EMPTY(&txTd),
        //    "-F- EMAC Tx interrupt received meanwhile no TX buffers has been scheduled\n\r");
        
        // Check the buffers
        while (CIRC_CNT(txTd.head, txTd.tail, TX_BUFFERS)) 
        {
            pTxTd = txTd.td + txTd.tail;
            pTxCb = txTd.txCb + txTd.tail;

            // Exit if buffer has not been sent yet
            if ((pTxTd->status & (unsigned int)EMAC_TX_USED_BIT) == 0) {
                break;
            }            
            // Notify upper layer that packet has been sent
            if (*pTxCb) {
                (*pTxCb)(txStatusFlag);
            }
/*start 2013-12-25 modified by xjx, */
            NetNIC_TxISR_Handler();
/*end 2013-12-25 modified by xjx, */
            
            CIRC_INC( txTd.tail, TX_BUFFERS );
        }

        // If a wakeup has been scheduled, notify upper layer that it can send 
        // other packets, send will be successfull.
        if( (CIRC_SPACE(txTd.head, txTd.tail, TX_BUFFERS) >= txTd.wakeupThreshold)
         &&  txTd.wakeupCb) 
        {
            txTd.wakeupCb();
        }
    }

    //OS_EXIT_CRITICAL();

}
/*end 2013-12-20 modified by xjx*/


void NetNIC_Setup_Int (void)
{

    BSP_IntClr(BSP_PER_ID_EMAC);

    BSP_IntVectSet((CPU_INT08U   )BSP_PER_ID_EMAC,
                   (CPU_INT08U   )0x5,
                   (CPU_INT08U   )BSP_INT_SCR_TYPE_INT_HIGH_LEVEL_SENSITIVE,
                   (CPU_FNCT_VOID)NetNIC_ISR_Handler );

    BSP_IntEn(BSP_PER_ID_EMAC);



}



/*
*********************************************************************************************************
*                                         NetNIC_RxPktGetSize()
*
* Description : Get network packet size from NIC.
*
* Argument(s) : none.
*
* Return(s)   : Size, in octets, of NIC's next network packet.
*
* Caller(s)   : NetIF_RxTaskHandler().
*********************************************************************************************************
*/

CPU_INT16U  NetNIC_RxPktGetSize (void)
{
    CPU_INT16U  size = 0;


//    size = AT91RM9200_EMAC_RxPktGetSize();
    return (size);
}


/*
*********************************************************************************************************
*                                            NetNIC_RxPkt()
*
* Description : Read network packet from NIC into buffer.
*
* Argument(s) : ppkt        Pointer to memory buffer to receive NIC packet.
*
*               size        Number of packet frame octets to read into buffer.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE            Packet successfully read.
*                               NET_ERR_INIT_INCOMPLETE     Network initialization NOT complete.
*                               NET_NIC_ERR_NULL_PTR        Argument 'ppkt' passed a NULL pointer.
*                               NET_NIC_ERR_INVALID_SIZE    Invalid size.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxTaskHandler().
*
* Note(s)     : (1) NetNIC_RxPkt() blocked until network initialization completes; perform NO action.
*********************************************************************************************************
*/
/*start 2013-12-20 modified by xjx, */
CPU_INT16U  NetNIC_RxPkt (void        *ppkt,
                    CPU_INT16U   size,
                    NET_ERR     *perr)
/*end 2013-12-20 modified by xjx*/
{
#if (NET_CTR_CFG_STAT_EN     == DEF_ENABLED)
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR   cpu_sr;
#endif
#endif
    /*start 2013-12-20 modified by xjx, */
    CPU_INT16U    rev_size;
    /*end 2013-12-20 modified by xjx*/
    if (Net_InitDone != DEF_YES) {                                      /* If init NOT complete, exit rx (see Note #2).        */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return 0;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                         /* ------------------- VALIDATE PTR ------------------ */
    if (ppkt == (void *)0) {
       *perr = NET_NIC_ERR_NULL_PTR;
        return 0;
    }

                                                                        /* ------------------- VALIDATE SIZE ----------------- */
    if (size < NET_IF_FRAME_MIN_SIZE) {
       *perr = NET_NIC_ERR_INVALID_SIZE;
        return 0;
    }
#endif

//    AT91RM9200_EMAC_RxPkt(ppkt, size, perr);                            /* Rd rx pkt from NIC.                                 */
    /*start 2013-12-20 modified by xjx, */
    if ( EMAC_RX_OK != EMAC_Poll(ppkt, size, &rev_size))
    {
        rev_size = 0;
        *perr = NET_NIC_ERR_INVALID_SIZE;
        return 0;
    }

    /*end 2013-12-20 modified by xjx*/

    NET_CTR_STAT_INC(NetNIC_StatRxPktCtr);

    *perr = NET_NIC_ERR_NONE;

    return rev_size;
}


/*
*********************************************************************************************************
*                                         NetNIC_RxPktDiscard()
*
* Description : Discard network packet from NIC to free NIC packet frames for new receive packets.
*
* Argument(s) : size        Number of packet frame octets.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE            Packet successfully discarded.
*                               NET_ERR_INIT_INCOMPLETE     Network initialization NOT complete.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxTaskHandler().
*
* Note(s)     : (1) NetNIC_RxPktDiscard() blocked until network initialization completes; perform NO action.
*
*               (2) #### 'perr' may NOT be necessary (remove before product release if unnecessary).
*********************************************************************************************************
*/

void  NetNIC_RxPktDiscard (CPU_INT16U   size,
                           NET_ERR     *perr)
{
#if (NET_CTR_CFG_ERR_EN      == DEF_ENABLED)
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR   cpu_sr;
#endif
#endif


    if (Net_InitDone != DEF_YES) {                                      /* If init NOT complete, exit discard (see Note #1).   */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }

//    AT91RM9200_EMAC_RxPktDiscard(size);

    NET_CTR_ERR_INC(NetNIC_ErrRxPktDiscardedCtr);

   *perr = NET_NIC_ERR_NONE;
}


/*
*********************************************************************************************************
*                                         NetNIC_TxPktPrepare()
*
* Description : Prepare to transmit data packets from network driver layer to network interface card.
*
* Argument(s) : ppkt        Pointer to memory buffer to transmit NIC packet.
*
*               size        Number of packet frame octets to write to frame.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE            Packet successfully transmitted.
*                               NET_ERR_INIT_INCOMPLETE     Network initialization NOT complete.
*
*                                                           - RETURNED BY NetNIC_TxPktDiscard() : -
*                               NET_ERR_TX                  Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Pkt_Tx().
*
* Note(s)     : (1) NetNIC_TxPktPrepare() blocked until network initialization completes; perform NO action.
*********************************************************************************************************
*/

void  NetNIC_TxPktPrepare (void        *ppkt,
                           CPU_INT16U   size,
                           NET_ERR     *perr)
{
    if (Net_InitDone != DEF_YES) {                                      /* If init NOT complete, exit tx (see Note #1).        */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                         /* ------------------- VALIDATE PTR ------------------ */
    if (ppkt == (void *)0) {
        NetNIC_TxPktDiscard(perr);
        return;
    }
#endif

//    AT91RM9200_EMAC_TxPktPrepare(ppkt, size, perr);                     /* Preparte to tx pkt to AT91RM9200.                   */

//    if (*perr != NET_NIC_ERR_NONE) {
//        NetNIC_TxPktDiscard(perr);
//        return;
//    }

    *perr = NET_NIC_ERR_NONE;
}


/*
*********************************************************************************************************
*                                            NetNIC_TxPkt()
*
* Description : Transmit data packets from network driver layer to network interface card.
*
* Argument(s) : ppkt        Pointer to memory buffer to transmit NIC packet.
*
*               size        Number of packet frame octets to write to frame.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE            Packet successfully transmitted.
*                               NET_ERR_INIT_INCOMPLETE     Network initialization NOT complete.
*
*                                                           - RETURNED BY NetNIC_TxPktDiscard() : -
*                               NET_ERR_TX                  Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Pkt_Tx().
*
* Note(s)     : (1) NetNIC_TxPkt() blocked until network initialization completes; perform NO action.
*********************************************************************************************************
*/

void  NetNIC_TxPkt (void        *ppkt,
                    CPU_INT16U   size,
                    NET_ERR     *perr)
{
#if (NET_CTR_CFG_STAT_EN     == DEF_ENABLED)
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR   cpu_sr;
#endif
#endif


    if (Net_InitDone != DEF_YES) {                                      /* If init NOT complete, exit tx (see Note #1).        */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                         /* ------------------- VALIDATE PTR ------------------ */
    if (ppkt == (void *)0) {
        NetNIC_TxPktDiscard(perr);
        return;
    }
#endif

//    AT91RM9200_EMAC_TxPkt(ppkt, size, perr);                            /* Tx pkt to AT91RM9200.                               */
    /*start 2013-12-20 modified by xjx, */
    EMAC_Send(ppkt, size, perr);
    /*end 2013-12-20 modified by xjx*/


    if (*perr != NET_NIC_ERR_NONE) {
        NetNIC_TxPktDiscard(perr);
        return;
    }

    NET_CTR_STAT_INC(NetNIC_StatTxPktCtr);

    *perr = NET_NIC_ERR_NONE;
}


/*
*********************************************************************************************************
*                                           NetNIC_PhyRegRd()
*
* Description : Read PHY data value.
*
* Argument(s) : phy         PHY address, normally 0.
*               reg         PHY register.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE                Physical register successfully read.
*
* Return(s)   : Data read from PHY register.
*
* Caller(s)   : NetNIC_PhyInit(),
*               NetNIC_PhyIsLinkUp().
*
* Note(s)     : Register ALWAYS reads 16-bit data values.
*********************************************************************************************************
*/

CPU_INT16U  NetNIC_PhyRegRd (CPU_INT08U   phy,
                             CPU_INT08U   reg,
                             NET_ERR     *perr)
{
    CPU_INT16U  reg_val = 0;


//    reg_val = AT91RM9200_EMAC_PhyRegRd(phy, reg);
   *perr    = NET_NIC_ERR_NONE;

    return (reg_val);
}


/*
*********************************************************************************************************
*                                           NetNIC_PhyRegWr()
*
* Description : Write PHY data value.
*
* Argument(s) : phy         PHY address, normally 0.
*               reg         PHY register.
*               val         Data to write to PHY register.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE                Physical register successfully written.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_PhyInit(),
*               NetNIC_PhyIsLinkUp().
*
* Note(s)     : Register ALWAYS writes 16-bit data values.
*********************************************************************************************************
*/

void  NetNIC_PhyRegWr (CPU_INT08U   phy,
                       CPU_INT08U   reg,
                       CPU_INT16U   val,
                       NET_ERR     *perr)
{
//    AT91RM9200_EMAC_PhyRegWr(phy, reg, val);
   *perr = NET_NIC_ERR_NONE;
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        NetNIC_RxISR_Handler()
*
* Description : Signal Network Interface Receive Task that a receive packet is available.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_ISR_Handler().
*
* Note(s)     : (1) NetNIC_ISR_Handler() decodes AT91RM9200 Receive ISR & calls NetNIC_RxISR_Handler().
*********************************************************************************************************
*/

void  NetNIC_RxISR_Handler (void)
{
    NET_ERR  err;


                                                                        /* Interrupts are acknowledged when ISR read.          */
                                                                        /* See 'ATMEL AT91RM9200; Revision 1768B-ATARM–08/03'  */
                                                                        /* p. 586.                                             */
                                                                        /* ISR are previously read in NetNIC_ISR_Handler().    */

                                                                        /* ------------- POST RX COMPLETE SIGNAL ------------- */
    NetOS_IF_RxTaskSignal(&err);                                        /* Signal Net IF Rx Task of NIC rx pkt.                */
}


/*
*********************************************************************************************************
*                                        NetNIC_TxISR_Handler()
*
* Description : (1) Clear transmit interrupt &/or transmit errors :
*
*                   (a) Acknowledge transmit interrupt.
*                   (b) Post transmit FIFO empty signal.
*
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_ISR_Handler().
*
* Note(s)     : (2) NetNIC_ISR_Handler() decodes AT91RM9200 Transmit ISR & calls NetNIC_TxISR_Handler().
*********************************************************************************************************
*/
static  void  NetNIC_TxISR_Handler (void)
{
                                                                        /* Interrupts are acknowledged when ISR read.          */
                                                                        /* See 'ATMEL AT91RM9200; Revision 1768B-ATARM–08/03'  */
                                                                        /* p. 586.                                             */
                                                                        /* ISR are previously read in NetNIC_ISR_Handler().    */

/*start 2013-12-25 modified by xjx, */                                                                        /* ------------- POST TX COMPLETE SIGNAL ------------- */
    NetOS_NIC_TxRdySignal();
/*end 2013-12-25 modified by xjx, */
}


/*
*********************************************************************************************************
*                                         NetNIC_TxPktDiscard()
*
* Description : On any Transmit errors, set error.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_ERR_TX                  Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_TxPkt().
*********************************************************************************************************
*/

static  void  NetNIC_TxPktDiscard (NET_ERR  *perr)
{
#if (NET_CTR_CFG_ERR_EN      == DEF_ENABLED)
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR   cpu_sr;
#endif
#endif


    NET_CTR_ERR_INC(NetNIC_ErrTxPktDiscardedCtr);

   *perr = NET_ERR_TX;
}


