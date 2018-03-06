/************************************************************************/

/* Main.h                               */

/************************************************************************/
#ifndef _MAIN_H
#define _MAIN_H
//#include "EPA_Ethernet.h"
//#include "global.h"
//#include "includes.h"
//#include "lwip.h"


// Task stack size
#define TASK_STK_SIZE   512

#define MAX_FILE_QUEUE      20
#define SYS_INIT_TASK_PRIO  (2)

#define MAX_GSM_QUEUE  20

#define MYextern    extern

//调度队列处理
#define Q_ERR       0
#define Q_FULL      0
#define Q_NO_ERR    2

typedef struct  boot_data_struct
{
    Uint32  ValidFlag[8];
    Uint32  resetErrContinueC;  /*连续发生错误的复位次数，连续发生错误的标志为在1分钟内，设备发生软服务*/
    Uint32  softRestC;          /*设备软复位总的次数*/
    Uint32  PowerOnFlag;
    Uint32  NicRstC;
    Uint32  inf_crc32;
} BOOT_DATA;
MYextern BOOT_DATA  BootData;

typedef struct
{
    struct pbuf *p;             //报文的pbuf
    struct netif    *netif;     //指定发送的网口
} DQ_Q;

typedef struct
{
    DQ_Q    *DQStart;
    DQ_Q    *DQEnd;
    DQ_Q    *DQFront;
    DQ_Q    *DQRear;
    Uint16  DQSize;
    Uint16  DQEntries;
} DQ_M;

//#define GetCheckCode(a, b)  crc16_check(a, b)
/****************************************************************/

/*      CONSTANTS                   */

/****************************************************************/
typedef struct
{
    Uint32  reserved0[4];

    Uint32  proFlashBase;
    Uint32  proFlashCRC32;
    Uint32  proRAMBase;
    Uint32  proRAMLength;

    Uint32  proRAMZIBase;
    Uint32  proRAMZILength;
    Uint32  runCnfData;
    Uint32  proCnfData;

    Uint16  cardType;
    Uint16  cardVery;
    Uint32  proDownloadC;
    Uint32  infCRC;     //写入确认信息时,信息区的CRC32校验码
    Uint32  infCRC32;   //刚开始烧录程序时,信息区的CRC校验码
} S_EPAProInf;

#define Pro_OK                      0x55aa
#define Pro_Err                     0xa5a5

//QQQ用户FLASH和其他地址变量需要调整
#define  FLASH_BEGIN            0x200000
#define  FLASH_END              0x27FFFF

#define EPALIB_USERPRO_FLASH_BEG    0x240000
#define EPALIB_USERPRO_FLASH_END    0x27FFFF
#define EPALIB_USERPRO_FLASH_Blocks ((EPALIB_USERPRO_FLASH_END - EPALIB_USERPRO_FLASH_BEG + 1)/4096)


#define EPAPRO_FLASH_BEG            0x204000
#define EPAPRO_FLASH_END            0x23BFFF
#define EPAPRO_FLASH_Blocks         ((EPAPRO_FLASH_END - EPAPRO_FLASH_BEG + 1)/4096)

/*---------------------------------------SRAM地址定义-------------------------------------*/
#define EPALIB_USERPRO_SRAM_LENGTH  0
#define EPALIB_USERPRO_SRAM_END     0x307FFF    //32k
#define EPALIB_USERPRO_SRAM_BEG     (EPALIB_USERPRO_SRAM_END - EPALIB_USERPRO_SRAM_LENGTH)

#define EPAPRO_SRAM_BEG             0x300000
#define EPAPRO_SRAM_END             (0x307FFF - EPALIB_USERPRO_SRAM_LENGTH)
#define EPAPRO_SRAM_LENGTH          (0x307FFF - EPALIB_USERPRO_SRAM_LENGTH) //0x3c000

#define EPALIB_USERPRO_OK           0x55aa
#define EPALIB_USERPRO_ERR          0xaa55

//extern Uint32   ResetPowerOn;

typedef struct
{
    Uint32  reserved0;
    Uint32  addrInitMain;
    Uint32  addrMain;
    Uint32  reserved1;

    Uint32  Flash_SRAM_Base;
    Uint32  SRAM_CRC32;
    Uint32  SRAM_Base;
    Uint32  SRAM_Length;

    Uint32  SRAM_ZI_Base;
    Uint32  SRAM_ZI_Length;
    Uint32  UserProChkID;
    Uint32  CORP_ID;

    Uint16  cardType;
    Uint16  CardVery;
    Uint32  proDownloadC;
    Uint32  LIBUSERPro_OKFlag;
    Uint32  infCRC32;
} S_UserProInf_OLD;

typedef struct
{
    Uint32  reserved0;
    Uint32  addrInitMain;
    Uint32  addrMain;
    Uint32  reserved1;

    Uint32  Flash_SRAM_Base;
    Uint32  SRAM_CRC32;
    Uint32  SRAM_Base;
    Uint32  SRAM_Length;

    Uint32  SRAM_ZI_Base;
    Uint32  SRAM_ZI_Length;
    Uint32  UserProChkID;
    Uint32  reserved2;

    Uint16  cardType;
    Uint16  CardVery;
    Uint32  proDownloadC;
    Uint32  reserved3;
    Uint32  reserved4;

    Uint8   DeviceID_Manufac_ID[6];
    Uint8   DeviceID_Dev_Type[4];
    Uint8   DeviceID_UID1_DDType[6];
    Uint8   DeviceID_UID2_ID[16];

    Uint8   DeviceType;
    Uint8   reserved5;
    Uint16  DeviceVersion;
    Uint32  reserved6;
    Uint32  reserved7;
    Uint32  infCRC32;
} S_UserProInf;



typedef struct          //<256	62*4=252
{
    Uint32  ARMErrFlag; //标记是否已经还没法送出去，即空白的空间
    Uint32  ExpRN[13];
    Uint32  ExpSP, ExpLR, ExpPC, CPSR, PCCodeData;              //PCCodeData,当前PC指令值
    //*******************28**********************
    //Uint32 SPBuf[8];			//触发异常的指令的SP存放的值。
} ExceptionBugData;

extern ExceptionBugData ExpBug;                                 //软件中断
extern OS_EVENT    *GSMRecSem;
extern DQ_M    GSMQM;
extern DQ_Q    GSMQ[MAX_GSM_QUEUE];



//Uint32  crc32_check(const Uint8 *scm_ptr, int scm_length, Uint32 relationkey);

//Uint32  ether_crc32(unsigned char *buf, int len);

//Uint16  crc16_check(Uint8 *pBuf, Uint16 length);

#endif
