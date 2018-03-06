#ifndef _WDT_H_
#define _WDT_H_

//#include "global.h"
//#include "pio.h"
//#include "cardConfig.h"

#define WDT_1P4SWDT         0x00
#define WDT_600MSWDT        0x10
#define WDT_200MSWDT        0x20
#define WDT_DISABLEWDT      0x30

#define WDT_TIME_SAVE       0x55    //重启后还是开看门狗的
#define WDT_TIME_NOTSAVE    0x11    //重启不开看门狗


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
SWDT_STAT_IDLE,//软件看门狗空闲
SWDT_STAT_SUSPEN,//软件看门狗挂起
SWDT_STAT_RUN//软件看门狗运行
}SWDT_STAT;

typedef struct soft_wach_dog_timer
{
	Uint16 watchDogTimeOut; //看门狗计数超时初值
	Uint16 watchDogTime;//看门狗定时器
	Uint8   watchDogCountBack;//上一次看门狗喂狗计数器
	Uint8   watchDogCount;//看门狗计数器
	SWDT_STAT watchDogState;//看门狗定时器状态
	Uint8   NOUSE8;
}SOFT_WATCH_DOG_TIMER;

typedef enum{//往这里添加软件看门狗ID 
	AtARPCacheScanTask_SWDT_ID,
	AtNetConfigServerStart_SWDT_ID,
	MAX_SWDT_ID 
}SWDT_ID; 

extern void WDT_FeedDog(void);
extern void Soft_HardwareWDTInit(void);
/*系统使用的函数*/
void    WDTDataInit(void);
Uint32  UpDataToWDT(WDT_MSG *msg);

/*用户使用的函数*/
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
