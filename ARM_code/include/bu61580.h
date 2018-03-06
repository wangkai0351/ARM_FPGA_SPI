#ifndef _BU61850_H
#define _BU61850_H


#define BU61580_RAM_BASE 0x30002000
#define BU61580_RAM_END 0x30003fff

#define MSG_BLOCK_BASE32 0x30002210
#define MSG_BLOCK_BASE16 0x108
#define MSG_BLOCK_SIZE 38   //short 个数
//#define MonComStack 0x30002204
#define ROLLOVER_TIME  131072//65536//4194000//2097000//131000 //TIME TAG RESOLUTION 8us ,ROLLOVER TIME 524ms
#define TIME_TAG_RESOLUTION  2//1//32//8 //64
#define RXBUFFERSIZE  80   //接收缓冲区大小	
/*寄存器地址*/
#define REG_INT_MASK 0x30000000
#define REG_CFG1 0x30000002
#define REG_CFG2 0x30000004
#define REG_RESET 0x30000006
#define REG_COM_STACK_POINT 0x30000006 //
#define REG_CTRL_WORD 0x30000008  //BC RT
#define REG_TIME_TAG 0x3000000a
#define REG_INT_STAT 0x3000000c
#define REG_CFG3 0x3000000e
#define REG_CFG4 0x30000010
#define REG_CFG5 0x30000012
#define REG_DATA_STACK_ADDR 0x30000014  //RT BM
#define REG_FRAME_RemainTime 0x30000016  //BC
#define REG_FRAME_RemainTime2Next 0x30000018  //BC
#define REG_FRAME_TIME 0x3000001a
#define REG_STATUS_WORD 0x3000001c  //RT
#define REG_BIT_WORD 0x3000001e  //RT

/*RAM 地址*/
#define RTCom_A_Stack 0x30002000
#define STACK_A_P    0x30002200
#define CMD_ILIEGAL_TABLE 0x30002600
#define TX_DATA_ADDR 0x30002800
#define MSG_A_CNT  0x30002202
#define SAD_BSY_TABLE 0x30002480 //SUBADDRESS BUSY TABLE 起始地址
#define Sel_Mon_LookupTab 0x30002500 //Selective Monitor Lookup Table起始地址
#define MT_CMD_Stack 0x30002800
#define MT_DATA_Stack 0x30003000

typedef struct
{
	Uint16 PStackA;
	Uint16 MsgACnt;
	Uint16 InitPStackA;
	Uint16 InitMsgACnt;
	Uint16 PStackB;
	Uint16 MsgBCnt;
	Uint16 InitPStackB;
	Uint16 InitMsgBCnt;	
}ExpandModeRAM;

typedef struct
{  
      Uint16 pRTComStackA;           //RT Command Stack Pointer A
      Uint16 RESERVE0[3];
      Uint16 pRTCStackB;           //RT Command Stack Pointer B
      Uint16 RESERVE1[3];
      Uint16 ModSltIntTab[8];        //Mode Code Selective Interrupt Table
      Uint16 ModCodData[48];         //Mode Code data
      Uint16 RXLookupTabA[32];     //Receive (/Broadcast)subaddress  Lookup Pointer Table 
      Uint16 TXLookupTabA[32];    //Transmit subaddress Lookup Pointer Table
      Uint16 BCSTLookuptabA[32]; //Broadcast Lookup Pointer Table
      Uint16 SubadContrWordTab[32];//Subaddress Control Word Table 
      Uint16 LookupTabB[128];         //Lookup Table B
      Uint16 BsyBitLookupTab[8]; //Busy Bit Lookup Table
      Uint16 RESERVE2[24];
      Uint16 DataBlock0[32];
      Uint16 DataBlock1[32];
      Uint16 DataBlock2[32];
      Uint16 DataBlock3[32];
      Uint16 DataBlock4[32];
      Uint16 CmdIlglzTab[256];        //Command Illegalizing Table
      Uint16 DataBlock5_100[3072]; //Data Block 5...100
}RTExpandModeRAM;

typedef struct
{
      Uint16 RTComStackA[256];         //RT Command Stack A
      Uint16 pRTComStackA;              //RT Command Stack Pointer A (fixed location)
      Uint16 RESERVE0;               
      Uint16 pMonComStackA;           //Monitor Command Stack Pointer A
      Uint16 pMonDatStackA;            //Monitor Data Stack Pointer A
      Uint16 pStackB;                        //Stack Pointer B
      Uint16 RESERVED1;
      Uint16 pMonComStackB;          //Monitor Command Stack Pointer B
      Uint16 pMonDatStackB;           //Monitor Data Stack Pointer B
      Uint16 ModCodSeltIntTab[8];  //Mode Code Selective Interrupt Table
      Uint16 ModCodData[48];         //Mode Code data
      Uint16 RXLookupTabA[32];     //Receive (/Broadcast)subaddress  Lookup Pointer Table 
      Uint16 TXLookupTabA[32];    //Transmit subaddress Lookup Pointer Table
      Uint16 BCSTLookuptabA[32]; //Broadcast Lookup Pointer Table
      Uint16 SubadContrWordTab[32];//Subaddress Control Word Table 
      Uint16 LookupTabB[128];    
      Uint16 BsyBitLookupTab[8];  //Busy Bit Lookup Table
      Uint16 RESERVE2[24];    
      Uint16 RTDATBlock0[32];      //RT Data Block 0 or Registers
      Uint16 SelMonLookupTab[128]; //Selective Monitor Lookup Table
      Uint16 ComIllegTab[256];     //Command Illegalizing Table
      Uint16 MonComStackA[1024];//Monitor Command Stack A 
      Uint16 MonDatStack[2048];   //Monitor Data Stack
      Uint16 DataBlock[8192];
}BMExpandModeRAM;

