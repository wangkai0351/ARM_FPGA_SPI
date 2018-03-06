#ifndef _TIMER_H
#define _TIMER_H

//#include "std.h"
#include "Global.h"
#include "at91sam9xe_out.h"

/*
*********************************************************************************************************
*                                             MISCELLANEOUS
*********************************************************************************************************
*/
#define SECOND_NSUNIT   1000000000  /* 1S钟多少NS */
#define SECOND_USUNIT   1000000     /* 1S钟有多少个us */

#define MS_NSUNIT       1000000     /* 1MS有多少个NS */
#define MS_USUNIT       1000        /* 1MS有多少个us */

/*
*********************************************************************************************************
*                                          HARDWARE REAL TIME DATATYPE
*********************************************************************************************************
*/

typedef struct recPtpTime
{
    struct recPtpTime   *next;
    TIMESTAMP   time;           /*用以记录接收到报文的时间*/
}REC_PTP_TIME;
/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/* 系统时钟，作为PTP和UCOS-II的时间驱动 */
extern void EPAPTPSYSTimeInit(void);
extern Uint32   GetSysTickTime(void);
extern Uint32 GetPtpTickTime(int channel);
extern Uint32   ISPTPSysTimePend(void);

extern REC_PTP_TIME *pRecPtpTime;

/* 调度事件时钟，作为CSME调度的驱动 */
extern void EPACSMETimeInit(void);
extern Uint32   GetCSMETickTime(void);
extern void SetCSMETime(Uint32);
#endif
 
 