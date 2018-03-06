#ifndef _TIME_OUT_H
#define _TIME_OUT_H

#include "Global.h"

#define TIME_ERR_NO_EXIST   1
#define TIME_ERR_OK         0

typedef struct  g_timeout
{
    struct g_timeout    *next;  /* ָ����һ����ʱ���ݽṹ*/
    Uint32  offset;             /*��ʱ��ʹ�� */
    Uint32  macroTime;          /*��λ10ms��������ʱ�䣬�����ֵΪ0����ʾִֻ��һ��*/
    Uint32  time;               /* ��λ10ms��������ʱ��ʱ��+��ǰϵͳʱ�� */
    void (*h) (void *user_arg); /* ��ʱʱ�䵽ʱ��ִ�еĻص����� */
    void    *user_arg;          /*�û��ڵ��ú���ʱ������Ĳ�������Ϊ�ص��������û�����*/
} S_G_TIMEOUT;


void TimeOutInit(void);
void TimeOutArrive(void);
Uint32 ChangeTimeOut(S_G_TIMEOUT *ptimeout, Uint32 newDelay);
S_G_TIMEOUT *TimeOutInsert(Uint32 macroCycle, Uint32 offset, void (*h) (void *user_arg), void *arg);
int    DeleteFromTimeChain(S_G_TIMEOUT *ptimeout);

#endif
