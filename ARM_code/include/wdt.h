#ifndef _WDT_H_
#define _WDT_H_

//#include "global.h"
//#include "pio.h"
//#include "cardConfig.h"

#define WDT_1P4SWDT         0x00
#define WDT_600MSWDT        0x10
#define WDT_200MSWDT        0x20
#define WDT_DISABLEWDT      0x30

#define WDT_TIME_SAVE       0x55    //�������ǿ����Ź���
#define WDT_TIME_NOTSAVE    0x11    //�����������Ź�


#define WDT_ERR_OK		0
#define WDT_ERR_QUEUE_FULL		1
#define WDT_ERR_MEM_OVER	2
#define WDT_ERR_LENTH_OVER	3
#define WDT_ERR_ERR		4
#define WDT_EN	1

typedef struct
{
    Uint8   *buf;
    Uint8  len;
    Uint8  startAddr;
    void (*WCallBack) (Uint32 errType, void *callArg);
    void    *callArg;
} WDT_MSG;

typedef enum{
SWDT_STAT_IDLE,//������Ź�����
SWDT_STAT_SUSPEN,//������Ź�����
SWDT_STAT_RUN//������Ź�����
}SWDT_STAT;

typedef struct soft_wach_dog_timer
{
	Uint16 watchDogTimeOut; //���Ź�������ʱ��ֵ
	Uint16 watchDogTime;//���Ź���ʱ��
	Uint8   watchDogCountBack;//��һ�ο��Ź�ι��������
	Uint8   watchDogCount;//���Ź�������
	SWDT_STAT watchDogState;//���Ź���ʱ��״̬
	Uint8   NOUSE8;
}SOFT_WATCH_DOG_TIMER;

typedef enum{//���������������Ź�ID 
	AtARPCacheScanTask_SWDT_ID,
	AtNetConfigServerStart_SWDT_ID,
	MAX_SWDT_ID 
}SWDT_ID; 

extern void WDT_FeedDog(void);
extern void Soft_HardwareWDTInit(void);
/*ϵͳʹ�õĺ���*/
void    WDTDataInit(void);
Uint32  UpDataToWDT(WDT_MSG *msg);

/*�û�ʹ�õĺ���*/
Uint32  UpDataWDT
        (
            Uint8  startAddr,
            Uint8   *buf,
            Uint8  len,
            void (*WCallBack) (Uint32 errType, void *callArg),
            void *callArg
        );
Uint32  ReadDataFromWDT(Uint8 startAddr, Uint8 *buf, Uint8 len);

/**/
//void    setWDTimeFromWDT(void);
void    WDT_SetTime(Uint8 rstTime);

extern void Soft_HardwareWDTInit(void);
extern Uint8 SoftWdtInit(SWDT_ID SwdtId, Uint32 TimerTop);
extern void SoftWdtFed(SWDT_ID SwdtId);
extern void SuspenWdt(SWDT_ID SwdtId);
extern void RunWdt(SWDT_ID SwdtId);
extern void SoftWdtISR(void);
extern void Fct_ByteWrite(Uint8 wData, Uint16 wAddr);
extern Uint8 Fct_ByteRead(Uint16 rAddr);
extern void Fct_ByteWrite(Uint8 wData, Uint16 wAddr);
#endif
