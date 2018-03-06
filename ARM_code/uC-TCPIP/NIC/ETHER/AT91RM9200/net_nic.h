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
* Filename      : net_nic.h
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
*                                                EXTERNS
*********************************************************************************************************
*/

#ifdef   NET_NIC_MODULE
#define  NET_NIC_EXT
#else
#define  NET_NIC_EXT  extern
#endif


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                DEFINES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                           GLOBAL VARIABLES
*********************************************************************************************************
*/

NET_NIC_EXT  CPU_BOOLEAN  NetNIC_ConnStatus;                            /* NIC's connection status : DEF_ON/DEF_OFF.           */

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)                                /* ------------------- NET DRV STATS ----------------- */
NET_NIC_EXT  NET_CTR      NetNIC_StatRxPktCtr;
NET_NIC_EXT  NET_CTR      NetNIC_StatTxPktCtr;
#endif

#if (NET_CTR_CFG_ERR_EN  == DEF_ENABLED)                                /* ------------------- NET DRV ERRS ------------------ */
NET_NIC_EXT  NET_CTR      NetNIC_ErrRxPktDiscardedCtr;
NET_NIC_EXT  NET_CTR      NetNIC_ErrTxPktDiscardedCtr;
#endif


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void         NetNIC_Init              (NET_ERR     *perr);

void         NetNIC_IntEn             (NET_ERR     *perr);

                                                                        /* ------------------- STATUS FNCTS ------------------ */
CPU_BOOLEAN  NetNIC_ConnStatusGet     (void);

void         NetNIC_ISR_Handler       (void);                           /* Decode & handle rx/tx ISRs.                         */

                                                                        /* --------------------- RX FNCTS -------------------- */
CPU_INT16U   NetNIC_RxPktGetSize      (void);                           /* Get NIC rx pkt size.                                */


/*start 2013-12-20 modified by xjx, */
CPU_INT16U   NetNIC_RxPkt             (void        *ppkt,               /* Rx pkt from NIC.                                    */
                                       CPU_INT16U   size,
                                       NET_ERR     *perr);
/*end 2013-12-20 modified by xjx*/
void         NetNIC_RxPktDiscard      (CPU_INT16U   size,               /* Discard rx pkt in NIC.                              */
                                       NET_ERR     *perr);

                                                                        /* --------------------- TX FNCTS -------------------- */
void         NetNIC_TxPktPrepare      (void        *ppkt,
                                       CPU_INT16U   size,
                                       NET_ERR     *perr);

void         NetNIC_TxPkt             (void        *ppkt,
                                       CPU_INT16U   size,
                                       NET_ERR     *perr);

                                                                        /* ------------------- PHYTER FNCTS ------------------ */
void         NetNIC_PhyInit           (NET_ERR     *perr);

CPU_INT16U   NetNIC_PhyRegRd          (CPU_INT08U   phy,
                                       CPU_INT08U   reg,
                                       NET_ERR     *perr);

void         NetNIC_PhyRegWr          (CPU_INT08U   phy,
                                       CPU_INT08U   reg,
                                       CPU_INT16U   val,
                                       NET_ERR     *perr);


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                       DEFINED IN OS'S  net_os.c
*********************************************************************************************************
*/

void         NetOS_NIC_Init           (NET_ERR     *perr);              /* Create Drv objs & start Drv tasks.                  */

void         NetOS_NIC_TxRdyWait      (NET_ERR     *perr);              /* Wait on drv tx empty signal from NIC.               */

void         NetOS_NIC_TxRdySignal    (void);                           /* Post    drv tx empty signal from NIC.               */


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                    DEFINED IN PRODUCT'S  net_bsp.c
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                   DEFINED IN PRODUCT's  net_isr_a.s
*********************************************************************************************************
*/

void         NetNIC_ISR               (void);


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                    DEFINED IN PRODUCT'S  net_isr.c
*
* Note(s) : (1) See this 'net_nic.c  AT91RM9200_EMAC_Init() Note #5'.
*
*           (2) See this 'net_nic.c  NetNIC_ISR_Handler()   Note #3'.
*********************************************************************************************************
*/

#if (NET_NIC_CFG_INT_CTRL_EN == DEF_ENABLED)
void         NetNIC_IntInit           (void);                           /* Init int ctrl'r        [see Note #1].               */

void         NetNIC_IntClr            (void);                           /* Clr  int ctrl'r src(s) [see Note #2].               */
#endif


/*
*********************************************************************************************************
*                                         CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef   NET_NIC_CFG_INT_CTRL_EN
#error   "NET_NIC_CFG_INT_CTRL_EN           not #define'd in 'net_cfg.h'"
#error   "                            [MUST be  DEF_DISABLED]"
#error   "                            [     ||  DEF_ENABLED ]"
#elif   ((NET_NIC_CFG_INT_CTRL_EN != DEF_DISABLED) && \
         (NET_NIC_CFG_INT_CTRL_EN != DEF_ENABLED ))
#error   "NET_NIC_CFG_INT_CTRL_EN           illegally #define'd in 'net_cfg.h'"
#error   "                            [MUST be  DEF_DISABLED]"
#error   "                            [     ||  DEF_ENABLED ]"
#endif



#if      (NET_NIC_CFG_TX_PKT_PREPARE_EN != DEF_ENABLED)
#error   "NET_NIC_CFG_TX_PKT_PREPARE_EN     MUST be enabled with this driver."
#endif
