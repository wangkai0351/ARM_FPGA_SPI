/*
*********************************************************************************************************
*         DW Project       
*
*Hardware platform : 
*
* File             : at_task.c
* version          : v1.0
* By               : 
* Modify         : 
* Date             : 
*********************************************************************************************************
*/

#include "global.h"
#include "net.h"
#include "at_cfg.h"
#include "at_err.h"
#include"pio.h"
#include "at_task.h"
#include"cpu.h"
#include"os_cpu.h"
#include"wdt.h"
#include"Bu61580.h"

INT8U  Arp_Cache_Scan_Task_State;

INT8U  NetServerStart_Task_State;


OS_EVENT *AtTask_TCPSend_SignalPtr;

OS_EVENT *AtTask_NetConfigServer_Rcv_QueuePtr;

OS_STK       AtTCPSend_TaskStartStk[AT_CFG_TASK_START_STK_SIZE];
OS_STK	    AtNetConfigServer_TaskStartStk[AT_CFG_TASK_START_STK_SIZE];


AT_NETCONFIG_MSG_Q_ENTRY AT_NetConfig_Msg_Q[AT_NETCONFIG_MSG_Q_SIZE];
void* AT_NetConfig_Msg_Q_Ptr[AT_NETCONFIG_MSG_Q_SIZE];
INT16U AT_NetConfig_Msg_Q_Index;

INT8U	NET_SOCK_RX_DATA[NET_SOCK_RX_DATA_SIZE];

STACK_STRU *pStackDisc;     //ÏûÏ¢ÃèÊö¿é
BC_TO_RT_STRU *pMsgBC2RT;
RT_TO_BC_STRU *pMsgRT2BC;
RT_TO_RT_STRU *pMsgRT2RT;
BC_TO_RT_CMD_STRU *pMsgBC2RT_CMD;
ExpandModeRAM * pRAM;
RTExpandModeRAM *pRTRAM;

Uint16 Stkptr;

/*-----------------------------------------------------------------------
º¯ÊýÃû³Æ£º      MClr
º¯ÊýËµÃ÷£º     ¶ÔÖ¸¶¨µØÖ·¿ªÊ¼Ð´0
Èë¿Ú²ÎÊýËµÃ÷£º dst Ä¿±êµØÖ· size Êý¾Ý´óÐ¡
·µ»Ø²ÎÊýËµÃ÷£º  ÎÞ
ÐÞ¸ÄËµÃ÷£º      ÎÞ
±¸×¢£º          
-------------------------------------------------------------------------*/
void MClr(unsigned short *dst,unsigned long size)
{
	while(size--)
	{
		*dst++=0;
	}
}


void AtAllTaskInit (void)
{

    Arp_Cache_Scan_Task_State = AT_TASK_INIT;

    NetServerStart_Task_State = AT_TASK_INIT;

    
    AtEventInit();  
	   
    Soft_HardwareWDTInit();	
    
}

void BCInit(void)//ÔöÇ¿Ä£Ê½Ö§³ÖÖØ´«
{
	WREG(REG_RESET)=0x0001;  //Èí¼þ¸´Î»
	MClr((unsigned short *) BU61580_RAM_BASE, 256);
	MClr((unsigned short *)MSG_BLOCK_BASE32, MSG_BLOCK_SIZE*3);
	pRAM=(ExpandModeRAM * )STACK_A_P;
	
//	WREG(REG_RESET)=0x0001;
        WREG(REG_CFG3)=0x8000;//ÔöÇ¿Ä£Ê½
	WREG(REG_INT_MASK)=0x0018;//0;ÖÐ¶Ï²»ÆÁ±Î
//	WREG(REG_CFG1)=0x0170;//0x0030;//0x70;//0x170; //ÅäÖÃÔöÇ¿Ä£Ê½ÏÂÖ÷Òª²ÎÊý£¬±ÈÈçÏûÏ¢´íÎóÊ±Í£Ö¹ÏûÏ¢/FRAME AUTO-REPEAT
	WREG(REG_CFG2)=0x8010;//0x8290;//0x8008;//0x0008;//0xc70; //ÖÐ¶ÏÒý½Å¸ºÂö³åÊä³ö
	WREG(REG_CFG4)=0x1060;//0x1060;//0x17e0;//0x1660;//ÅäÖÃBC¿ØÕ¹¿ØÖÆ×ÖÊ¹ÄÜ£¬ÏûÏ¢´íÎóÖØ´« ÖØ´«Ê¹ÓÃ²»Í¬×ÜÏßµÈ
	WREG(REG_CFG5)=0x0200;//0x301;//Ñ¡Ôñ16MHZÊ±ÖÓ RTµØÖ·ÆæÅ¼Ð£Ñé
	WREG(REG_TIME_TAG)=0;//³õÊ¼»¯Ê±±ê¼Ä´æÆ÷ 
//	WREG(REG_FRAME_TIME)=0x2710;//1000ms;//µ¥Î»100us Ö¡ËÙÂÊ200us
	WREG(REG_CFG3)=0x8000;//ÔöÇ¿Ä£Ê½
	        
	pRAM->PStackA=0;//4;  //Õ»Ö¸ÕëÖ¸ÏòµÚ¶þ¸öÏûÏ¢ÃèÊöÆ÷
	pRAM->MsgACnt=0xfffe; //·¢ËÍ´ÎÊý1
        
	pRAM->InitPStackA=0;//4;//Ö¡ÆðÊ¼Õ»Ö¸ÕëÎ»ÖÃ
	pRAM->InitMsgACnt=0xfffe;
	
	pStackDisc=(STACK_STRU *)BU61580_RAM_BASE;
	pStackDisc->BlockStatusWord=0;//0xdebf;
	pStackDisc->TimeTagWord=0;//0xaabb;
	pStackDisc->MsgGapWord=0x0320;//1000;//110;
	pStackDisc->MsgBlockAddr=MSG_BLOCK_BASE16;
/*	pStackDisc++;
	pStackDisc->BlockStatusWord=0;//0xdebf;
	pStackDisc->TimeTagWord=0;//0xccdd;
	pStackDisc->MsgGapWord=0x03e8;
	pStackDisc->MsgBlockAddr=MSG_BLOCK_BASE16+MSG_BLOCK_SIZE;
	pStackDisc++;
	pStackDisc->BlockStatusWord=0xdebf;
	pStackDisc->TimeTagWord=0xccdd;
	pStackDisc->MsgGapWord=151;
	pStackDisc->MsgBlockAddr=MSG_BLOCK_BASE16+MSG_BLOCK_SIZE*2;	

	pMsgRT2BC=(RT_TO_BC_STRU *)MSG_BLOCK_BASE32;
	pMsgRT2BC->CtrWord=0x310;//0x390;    //¿ØÖÆ×Ö£¬ÓÃÀ´Ñ¡Ôñ·¢ËÍÍ¨µÀºÍ·¢ËÍ·½Ê½
	pMsgRT2BC->TComWord=0x0c23;         //·¢ËÍÃüÁî×Ö

	pMsgBC2RT=(BC_TO_RT_STRU *)(MSG_BLOCK_BASE32+MSG_BLOCK_SIZE*2);
	pMsgBC2RT->CtrWord=0x310;//0x390ÎªAÍ¨µÀ£¬310ÎªBÍ¨µÀ;  //¿ØÖÆ×Ö£¬ÓÃÀ´Ñ¡Ôñ·¢ËÍÍ¨µÀºÍ·¢ËÍ·½Ê½
	pMsgBC2RT->RComWord=0x0823;       //½ÓÊÕÃüÁî×Ö
	pMsgBC2RT->Data[0]=0xfc73;
	pMsgBC2RT->Data[1]=1;
	pMsgBC2RT->Data[2]=30;	   */
/*
	pMsgRT2RT=(RT_TO_RT_STRU *)(MSG_BLOCK_BASE32+MSG_BLOCK_SIZE*4);
	pMsgRT2RT->CtrWord=0x311;
	pMsgRT2RT->RComWord=0x1023;
	pMsgRT2RT->TComWord=0x0c23;*/
	
	//MCpy(BUTemp, (unsigned short *)BU61580_REG_BASE, 15);
	//MCpy(&BUTemp[16], (volatile unsigned short *)BU61580_RAM_BASE, 12);
	//MCpy(&BUTemp[29], (volatile unsigned short *)STACK_A_P, 4);
	//MCpy(&BUTemp[34], (volatile unsigned short *)MSG_BLOCK_BASE32, MSG_BLOCK_SIZE*4);
	
	//WREG(REG_RESET)|=BCSTART;
	//delay(250);
	//pRAM->InitPStackA=0;
	//pRAM->InitMsgACnt=0xfffe;	
	//WREG(REG_RESET)|=0x0040;
/*	pRAM->PStackA=4;
	pRAM->MsgACnt=0xfffe;
	pRAM->InitPStackA=4;
	pRAM->InitMsgACnt=0xfffe;*/	
	//MCpy((unsigned short *)BU61580_RAM_BASE, chara, 15);
}

