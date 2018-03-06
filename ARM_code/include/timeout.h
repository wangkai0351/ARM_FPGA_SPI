#ifndef _TIME_OUT_H
#define _TIME_OUT_H

#include "Global.h"

#define TIME_ERR_NO_EXIST   1
#define TIME_ERR_OK         0

typedef struct  g_timeout
{
    struct g_timeout    *next;  /* 指向下一个定时数据结构*/
    Uint32  offset;             /*暂时不使用 */
    Uint32  macroTime;          /*单位10ms，宏周期时间，如果该值为0，表示只执行一次*/
    Uint32  time;               /* 单位10ms，设置延时的时间+当前系统时间 */
    void (*h) (void *user_arg); /* 延时时间到时，执行的回调函数 */
    void    *user_arg;          /*用户在调用函数时，传入的参数，作为回调函数的用户参数*/
} S_G_TIMEOUT;


void TimeOutInit(void);
void TimeOutArrive(void);
Uint32 ChangeTimeOut(S_G_TIMEOUT *ptimeout, Uint32 newDelay);
S_G_TIMEOUT *TimeOutInsert(Uint32 macroCycle, Uint32 offset, void (*h) (void *user_arg), void *arg);
int    DeleteFromTimeChain(S_G_TIMEOUT *ptimeout);

#endif
