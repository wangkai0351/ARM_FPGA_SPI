/*
*********************************************************************************************************
*         DW Project       
*
*Hardware platform : 
*
* File             : at_cfg_interface.c
* version          : v1.0
* By               : 
* Modify         : 
* Date             : 
*********************************************************************************************************
*/
#include "global.h"
#include "board.h"

#include "net.h"
#include "at_cfg.h"
#include "at_err.h"





NET_ERR AtSetIPAddr (NET_IP_ADDR ip_addr, NET_IP_ADDR ip_mask)
{
    NET_ERR at_err;

    at_err = NetIP_CfgAddrThisHost(ip_addr, ip_mask);

    return at_err;
}


NET_IP_ADDR  AtGetIPAddr (void)
{
    return  NetIP_GetAddrThisHost();
}


NET_IP_ADDR AtGetIPMask (void)
{
    return NetIP_GetAddrSubnetMask();
}

NET_ERR AtSetIPAddrStr (CPU_CHAR *ip_str, CPU_CHAR *mask_str)
{
    NET_ERR at_err;
    NET_IP_ADDR ip_addr;
    NET_IP_ADDR ip_mask;

    ip_addr = NetASCII_Str_to_IP(ip_str,  &at_err);
    if (NET_ASCII_ERR_NONE != at_err) {
        return at_err;
    }

    ip_mask = NetASCII_Str_to_IP(mask_str, &at_err);
    if (NET_ASCII_ERR_NONE != at_err) {
        return at_err;
    }

    at_err = NetIP_CfgAddrThisHost(ip_addr, ip_mask);

    return at_err;
}


NET_ERR AtSetIPGateway (NET_IP_ADDR ip_gw)
{
    NET_ERR at_err;

    at_err = NetIP_CfgAddrDfltGateway(ip_gw);

    return at_err;
}


NET_IP_ADDR AtGetIPGateway (void)
{
    return NetIP_GetAddrDfltGateway();
}


NET_ERR AtSetIPGatewayStr (CPU_CHAR *gw_str)
{
    NET_ERR at_err;
    NET_IP_ADDR ip_gw;

    ip_gw = NetASCII_Str_to_IP(gw_str,   &at_err);
    if (NET_ASCII_ERR_NONE != at_err) {
        return at_err;
    }

    at_err = NetIP_CfgAddrDfltGateway(ip_gw);

    return at_err;
}


void AtSetEmacAddr(INT8U *mac_addr)
{
    INT8U mac_addr_dflt[6] = {0x00,0x50,0xc2,0x25,0x60,0x01};

    if (NULL == mac_addr) {
        AT91C_BASE_EMAC->EMAC_SA1L = ((INT32U) mac_addr_dflt[3] << 24) | ((INT32U) mac_addr_dflt[2] << 16) | ((INT32U) mac_addr_dflt[1] << 8) | mac_addr_dflt[0];
        AT91C_BASE_EMAC->EMAC_SA1H = ((INT32U) mac_addr_dflt[5] << 8) | mac_addr_dflt[4];
        return;
    }

    AT91C_BASE_EMAC->EMAC_SA1L = ((INT32U) mac_addr[3] << 24) | ((INT32U) mac_addr[2] << 16) | ((INT32U) mac_addr[1] << 8) | mac_addr[0];
    AT91C_BASE_EMAC->EMAC_SA1H = ((INT32U) mac_addr[5] << 8) | mac_addr[4];
}


void AtGetEmacAddr(INT8U *mac_addr)
{
    Mem_Copy((void *)mac_addr, (void *)AT91C_BASE_EMAC->EMAC_SA1L, (CPU_SIZE_T)4);
    Mem_Copy((void *)mac_addr[4], (void *)AT91C_BASE_EMAC->EMAC_SA1H, (CPU_SIZE_T)2);
}