/*-----------------------------------------------------------------------
º¯ÊýÃû³Æ£º      RTInit
º¯ÊýËµÃ÷£º      RTÔöÇ¿Ä£Ê½³õÊ¼»¯
Èë¿Ú²ÎÊýËµÃ÷£º  ÎÞ
·µ»Ø²ÎÊýËµÃ÷£º  ÎÞ
ÐÞ¸ÄËµÃ÷£º             ÎÞ
±¸×¢£º         
-------------------------------------------------------------------------*/
void RTInit(void)
{
     WREG(REG_RESET)=0x0001;  //Èí¼þ¸´Î»
     MClr((unsigned short *) BU61580_RAM_BASE, 256);
     MClr((unsigned short *)SAD_BSY_TABLE,8);  //All subaddresses initialized as not busy	 
//      WREG(REG_RESET)=0x0001;  //Èí¼þ¸´Î»
//      WREG(REG_CFG1)=0x8f80;  //ENHANCED  RT
      WREG(REG_CFG3) =0x801D;//0x8000;   //ÔöÇ¿Ä£Ê½
      WREG(REG_INT_MASK)=0x0001;//0x0000; //ÖÐ¶ÏÊ¹ÄÜ
      WREG(REG_CFG2)=0xb813;       //Enable ENHANCEDINTERRUPTS, BUSY LOOKUP T ABLE, RECEIVE SUBADDRESS DOUBLE BUFFERING, ENHANCED RT MEMORY MANAGEMENT
      WREG(REG_CFG5)=0x0100;      // 16M clock EXPANDED ZERO-CROSSING ENABLED
      pRTRAM=(RTExpandModeRAM * )STACK_A_P;
	  
      pRTRAM->pRTComStackA=0x0000;  //RT Command Stack Pointer AÖ¸ÏòStack A
      pRTRAM->RXLookupTabA[1]=0x0800; //Reserved for 1024-word circular buffer for receive subaddress1Êý¾Ý´æ·ÅÔÚ0x0800-0x0BFF
//      pRTRAM->RXLookupTabA[1]=0x0800; //Reserved for 1024-word circular buffer for receive subaddress1Êý¾Ý´æ·ÅÔÚ0x0800-0x0BFF
//      pRTRAM->RXLookupTabA[2]=0x0800; //Reserved for 1024-word circular buffer for receive subaddress1Êý¾Ý´æ·ÅÔÚ0x0800-0x0BFF
//      pRTRAM->RXLookupTabA[3]=0x0800; //Reserved for 1024-word circular buffer for receive subaddress1Êý¾Ý´æ·ÅÔÚ0x0800-0x0BFF
//      pRTRAM->RXLookupTabA[4]=0x0800; //Reserved for 1024-word circular buffer for receive subaddress1Êý¾Ý´æ·ÅÔÚ0x0800-0x0BFF
      
      
      pRTRAM->TXLookupTabA[1]=0x0400;//·¢ËÍsubaddress1Êý¾Ý´æ·ÅÔÚ0x0400(Êý¾Ý¿é5)
      pRTRAM->SubadContrWordTab[1]=0xc210;//TX EOM/RX EOM/BCST EOM ÖÐ¶ÏÊ¹ÄÜ/RX: DOUBLE BUFFER ENABLE/·¢ËÍµ¥ÏûÏ¢Ä£Ê½
      MClr((unsigned short *) CMD_ILIEGAL_TABLE, 256);
      MClr((unsigned short *) TX_DATA_ADDR, 32);
//      	Uint16 m;
//	for(m=0;m<32;m++)
//		{
//			pRTRAM->DataBlock5_100[m]=0x000a+m*2;
//		}
//      pRTRAM->DataBlock5_100[0]=0xfc73; 
//      pRTRAM->DataBlock5_100[1]=0x0001;
//      pRTRAM->DataBlock5_100[2]=0x0100;
//     pRTRAM->DataBlock5_100[3]=0x1122;
      
      WREG(REG_CFG1)=0x8f80;//0x8e80;//0x8900;//0x8e00;//0x8f80;//0x8b00;//0x8d80;//0x8f00;//0x8a00;//0x8d00;  //ENHANCED  RT
}



