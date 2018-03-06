#ifndef _MAIN_IN_H
#define _MAIN_IN_H
/*
#include "main.h"
#include "peripherals\emac\emac.h"
#include "wdt.h"
#include "pio.h"

//#include "usart.h"
#include "usart_dw.h"
#include "gsm.h"

#include "irq.h"

#include "timer.h"
//#include "netDownload.h"

#include "queue.h"
*/
extern unsigned char EmacSendTemp[256];
extern void EmacAddrMatchInit(Uint8 *MACAddress);
extern void GSMRecTask(void *data);
extern void suspendAllTask(void);

#endif