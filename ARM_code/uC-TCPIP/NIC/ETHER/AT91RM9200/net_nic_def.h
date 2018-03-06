/*
*********************************************************************************************************
*                                              uC/TCP-IP
*                                      The Embedded TCP/IP Suite
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
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
*                                   NETWORK INTERFACE CARD DEFINES
*
*                                            Davicom DM9000EP
*
* Filename      : net_nic_def.h
* Version       : V1.88
* Programmer(s) : EHS
*********************************************************************************************************
*/

#ifndef  __NET_NIC_DEF_H__
#define  __NET_NIC_DEF_H__

/*
*********************************************************************************************************
*                                        DEFINITIONS
*
* Note(s) : (1) These global #define's are required PRIOR to network configuration.
*********************************************************************************************************
*/

#define  EMAC_MAC_ADDR_SEL_NONE                     0       /* Do not explicity select a MAC address src   */
#define  EMAC_MAC_ADDR_SEL_EEPROM                   1       /* EMAC MAC addr cfg'd from NIC's ext EEPROM.  */
#define  EMAC_MAC_ADDR_SEL_CFG                      2       /* EMAC MAC addr cfg'd by prod app @ init.     */

#define  EMAC_SPD_0                                 0       /* Link speed unknown, or link down            */
#define  EMAC_SPD_10                               10       /* Link speed = 10mbps                         */
#define  EMAC_SPD_100                             100       /* Link speed = 100mbps                        */

#define  EMAC_DUPLEX_UNKNOWN                        0       /* Duplex uknown or auto-neg incomplete        */
#define  EMAC_DUPLEX_HALF                           1       /* Duplex = Half Duplex                        */
#define  EMAC_DUPLEX_FULL                           2       /* Duplex = Full Duplex                        */


#endif
