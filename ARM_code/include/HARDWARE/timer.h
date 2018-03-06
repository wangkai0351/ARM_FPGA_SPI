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
#define SECOND_NSUNIT   1000000000  /* 1S�Ӷ���NS */
#define SECOND_USUNIT   1000000     /* 1S���ж��ٸ�us */

#define MS_NSUNIT       1000000     /* 1MS�ж��ٸ�NS */
#define MS_USUNIT       1000        /* 1MS�ж��ٸ�us */

/*
*********************************************************************************************************
*                                          HARDWARE REAL TIME DATATYPE
*********************************************************************************************************
*/

typedef struct recPtpTime
{
    struct recPtpTime   *next;
    TIMESTAMP   time;           /*���Լ�¼���յ����ĵ�ʱ��*/
}REC_PTP_TIME;
/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/* ϵͳʱ�ӣ���ΪPTP��UCOS-II��ʱ������ */
extern void EPAPTPSYSTimeInit(void);
extern Uint32   GetSysTickTime(void);
extern Uint32 GetPtpTickTime(int channel);
extern Uint32   ISPTPSysTimePend(void);

extern REC_PTP_TIME *pRecPtpTime;

/* �����¼�ʱ�ӣ���ΪCSME���ȵ����� */
extern void EPACSMETimeInit(void);
extern Uint32   GetCSMETickTime(void);
extern void SetCSMETime(Uint32);
#endif
 
 