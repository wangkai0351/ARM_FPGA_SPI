#ifndef _NETDOWNLOAD_H
#define _NETDOWNLOAD_H

//#include "cardConfig.h"
#include "global.h"
//QQQ FLASH大小需要调整 
#define FLASH_EPABOOT_ADDR      FLASH_BEGIN
#define FLASH_EPAPRO_BACK_ADDR  0x203c00    //15k
#define FLASH_EPAPRO_ADDR       EPAPRO_FLASH_BEG
#define FLASH_USERPRO_BACK_ADDR 0x23c000    //EPAPRO_FLASH_END+1
#define FLASH_USERPRO_ADDR      EPALIB_USERPRO_FLASH_BEG

//更新程序时，上位机下发给卡件的数据区的crc校验和的密码子
#define PRO_PASSWORD    0x55555555

//定义上位机发送的数据报文的命令
#define UPPROBEG    0x12345678          //初始化设备烧录得命令
#define UPPROING    0x5a5a5a5a          //更新设备程序的命令
#define UPPROEND    0x87654321          //更新结束程序的命令
#define RESTART     0x33333333          //设备硬复位的命令
#define GETLIST     0x44444444          //获得设备链表的命令
#define CNFPRO      0x55555555          //上位机确认设备程序有效的命令

//设备发送的反馈信息中的请求类型
#define PACK_CHECKSUM_ERR   0x1234      //上位机发送的报文的头校验出错
#define DATA_CRC_ERR        0x4321      //上位机发送的报文的数据CRC校验出错
#define DATA_MIS_REQ        0x5555      //设备收报文时，出现了丢包的请求
#define WRITE_FLASH_ERR     0x6666      //写Flash出错了
#define END_NO_ERR          0x0000      //烧录结束时，设备没有出错
#define END_ERR             0xffff      //烧录结束时，更新还没完成
#define UPPRO_READY         0xa5a5      //初始化烧录程序时，设备已经准备好了
#define RSP_RESTART         0x0001      //设备重启命令的回复字
#define RSP_GETLIST         0x1015      //获得设备链表的回复字
#define RSP_CNFPRO          0x1111      //确认程序时，返回的确认有效命令字

//设备状态信息的定义
#define UPPRO_NO_STAT       0x0000      //设备未进行过任何的更新程序操作的状态，设备复位后的状态
#define UPPRO_ERR_STAT      0x1111      //设备更新程序时，发生了错误，但错误还没消除的状态
#define UPPRO_FLASH_STAT    0xa5a5      //正在更新程序的状态
#define UPPRO_END_STAT      0xaaaa      //更新程序结束了。

//命令字对的设备操作权限(由设备类型与发送的设备类型决定)
#define CMD_EFFECT      0x01            //该字段只作为内部使用，与上位机的交互不相关
#define CMD_ERR         0x02
#define CMD_PROEFFECT   0x03

typedef struct
{
    Uint8   DMAC[6], SMAC[6];
    Uint16  type, packCheckSum;         // 16字节 packCheckSum计算包头的和校验。

    Uint32  DestIP;
    Uint32  SourceIP;
    Uint16  CardType;
    Uint16  CardVery;
    Uint16  upProStat;
    Uint16  ErrReqType;

    Uint32  command;                    //命令字    // 16字节
    Uint32  SNO;                        //序列号
    Uint32  datalength;
    Uint32  dataCRC;                    //先计算data的crc，最后计算packCheckSum

    Uint16  data[256];                  //512字节
} UPDATAFLASHPRO;

typedef struct
{
    Uint8   DMAC[6];
    Uint8   SMAC[6];
    Uint16  type;
    Uint16  packCheckSum;

    Uint32  DestIP;
    Uint32  SourceIP;
    Uint16  CardType;
    Uint16  CardVery;
    Uint16  upProStat;
    Uint16  ErrReqType;

    Uint32  command;                    //  //
    Uint32  SNO;                        //
    Uint32  datalength;
    Uint32  dataCRC;                    //
} NetProHeader;

extern Uint16   upProStat;              //卡件更新程序时的状态

extern Uint32   checkCMD(UPDATAFLASHPRO *data32);
extern Uint16   FlashProChksum(Uint16 *buffer, Uint16 size);
extern void sendErrReq(UPDATAFLASHPRO *updata, Uint16 errType, struct netif *device);
extern void UpDataFlashProgram(Uint32 *pData32, struct netif *device);
extern void BlockErase(Uint32 addr);
extern void FlashErase(Uint32 addr);

// 供外部完成的函数，主要为了移植方便

//extern void upProBegHandle(void );
#define upProBegHandle()    (OnlyNET())
extern void proFlashBegHandle(void);

extern void flashWriteErrHandle(void);

extern Uint32   writeNetDataToProFlash(Uint32 startAddr, Uint32 *data, Uint32 length);

extern void cardDelay(void);
//extern void UNcardDelay(void);

extern void hardwareReset(void);

extern Uint32   GetCardIP(void);

//extern void upProBegHandle(void );          //外部需要移植的函数
//extern void flashWriteErrHandle(void);  //外部需要移植的函数
//extern void cardDelay(void);
//extern void UNcardDelay(void);
//extern void hardwareReset(void);

extern Uint32   cardIP;
//extern Uint32   GetCardIP(void);

extern void EarseEPAProInf(void);       //擦除EPA仿真平台程序信息区内容
extern void EarseEPAUserProInf(void);   //擦除EPA用户程序信息区内容
void LocalHexInfInit(void);
//上位机发送更新卡件命令时的命令字节。
void UpDataDeviceIDToFlash(void);
extern void upProInit(void);
void    dealTestPack(Uint32 *pd32, struct netif *netif);    //更新程序、测试报文

extern void netTestInit(void);
extern void UNBreakPoint(void);
extern void OnlyNET(void);
extern Uint32   IsNetTestPack(Uint8 *p);
extern void DealNetTest(Uint32 *pData32, struct netif *device);
#endif