NET_SOCK_DESC At_Fdset;
INT32U TCPConnect (AT_NET_Q_ENTRY *TCP_tcb)
{
    NET_ERR            at_err;

    NET_SOCK_ADDR_IP   rtu_sock_addr_ip;
    NET_SOCK_ADDR_LEN  rtu_sock_addr_ip_size;

    //NET_SOCK_RTN_CODE  conn_rtn_code;
    INT8U  conn_retry_times = 0;


    rtu_sock_addr_ip_size = sizeof(NET_SOCK_ADDR_IP);
    Mem_Clr((void     *)&rtu_sock_addr_ip, (CPU_SIZE_T)rtu_sock_addr_ip_size);
    rtu_sock_addr_ip.AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V4;
    rtu_sock_addr_ip.Addr       = NET_UTIL_HOST_TO_NET_32(TCP_tcb->ip_addr); 
    rtu_sock_addr_ip.Port       = NET_UTIL_HOST_TO_NET_16(TCP_tcb->net_port);


    TCP_tcb->socket_id = NetSock_Open(NET_SOCK_ADDR_FAMILY_IP_V4,
                                                                         NET_SOCK_TYPE_STREAM,
                                                                         NET_SOCK_PROTOCOL_TCP,
                                                                         &at_err);
    if (NET_SOCK_ERR_NONE != at_err) {
        AtErrLogWrite(0x01);
        return  0x01;//AT_IEC104_CONNECT_OPEN_SOCK_ERR;
    }


    do {
        /*conn_rtn_code = */NetSock_Conn((NET_SOCK_ID)TCP_tcb->socket_id,
                                                     (NET_SOCK_ADDR*)&rtu_sock_addr_ip,
                                                     (NET_SOCK_ADDR_LEN)sizeof(rtu_sock_addr_ip),
                                                     (NET_ERR*)&at_err);
        conn_retry_times++;
    } while ((NET_SOCK_ERR_NONE != at_err) && (conn_retry_times < 3));

    if (NET_SOCK_ERR_NONE != at_err) {
        NetSock_Close(TCP_tcb->socket_id, &at_err);
        AtErrLogWrite(0x02);
        return 0x02;//AT_IEC104_CONNECT_CONNECT_SOCK_ERR;
    }

    FD_SET(TCP_tcb->socket_id, &At_Fdset);

    return 0x03;//AT_IEC104_ERR_NONE;
}


INT32U TcpPackageSend (AT_NET_Q_ENTRY *tcp, INT8U *data, INT32S data_len)
{
    INT32S tx_size_sended = 0;
    NET_ERR at_err;

    do {
        tx_size_sended = NetSock_TxData(tcp->socket_id, 
                                                        (void *)data, 
                                                        (INT16U)data_len, 
                                                        (INT16S)NET_SOCK_FLAG_NONE, 
                                                        &at_err);

        OSTimeDlyHMSM(0, 0, 0, 100);
    } while (0 > tx_size_sended);

    if (NET_SOCK_ERR_NONE != at_err) {
        NetSock_Close(tcp->socket_id, &at_err);
        AtErrLogWrite(0x05);
        return 0x05;
    }

    if (data_len != tx_size_sended) {
        NetSock_Close(tcp->socket_id, &at_err);
        AtErrLogWrite(0x06);
        return 0x06;
    }

    return 0x01;
}

AT_NET_Q_ENTRY m_tcp;
static   RTCommandStack *newrxpacket = NULL;
void  AtTCPSendTask (void *p_arg)
{
	INT8U  terr,conn_fail,send_fail;
	NET_ERR            at_err;
	Uint16 *pReadRAM,*pRTDATA;
	AT_NET_Q_ENTRY *m_tcp_p;
	INT8U net_connect=1;
   	INT8U motor_cmd[12]={0x30,0x30,0x31,0x31,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x39};
     	RTInit();
	IOCLR(AT91C_BASE_PIOC, RTAD0);
	IOSET(AT91C_BASE_PIOC, RTAD1);
	IOCLR(AT91C_BASE_PIOC, RTAD2);
	IOCLR(AT91C_BASE_PIOC, RTAD3);
	IOCLR(AT91C_BASE_PIOC, RTAD4);
	IOCLR(AT91C_BASE_PIOC, RTADP);
       AIC_EnableIT(AT91C_ID_IRQ1); //¿ªIRQ1ÖÐ¶Ï
        
        RTCommandStack*Packet=newrxpacket;
        m_tcp_p = &m_tcp;
	 m_tcp_p->ip_addr=0xc0a800c8;
	 m_tcp_p->net_port=4660;    
      
        while (DEF_TRUE) {
         		 
		Arp_Cache_Scan_Task_State = AT_TASK_PENDING;
             	                
		OSSemPend(AtTask_TCPSend_SignalPtr, 0, (INT8U *)&terr);         //1000=2.67s

		Arp_Cache_Scan_Task_State = AT_TASK_IS_RUNNING;

              if(net_connect)
		{
			TCPConnect(m_tcp_p);
			net_connect=0;
		}
                pReadRAM=(Uint16 *)(Stkptr+BU61580_RAM_BASE);	
		Packet->pDataBlock=*(pReadRAM+2);
		pRTDATA=(Uint16 *)(((Packet->pDataBlock)*2)+BU61580_RAM_BASE);//È¡Êý¾ÝÕ»Ö¸Õë
		Packet->RTData[0]=*pRTDATA;
		Packet->RTData[1]=*(pRTDATA+1);
		Packet->RTData[2]=*(pRTDATA+2);
                if(Packet->RTData[0]==0x0001)
                {
                    motor_cmd[2]=0x30;   //ÄæÊ±Õë×ª
                }
                else if(Packet->RTData[0]==0x02)
                {
                    motor_cmd[2]=0x31;    //Ë³Ê±Õë×ª
                }
                if(Packet->RTData[1]==0x0002)
                {
                    motor_cmd[8]=0x34;    //Âö³åÆµÂÊ400HZ
                }
                else if(Packet->RTData[1]==0x0003)
                {
                    motor_cmd[8]=0x39;    //Âö³åÆµÂÊ900HZ
                }
                 if(Packet->RTData[2]==0x0002)
                {
                    motor_cmd[8]=0x30;     //Í£Ö¹  
                    AtTask_TCPSend_SignalPtr->OSEventCnt=0;
                }
                Stkptr=(Stkptr+8)%256;
				
		send_fail=TcpPackageSend(m_tcp_p, (INT8U *)&motor_cmd, 12);
//		if(send_fail !=0x01)
//		{
			NetSock_Close(m_tcp_p->socket_id, &at_err);
			OSTimeDlyHMSM(0, 0, 0, 100);
			TCPConnect(m_tcp_p);
//		} 
        }

}


