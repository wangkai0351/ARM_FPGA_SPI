/*
*********************************************************************************************************
*         DW Project       
*
*Hardware platform : 
*
* File             : at_task.h
* version          : v1.0
* By               : 
* Modify         : 
* Date             : 
*********************************************************************************************************
*/
#ifndef AT_TASK_H
#define AT_TASK_H


#define AT_TASK_INIT            0
#define AT_TASK_IS_RUNNING         1   
#define AT_TASK_PENDING     2


#define AT_CFG_TASK_START_STK_SIZE     10240

#define AT_NETCONFIG_TASK_PRIO			56

//#define AT_ARPSCAN_TASK_PRIO                		50

#define AT_TCPSend_TASK_PRIO                        50


#define WREG(x)                                                              \
        (*((volatile unsigned short *)(x)))
#define RREG(x)                                                             \
	(*((volatile unsigned short *)(x)))
#define WRAM(x)                                                              \
        (*((unsigned short *)(x)))        
#define DWRAM(x)                                                              \
        (*((unsigned long *)(x)))      



#define AT_NETCONFIG_MSG_Q_SIZE   64

#define NET_SOCK_RX_DATA_SIZE	1500

typedef struct at_port_scan_msg_q_entry {
    INT8U    index;
    INT32U    addr;
//    INT8U	    mac_addr[6];
    INT8U    reserved;
    INT8U    reserved1;
}AT_PORT_SCAN_MSG_Q_ENTRY;




typedef struct at_net_q_entry{
	INT32U ip_addr;
	INT16U net_port;
	NET_SOCK_ID   socket_id;

}AT_NET_Q_ENTRY;

typedef struct at_netconfig_msg_q_entry{
	INT32U ip_addr;
	INT16U netconfig_server_port;

}AT_NETCONFIG_MSG_Q_ENTRY;

extern INT8U  Arp_Cache_Scan_Task_State;

extern OS_EVENT *AtTask_TCPSend_SignalPtr;
extern INT8U  Arp_Cache_Scan_Task_State;

extern void AtAllTaskInit (void);

extern void  AtTCPSendTask (void *p_arg);

extern void  AtTaskCreate (void);

extern void  AtEventInit (void);
extern void RTAddr_Config(INT8U data);


#endif