typedef struct Monitor_Command_Stack
{
	Uint8   UseFlag;
       Uint16 BlockStatusWord;
       Uint32 NowTime_L;
	Uint32 NowTime_H;
	Uint16 pDataBlock;
	Uint16 BMData[32];
	Uint16 RecevedCommandWord;
	struct Monitor_Command_Stack *next;
}MonitorCommandStack;

typedef struct RT_Command_Stack
{
//	Uint8   UseFlag;
//       Uint16 BlockStatusWord;
//       Uint32 NowTime_L;
//	Uint32 NowTime_H;
	Uint16 pDataBlock;
	Uint16 RTData[32];
//	Uint16 RecevedCommandWord;
//	struct Monitor_Command_Stack *next;
}RTCommandStack;

typedef struct
{
	Uint16 BlockStatusWord;
	Uint16 TimeTagWord;
	Uint16 MsgGapWord;
	Uint16 MsgBlockAddr;
}STACK_STRU;

typedef struct
{
	Uint16 CtrWord;
	Uint16 RComWord;
	Uint16 Data[32];
	Uint16 LastDataBack;
	Uint16 StatusWord;
}BC_TO_RT_STRU;

typedef struct
{
	Uint16 CtrWord;
	Uint16 TComWord;
	Uint16 TComWordBack;
	Uint16 StatusWord;	
	Uint16 Data[32];
}RT_TO_BC_STRU;

typedef struct
{
	Uint16 CtrWord;
	Uint16 RComWord;
	Uint16 TComWord;	
	Uint16 TComWordBack;
	Uint16 TxRTStatusWord;		
	Uint16 Data[32];
	Uint16 RxRTStatusWord;
}RT_TO_RT_STRU;

typedef struct
{
	Uint16 CtrWord;
	Uint16 RComWord;
	Uint16 TComWordBack;
	Uint16 StatusWord;
	Uint16 Data;
}BC_TO_RT_CMD_STRU;

typedef struct MemBlockStruct {
	Uint16 absaddr;
	Uint8  status;
	Uint16 size;
	Uint16 gp; /* gaptime changed to gp "general purpose" [modified 01-JUN-1995] */
	Uint16 condition;
	struct MemBlockStruct *next,*prev;

	/* New Members added for BC Major Frame Support */
	Uint16 count;
	Uint16 blkstatus;
	Uint16 timetag;
} *MemBlockHandle, MemBlockType;

/* BC CONSTANTS -----------------------------------------------------------*/

/* t/~r bit in command word */
#define TX_CMD 0x0400
#define RX_CMD 0x0000

/* bc control word */
#define CW_RTTORT          0x0001
#define CW_BRDCST          0x0002
#define CW_MODE            0x0004
#define CW_1553A           0x0008
#define CW_1553B           0x0000
#define CW_EOMINT          0x0010
#define CW_MASKBRDCST      0x0020
#define CW_SELFTEST        0x0040
#define CW_CHANNELA        0x0080
#define CW_CHANNELB        0x0000
#define CW_RETRYENA        0x0100
#define CW_RESERVEDMASK    0X0200
#define CW_TERMFLAGMASK    0x0400
#define CW_FLAGMASK        0x0800
#define CW_BUSYMASK        0x1000
#define CW_REQMASK         0x2000
#define CW_MSGERRMASK      0x4000

/* bc block status word */
#define BC_EOM             0x8000
#define BC_SOM             0x4000
#define BC_CHANNELB        0x2000
#define BC_ERR             0x1000
#define BC_STATSET         0x0800
#define BC_FORMATERR       0x0400
#define BC_RESP            0x0200
#define BC_LOOPFAIL        0X0100
#define BC_MASKSTATSET     0X0080
#define BC_RETRY1          0X0040
#define BC_RETRY0          0X0020
#define BC_GDDATATX        0X0010
#define BC_WRNGSTADNGAP    0X0008
#define BC_WRDCNTERR       0X0004
#define BC_INCSYNC         0X0002
#define BC_INVALIDWD       0X0001

/* gaptime option */
#define BU_BCNOGAP         0x0001

/* message condition */
#define     BU_BCALWAYS         0x0000
#define     BU_BCNTIMES         0x4000
#define     BU_BCEVERYNTHTIME   0x8000
#define     BU_BCUSERCONDITION  0xC000

/* command stack size */
#define     BC_CMD256      0x0000
#define     BC_CMD512      0x2000
#define     BC_CMD1024     0x4000
#define     BC_CMD2048     0x6000

/* number of bc retries */
#define     BU_BCRETRY0    0xffff
#define     BU_BCRETRY1    0x0000
#define     BU_BCRETRY2    0x0008

/* Active Area */
#define     BU_BCAREASTKA    0x0000
#define     BU_BCAREASTKB    0x2000

/* bus to use on retry */
#define     BU_BCRETRYSAME 0x0000
#define     BU_BCRETRYALT  0x0001

/* trigger to run bc */
#define     BC_TRIGNONE    0x0000
#define     BC_TRIGEXT     0x0080
#define     BC_TRIGINT     0x0040

/* frame buffers */
#define     BU_BCFRMBUFA   0x0000
#define     BU_BCFRMBUFB   0x2000

/* desciptor stack pointers and counters */
#define     BC_STKA        0x0100
#define     BC_MSGCNTA     0x0101
#define     BC_INITSTKA    0x0102
#define     BC_INITMSGCNTA 0x0103
#define     BC_STKB        0x0104
#define     BC_MSGCNTB     0x0105
#define     BC_INITSTKB    0x0106
#define     BC_INITMSGCNTB 0x0107

/* run minor macro definitions */
#define     BU_BCSINGLE     0
#define     BU_BCREPEAT     1

#endif