void  AtNetConfigServerStart (void *p_arg)
{
	Uint16 i,j,test;
    NET_SOCK_ID         sock_listen;
    NET_SOCK_ID         sock_req;

    NET_SOCK_ADDR_IP    server_sock_addr_ip;
    NET_SOCK_ADDR_LEN   server_sock_addr_ip_size; 
    NET_SOCK_ADDR_IP    client_sock_addr_ip;
    NET_SOCK_ADDR_LEN   client_sock_addr_ip_size; 
    CPU_BOOLEAN         attempt_conn;
    INT32S  rx_size; 

    NET_ERR             at_err;

    NET_IP_ADDR svr_ip_addr;

    INT8U terr; 
    AT_NETCONFIG_MSG_Q_ENTRY *msg_ptr;

//add IP_address
	
    svr_ip_addr=0xc0a8000b;
    INT8U listen_state=1;
    INT8U  idle_count=0;
	   
    while (DEF_ON) { 
    
	//SuspenWdt(AtNetConfigServerStart_SWDT_ID);
	NetServerStart_Task_State=AT_TASK_PENDING;
	
        //msg_ptr = OSQPend(AtTask_NetConfigServer_Rcv_QueuePtr, (INT32U)(10*1000), (INT8U *)&terr);
	
	NetServerStart_Task_State=AT_TASK_IS_RUNNING;

	//RunWdt(AtNetConfigServerStart_SWDT_ID);
	//SoftWdtFed(AtNetConfigServerStart_SWDT_ID);

        OSTimeDlyHMSM(0, 0, 0, 100);
	
//        svr_ip_addr =  msg_ptr->ip_addr;
	//if(svr_ip_addr != AtGetIPAddr())	{
	//	NetIP_CfgAddrThisHost(svr_ip_addr, 0xFFFFFF00);
	//}
//	svr_ip_addr=AtGetIPAddr();
	if(0==listen_state){
		continue;
	}
	//¿ªÆôÕìÌýSOCKET ¶Ë¿Ú
        sock_listen = NetSock_Open( NET_SOCK_ADDR_FAMILY_IP_V4,
                                    NET_SOCK_TYPE_STREAM,
                                    NET_SOCK_PROTOCOL_TCP,
                                   &at_err);
        if (at_err != NET_SOCK_ERR_NONE) {
            AtErrLogWrite(NET_SERVER_SOCK_OPEN_ERR);
            continue;
        }

	//Éè±¸ÕìÌý¶Ë¿Ú²ÎÊý
        server_sock_addr_ip_size = sizeof(server_sock_addr_ip);
        Mem_Clr((void     *)&server_sock_addr_ip,
                (CPU_SIZE_T) server_sock_addr_ip_size);
        server_sock_addr_ip.AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V4;
        server_sock_addr_ip.Addr       = NET_UTIL_HOST_TO_NET_32(svr_ip_addr); 
        server_sock_addr_ip.Port       = NET_UTIL_HOST_TO_NET_16(2000/*msg_ptr->netconfig_server_port*/);
        
        //¹ØÁªIPµØÖ·½øÈëÕìÌý×´Ì¬
        NetSock_Bind((NET_SOCK_ID      ) sock_listen,
                     (NET_SOCK_ADDR   *)&server_sock_addr_ip,
                     (NET_SOCK_ADDR_LEN) NET_SOCK_ADDR_SIZE,
                     (NET_ERR         *)&at_err);
        if (at_err != NET_SOCK_ERR_NONE) {
            NetSock_Close(sock_listen, &at_err);
            AtErrLogWrite(NET_SERVER_SOCK_BIND_ERR);
            continue;
        }

	//ÕìÌý½ÓÊÕÁªÈëµÄ¿Í»§¶ËµÄIPµØÖ·ºÍSOCKET
	//½ÓÈëµÄÊý¾Ý·ÅÈëÓÚÖ¸¶¨Á´±íÖÐ
        NetSock_Listen( sock_listen,
                        TCP_SERVER_CONN_Q_SIZE,
                       &at_err);
        if (at_err != NET_SOCK_ERR_NONE) {
            NetSock_Close(sock_listen, &at_err);
            AtErrLogWrite(NET_SERVER_SOCK_LISTEN_ERR);
            continue;
        }    

        

      while (DEF_ON) {

	    //SoftWdtFed(AtNetConfigServerStart_SWDT_ID);	
		
            client_sock_addr_ip_size = sizeof(client_sock_addr_ip);


	    //´ÓÖ¸¶¨Á´±íÖÐ»ñÈ¡Ò»×éIPµØÖ·ºÍSOCKET½øÐÐÊý¾Ý½ÓÊÕ
            sock_req = NetSock_Accept((NET_SOCK_ID        ) sock_listen,	
                                      (NET_SOCK_ADDR     *)&client_sock_addr_ip,
                                      (NET_SOCK_ADDR_LEN *)&client_sock_addr_ip_size,
                                      (NET_ERR           *)&at_err);
            switch (at_err) {
                case NET_SOCK_ERR_NONE:
                     attempt_conn = DEF_NO;
                     break;

                case NET_ERR_INIT_INCOMPLETE:
                     attempt_conn = DEF_YES;
                     break;
                case NET_SOCK_ERR_NULL_PTR:
                     attempt_conn = DEF_YES;
                     break;
                case NET_SOCK_ERR_NONE_AVAIL:
                     attempt_conn = DEF_YES;
                     break;
                case NET_SOCK_ERR_CONN_ACCEPT_Q_NONE_AVAIL:
                     attempt_conn = DEF_YES;
                     break;
                case NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT:
                     attempt_conn = DEF_YES;
                     break;
                case NET_OS_ERR_LOCK:
                     attempt_conn = DEF_YES;
                     break;

                default:
                     attempt_conn = DEF_NO;
                     break;
            }

            if (attempt_conn == DEF_YES) {
		  OSTimeDlyHMSM(0,0,0,300);
                continue;
            }


            if (at_err != NET_SOCK_ERR_NONE) {
                NetSock_Close(sock_req, &at_err);
                AtErrLogWrite(NET_SERVER_SOCK_ACCEPT_ERR);
                continue;
            }

		//*****************
			while (DEF_ON) 
			{ 
				
				//SoftWdtFed(AtNetConfigServerStart_SWDT_ID);
				//ÌáÈ¡Ó¦ÓÃ²ãÊý¾Ý
				rx_size = NetSock_RxData( sock_req,
				      &NET_SOCK_RX_DATA,
				      NET_SOCK_RX_DATA_SIZE,
				      NET_SOCK_FLAG_NONE,
				     &at_err);

				if ((rx_size < 0) && (NET_SOCK_ERR_RX_Q_EMPTY != at_err)) 
				{
					NetSock_Close(sock_req, &at_err);
					AtErrLogWrite(NET_SERVER_RX_SIZE_ERR);
					
					break;
				}
				
				if(NET_SOCK_ERR_RX_Q_CLOSED == at_err)
				{
					NetSock_Close(sock_req, &at_err);
					AtErrLogWrite(NET_SERVER_SOCK_ERR_RX_Q_CLOSED);
					
					break;
				}				
		
				
				if ((rx_size < 0) && (NET_SOCK_ERR_RX_Q_EMPTY == at_err)) 
				{
					idle_count++;
					if(idle_count >=3 ){
						NetSock_Close(sock_req, &at_err);
						AtErrLogWrite(NET_SERVER_RX_SIZE_ERR);
						
						break;
					}
					continue;
				}
				idle_count=0;
				
				if (rx_size > 0) 
				{
						
					//´Ë´¦Ìí¼ÓÊý¾Ý´¦Àí
					switch(NET_SOCK_RX_DATA[0])
					{
						case 0x10:                               //BC·ºÀÄ¹¥»÷
						{
							BCInit();
							WREG(REG_CFG1)=0x0160;//0x0030;//0x70;//0x170; //ÅäÖÃÔöÇ¿Ä£Ê½ÏÂÖ÷Òª²ÎÊý£¬±ÈÈçÏûÏ¢´íÎóÊ±Í£Ö¹ÏûÏ
                                                        pMsgBC2RT=(BC_TO_RT_STRU *)MSG_BLOCK_BASE32;
//							pEMAC+=3;
							if(NET_SOCK_RX_DATA[3]==0x01)
							{
                                            		pMsgBC2RT->CtrWord=0x0390;  //AÍ¨µÀ·¢ËÍ				
                                        		}
							else if(NET_SOCK_RX_DATA[3]==0x10)
							{
							    pMsgBC2RT->CtrWord=0x0310;  //BÍ¨µÀ·¢ËÍ
							}
//	                                        	pEMAC++;
	                                        	pMsgBC2RT->RComWord=((NET_SOCK_RX_DATA[4]<<11)|(NET_SOCK_RX_DATA[5]<<5)|(NET_SOCK_RX_DATA[6]));
//	                                        	pEMAC+=3;
                                                        j=pMsgBC2RT->RComWord & 0x1f;
	                                        	for(i=0;i<j;i++)
	                                        	{
	                                            	pMsgBC2RT->Data[i]=((NET_SOCK_RX_DATA[7+2*i]<<8)+NET_SOCK_RX_DATA[8+2*i]);
//	                                            	pEMAC+=2;
	                                       	 }
	                                        	pMsgBC2RT->LastDataBack=0;
	                                        	pMsgBC2RT->StatusWord=0;
	                                        	delay(1000);
							WREG(REG_RESET)=0x0002;//BCSTART;					
							delay(600);//250
							break;
						}
						case 0x11:                                     //BC¶¨Ê±¹¥»÷
						{
							BCInit();
							WREG(REG_CFG1)=0x0170;//0x0030;//0x70;//0x170; //ÅäÖÃÔöÇ¿Ä£Ê½ÏÂÖ÷Òª²ÎÊý£¬±ÈÈçÏûÏ¢´íÎóÊ±Í£Ö¹ÏûÏ
//							pEMAC++;
                                        		WREG(REG_FRAME_TIME)=((NET_SOCK_RX_DATA[1]<<8)+NET_SOCK_RX_DATA[2]);//0x2710;//1000ms;//µ¥Î»100us Ö¡ËÙÂÊ200us
                                        		pMsgBC2RT=(BC_TO_RT_STRU *)MSG_BLOCK_BASE32;
//                                        		pEMAC+=2;
							if(NET_SOCK_RX_DATA[3]==0x01)
							{
							      pMsgBC2RT->CtrWord=0x0390;  //AÍ¨µÀ·¢ËÍ
							}
							else if(NET_SOCK_RX_DATA[3]==0x10)
							{
							      pMsgBC2RT->CtrWord=0x0310;  //BÍ¨µÀ·¢ËÍ
							}
//		                                        pEMAC++;
                                                        pMsgBC2RT->RComWord=((NET_SOCK_RX_DATA[4]<<11)|(NET_SOCK_RX_DATA[5]<<5)|(NET_SOCK_RX_DATA[6]));
//		                                        pEMAC+=3;
                                                        j=pMsgBC2RT->RComWord & 0x1f;
                                                        for(i=0;i<j;i++)
                                                        {
		                                               pMsgBC2RT->Data[i]=((NET_SOCK_RX_DATA[7+2*i]<<8)+NET_SOCK_RX_DATA[8+2*i]);
//		                                               pEMAC+=2;
                                                        }
                                                        pMsgBC2RT->LastDataBack=0;
                                                        pMsgBC2RT->StatusWord=0;
												
							 WREG(REG_RESET)=0x0002;//BCSTART;
                                                          delay(600);//250
                                                          break;
                                          }
                                          case 0x01:                                //Î±×°RTµØÖ·¹¥»÷
                                          {
							RTInit();
	                                    	test=RREG(REG_CFG2);
							RTAddr_Config(NET_SOCK_RX_DATA[4]);					
	                                          Uint16 m;
							m=NET_SOCK_RX_DATA[6];
							for(i=0;i<m;i++)
							{
								pRTRAM->DataBlock5_100[i]=((NET_SOCK_RX_DATA[7+2*i]<<8)|NET_SOCK_RX_DATA[8+2*i]);
							}
							AIC_EnableIT(AT91C_ID_IRQ1); //¿ªIRQ1ÖÐ¶Ï
							IOCLR(AT91C_BASE_PIOA, LED_AFAIL);
		                                   break;				
						}
						case 0x44:                              //Í£Ö¹
						{
							WREG(REG_RESET)=0x0001;//BCSTOP; 
	                                			break;
						}
						default :
						{
							break;
						}			
					}
				}
			
			}
		NetSock_Close(sock_listen, &at_err);	
		listen_state = 0;
		break;
        }


    }

    
}

void  AtTaskCreate (void)
{
#if (OS_TASK_NAME_EN > 0)
    CPU_INT08U  task_err;
#endif

    OSTaskCreateExt((void (*)(void *)) AtTCPSendTask,                 
	                (void           *) 0,
	                (OS_STK         *)&AtTCPSend_TaskStartStk[AT_CFG_TASK_START_STK_SIZE - 1],
	                (INT8U           ) AT_TCPSend_TASK_PRIO,
	                (INT16U          ) AT_TCPSend_TASK_PRIO,
	                (OS_STK         *)&AtTCPSend_TaskStartStk[0],
	                (INT32U          ) AT_CFG_TASK_START_STK_SIZE,
	                (void           *) 0,
	                (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet(AT_TCPSend_TASK_PRIO, "AtTcpSendData", &task_err);
#endif
/*
    OSTaskCreateExt((void (*)(void *)) AtNetConfigServerStart,      
	                (void           *) 0,
	                (OS_STK         *)&AtNetConfigServer_TaskStartStk[AT_CFG_TASK_START_STK_SIZE - 1],
	                (INT8U           ) AT_NETCONFIG_TASK_PRIO,
	                (INT16U          ) AT_NETCONFIG_TASK_PRIO,
	                (OS_STK         *)&AtNetConfigServer_TaskStartStk[0],
	                (INT32U          ) AT_CFG_TASK_START_STK_SIZE,
	                (void           *) 0,
	                (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet(AT_NETCONFIG_TASK_PRIO, "AtInitiativeUpload", &task_err);
#endif
*/
}

void  AtEventInit (void)
{
    //INT16U   index;



	AtTask_TCPSend_SignalPtr = OSSemCreate((INT16U)0);
	if (AtTask_TCPSend_SignalPtr == (OS_EVENT *)0) {
	    	AtErrLogWrite(0x223344);
	    	return;
	}
    	
	AT_NetConfig_Msg_Q_Index=0;
	AtTask_NetConfigServer_Rcv_QueuePtr=OSQCreate((void * *)&AT_NetConfig_Msg_Q_Ptr[0], AT_NETCONFIG_MSG_Q_SIZE);
	if(AtTask_NetConfigServer_Rcv_QueuePtr == (OS_EVENT *)0){
		AtErrLogWrite(0x112233);
	    	return;
	}
    


}
void RTAddr_Config(INT8U data)
{
	switch(data)
	{
			case 0x01:                                      //RTµØÖ·1
			{
				       IOSET(AT91C_BASE_PIOC, RTAD0);
				       IOCLR(AT91C_BASE_PIOC, RTAD1);
				       IOCLR(AT91C_BASE_PIOC, RTAD2);
				       IOCLR(AT91C_BASE_PIOC, RTAD3);
				       IOCLR(AT91C_BASE_PIOC, RTAD4);
					IOCLR(AT91C_BASE_PIOC, RTADP);
			       	break;
			}
			case 0x02:                                    //RTµØÖ·2
			{
                                   IOCLR(AT91C_BASE_PIOC, RTAD0);
                                   IOSET(AT91C_BASE_PIOC, RTAD1);
                                   IOCLR(AT91C_BASE_PIOC, RTAD2);
                                   IOCLR(AT91C_BASE_PIOC, RTAD3);
                                   IOCLR(AT91C_BASE_PIOC, RTAD4);
	      				IOCLR(AT91C_BASE_PIOC, RTADP);
                                   break;
			}
			case 0x03:                            //RTµØÖ·3
			{
	                           IOSET(AT91C_BASE_PIOC, RTAD0);
	                           IOSET(AT91C_BASE_PIOC, RTAD1);
	                           IOCLR(AT91C_BASE_PIOC, RTAD2);
	                           IOCLR(AT91C_BASE_PIOC, RTAD3);
	                           IOCLR(AT91C_BASE_PIOC, RTAD4);
			             IOSET(AT91C_BASE_PIOC, RTADP);
                                  break;
			}
			case 0x04:
			{
                                  IOCLR(AT91C_BASE_PIOC, RTAD0);
                                  IOCLR(AT91C_BASE_PIOC, RTAD1);
                                  IOSET(AT91C_BASE_PIOC, RTAD2);
                                  IOCLR(AT91C_BASE_PIOC, RTAD3);
                                  IOCLR(AT91C_BASE_PIOC, RTAD4);
			       	IOCLR(AT91C_BASE_PIOC, RTADP);
                                  break;
			}
			case 0x05:
			{
                                   IOSET(AT91C_BASE_PIOC, RTAD0);
                                   IOCLR(AT91C_BASE_PIOC, RTAD1);
                                   IOSET(AT91C_BASE_PIOC, RTAD2);
                                   IOCLR(AT91C_BASE_PIOC, RTAD3);
                                   IOCLR(AT91C_BASE_PIOC, RTAD4);
                                   IOSET(AT91C_BASE_PIOC, RTADP);
                                   break;
			}
			case 0x06:
			{
                                   IOCLR(AT91C_BASE_PIOC, RTAD0);
                                   IOSET(AT91C_BASE_PIOC, RTAD1);
                                   IOSET(AT91C_BASE_PIOC, RTAD2);
                                   IOCLR(AT91C_BASE_PIOC, RTAD3);
                                   IOCLR(AT91C_BASE_PIOC, RTAD4);
			       	IOSET(AT91C_BASE_PIOC, RTADP);
                                   break;
			}
			case 0x07:
			{
	                             IOSET(AT91C_BASE_PIOC, RTAD0);
	                             IOSET(AT91C_BASE_PIOC, RTAD1);
	                             IOSET(AT91C_BASE_PIOC, RTAD2);
	                             IOCLR(AT91C_BASE_PIOC, RTAD3);
	                             IOCLR(AT91C_BASE_PIOC, RTAD4);
			       	IOCLR(AT91C_BASE_PIOC, RTADP);
                                           break;
			}
			case 0x08:
			{
                                   IOCLR(AT91C_BASE_PIOC, RTAD0);
                                   IOCLR(AT91C_BASE_PIOC, RTAD1);
                                   IOCLR(AT91C_BASE_PIOC, RTAD2);
                                   IOSET(AT91C_BASE_PIOC, RTAD3);
                                   IOCLR(AT91C_BASE_PIOC, RTAD4);
			       	IOCLR(AT91C_BASE_PIOC, RTADP);
                                   break;
			}
			case 0x09:
			{
                                   IOSET(AT91C_BASE_PIOC, RTAD0);
                                   IOCLR(AT91C_BASE_PIOC, RTAD1);
                                   IOCLR(AT91C_BASE_PIOC, RTAD2);
                                   IOSET(AT91C_BASE_PIOC, RTAD3);
                                   IOCLR(AT91C_BASE_PIOC, RTAD4);
			      		IOSET(AT91C_BASE_PIOC, RTADP);
                                   break;
			} 
			case 0x0a:
			{
                          		IOCLR(AT91C_BASE_PIOC, RTAD0);
                          		IOSET(AT91C_BASE_PIOC, RTAD1);
                          		IOCLR(AT91C_BASE_PIOC, RTAD2);
                          		IOSET(AT91C_BASE_PIOC, RTAD3);
                          		IOCLR(AT91C_BASE_PIOC, RTAD4);
			     		IOSET(AT91C_BASE_PIOC, RTADP);
                          		break;
			}
			case 0x0b:
			{
                                   IOSET(AT91C_BASE_PIOC, RTAD0);
                                   IOSET(AT91C_BASE_PIOC, RTAD1);
                                   IOCLR(AT91C_BASE_PIOC, RTAD2);
                                   IOSET(AT91C_BASE_PIOC, RTAD3);
                                   IOCLR(AT91C_BASE_PIOC, RTAD4);
			      		IOCLR(AT91C_BASE_PIOC, RTADP);
                                   break;
			}
			case 0x0c:
			{
                                   IOCLR(AT91C_BASE_PIOC, RTAD0);
                                   IOCLR(AT91C_BASE_PIOC, RTAD1);
                                   IOSET(AT91C_BASE_PIOC, RTAD2);
                                   IOSET(AT91C_BASE_PIOC, RTAD3);
                                  	IOCLR(AT91C_BASE_PIOC, RTAD4);
			      		IOSET(AT91C_BASE_PIOC, RTADP);
                                   break;
			}
			case 0x0d:
			{
			      		IOSET(AT91C_BASE_PIOC, RTAD0);
                          		IOCLR(AT91C_BASE_PIOC, RTAD1);
                          		IOSET(AT91C_BASE_PIOC, RTAD2);
                          		IOSET(AT91C_BASE_PIOC, RTAD3);
                          		IOCLR(AT91C_BASE_PIOC, RTAD4);
			      		IOCLR(AT91C_BASE_PIOC, RTADP);
			      		break;
			}
			case 0x0e:
			{
			      		IOCLR(AT91C_BASE_PIOC, RTAD0);
                          		IOSET(AT91C_BASE_PIOC, RTAD1);
                          		IOSET(AT91C_BASE_PIOC, RTAD2);
                          		IOSET(AT91C_BASE_PIOC, RTAD3);
                          		IOCLR(AT91C_BASE_PIOC, RTAD4);
			      		IOCLR(AT91C_BASE_PIOC, RTADP);
			      		break;
			}
			case 0x0f:
			{
			      		IOSET(AT91C_BASE_PIOC, RTAD0);
                          		IOSET(AT91C_BASE_PIOC, RTAD1);
                          		IOSET(AT91C_BASE_PIOC, RTAD2);
                          		IOSET(AT91C_BASE_PIOC, RTAD3);
                          		IOCLR(AT91C_BASE_PIOC, RTAD4);
			      		IOSET(AT91C_BASE_PIOC, RTADP);
		             		break;
			}
			case 0x10:
			{
			      		IOCLR(AT91C_BASE_PIOC, RTAD0);
                          		IOCLR(AT91C_BASE_PIOC, RTAD1);
                          		IOCLR(AT91C_BASE_PIOC, RTAD2);
                          		IOCLR(AT91C_BASE_PIOC, RTAD3);
                          		IOSET(AT91C_BASE_PIOC, RTAD4);
			      		IOCLR(AT91C_BASE_PIOC, RTADP);
			      		break;
			}
			case 0x11:
			{
				IOSET(AT91C_BASE_PIOC, RTAD0);
                            IOCLR(AT91C_BASE_PIOC, RTAD1);
                            IOCLR(AT91C_BASE_PIOC, RTAD2);
                            IOCLR(AT91C_BASE_PIOC, RTAD3);
                            IOSET(AT91C_BASE_PIOC, RTAD4);
			        IOSET(AT91C_BASE_PIOC, RTADP);
	                        break;
			}
			case 0x12:
			{
				IOCLR(AT91C_BASE_PIOC, RTAD0);
                            IOSET(AT91C_BASE_PIOC, RTAD1);
                            IOCLR(AT91C_BASE_PIOC, RTAD2);
                            IOCLR(AT91C_BASE_PIOC, RTAD3);
                            IOSET(AT91C_BASE_PIOC, RTAD4);
                            IOSET(AT91C_BASE_PIOC, RTADP);
                            break;
			}
			case 0x13:
			{
				IOSET(AT91C_BASE_PIOC, RTAD0);
                            IOSET(AT91C_BASE_PIOC, RTAD1);
                            IOCLR(AT91C_BASE_PIOC, RTAD2);
                            IOCLR(AT91C_BASE_PIOC, RTAD3);
                            IOSET(AT91C_BASE_PIOC, RTAD4);
                            IOCLR(AT91C_BASE_PIOC, RTADP);
                            break;
			}
			case 0x14:
			{
				IOCLR(AT91C_BASE_PIOC, RTAD0);
                            IOCLR(AT91C_BASE_PIOC, RTAD1);
                            IOSET(AT91C_BASE_PIOC, RTAD2);
                            IOCLR(AT91C_BASE_PIOC, RTAD3);
                            IOSET(AT91C_BASE_PIOC, RTAD4);
                            IOSET(AT91C_BASE_PIOC, RTADP);
                            break;
			}
			case 0x15:
			{
				IOSET(AT91C_BASE_PIOC, RTAD0);
                            IOCLR(AT91C_BASE_PIOC, RTAD1);
                            IOSET(AT91C_BASE_PIOC, RTAD2);
                            IOCLR(AT91C_BASE_PIOC, RTAD3);
                            IOSET(AT91C_BASE_PIOC, RTAD4);
				IOCLR(AT91C_BASE_PIOC, RTADP);
                                            break;
			}
			case 0x16:
			{
                            IOCLR(AT91C_BASE_PIOC, RTAD0);
                            IOSET(AT91C_BASE_PIOC, RTAD1);
                            IOSET(AT91C_BASE_PIOC, RTAD2);
                            IOCLR(AT91C_BASE_PIOC, RTAD3);
                            IOSET(AT91C_BASE_PIOC, RTAD4);
                            IOCLR(AT91C_BASE_PIOC, RTADP);
                            break;
			}
			case 0x17:
			{
				IOSET(AT91C_BASE_PIOC, RTAD0);
                            IOSET(AT91C_BASE_PIOC, RTAD1);
                            IOSET(AT91C_BASE_PIOC, RTAD2);
                            IOCLR(AT91C_BASE_PIOC, RTAD3);
                            IOSET(AT91C_BASE_PIOC, RTAD4);
				IOSET(AT91C_BASE_PIOC, RTADP);
				break;
			}
			case 0x18:
			{
				IOCLR(AT91C_BASE_PIOC, RTAD0);
                            IOCLR(AT91C_BASE_PIOC, RTAD1);
                            IOCLR(AT91C_BASE_PIOC, RTAD2);
                            IOSET(AT91C_BASE_PIOC, RTAD3);
                            IOSET(AT91C_BASE_PIOC, RTAD4);
				IOSET(AT91C_BASE_PIOC, RTADP);
				break;
			}
			case 0x19:
			{
				IOSET(AT91C_BASE_PIOC, RTAD0);
                            IOCLR(AT91C_BASE_PIOC, RTAD1);
                            IOCLR(AT91C_BASE_PIOC, RTAD2);
                            IOSET(AT91C_BASE_PIOC, RTAD3);
                            IOSET(AT91C_BASE_PIOC, RTAD4);
				IOCLR(AT91C_BASE_PIOC, RTADP);
				break;
			}
			case 0x1a:
			{
				IOCLR(AT91C_BASE_PIOC, RTAD0);
                            IOSET(AT91C_BASE_PIOC, RTAD1);
                            IOCLR(AT91C_BASE_PIOC, RTAD2);
                            IOSET(AT91C_BASE_PIOC, RTAD3);
                            IOSET(AT91C_BASE_PIOC, RTAD4);
				IOCLR(AT91C_BASE_PIOC, RTADP);
				break;
			}
			case 0x1b:
			{
				IOSET(AT91C_BASE_PIOC, RTAD0);
                            IOSET(AT91C_BASE_PIOC, RTAD1);
                            IOCLR(AT91C_BASE_PIOC, RTAD2);
                            IOSET(AT91C_BASE_PIOC, RTAD3);
                            IOSET(AT91C_BASE_PIOC, RTAD4);
				IOSET(AT91C_BASE_PIOC, RTADP);
				break;
			}
			case 0x1c:
			{
				IOCLR(AT91C_BASE_PIOC, RTAD0);
                            IOCLR(AT91C_BASE_PIOC, RTAD1);
                            IOSET(AT91C_BASE_PIOC, RTAD2);
                            IOSET(AT91C_BASE_PIOC, RTAD3);
                            IOSET(AT91C_BASE_PIOC, RTAD4);
				IOCLR(AT91C_BASE_PIOC, RTADP);
				break;
			}
			case 0x1d:
			{
				IOSET(AT91C_BASE_PIOC, RTAD0);
                            IOCLR(AT91C_BASE_PIOC, RTAD1);
                            IOSET(AT91C_BASE_PIOC, RTAD2);
                            IOSET(AT91C_BASE_PIOC, RTAD3);
                            IOSET(AT91C_BASE_PIOC, RTAD4);
				IOSET(AT91C_BASE_PIOC, RTADP);
                                            break;
			}
			case 0x1e:
			{
				IOCLR(AT91C_BASE_PIOC, RTAD0);
                            IOSET(AT91C_BASE_PIOC, RTAD1);
                            IOSET(AT91C_BASE_PIOC, RTAD2);
                            IOSET(AT91C_BASE_PIOC, RTAD3);
                            IOSET(AT91C_BASE_PIOC, RTAD4);
				IOSET(AT91C_BASE_PIOC, RTADP);
				break;
			}
			case 0x1f:
			{
				IOSET(AT91C_BASE_PIOC, RTAD0);
                            IOSET(AT91C_BASE_PIOC, RTAD1);
                            IOSET(AT91C_BASE_PIOC, RTAD2);
                            IOSET(AT91C_BASE_PIOC, RTAD3);
                            IOSET(AT91C_BASE_PIOC, RTAD4);
				IOCLR(AT91C_BASE_PIOC, RTADP);
				break;
			}
      }
}

