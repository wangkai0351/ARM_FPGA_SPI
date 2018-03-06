#ifndef _NETDOWNLOAD_H
#define _NETDOWNLOAD_H

//#include "cardConfig.h"
#include "global.h"
//QQQ FLASH��С��Ҫ���� 
#define FLASH_EPABOOT_ADDR      FLASH_BEGIN
#define FLASH_EPAPRO_BACK_ADDR  0x203c00    //15k
#define FLASH_EPAPRO_ADDR       EPAPRO_FLASH_BEG
#define FLASH_USERPRO_BACK_ADDR 0x23c000    //EPAPRO_FLASH_END+1
#define FLASH_USERPRO_ADDR      EPALIB_USERPRO_FLASH_BEG

//���³���ʱ����λ���·�����������������crcУ��͵�������
#define PRO_PASSWORD    0x55555555

//������λ�����͵����ݱ��ĵ�����
#define UPPROBEG    0x12345678          //��ʼ���豸��¼������
#define UPPROING    0x5a5a5a5a          //�����豸���������
#define UPPROEND    0x87654321          //���½������������
#define RESTART     0x33333333          //�豸Ӳ��λ������
#define GETLIST     0x44444444          //����豸���������
#define CNFPRO      0x55555555          //��λ��ȷ���豸������Ч������

//�豸���͵ķ�����Ϣ�е���������
#define PACK_CHECKSUM_ERR   0x1234      //��λ�����͵ı��ĵ�ͷУ�����
#define DATA_CRC_ERR        0x4321      //��λ�����͵ı��ĵ�����CRCУ�����
#define DATA_MIS_REQ        0x5555      //�豸�ձ���ʱ�������˶���������
#define WRITE_FLASH_ERR     0x6666      //дFlash������
#define END_NO_ERR          0x0000      //��¼����ʱ���豸û�г���
#define END_ERR             0xffff      //��¼����ʱ�����»�û���
#define UPPRO_READY         0xa5a5      //��ʼ����¼����ʱ���豸�Ѿ�׼������
#define RSP_RESTART         0x0001      //�豸��������Ļظ���
#define RSP_GETLIST         0x1015      //����豸����Ļظ���
#define RSP_CNFPRO          0x1111      //ȷ�ϳ���ʱ�����ص�ȷ����Ч������

//�豸״̬��Ϣ�Ķ���
#define UPPRO_NO_STAT       0x0000      //�豸δ���й��κεĸ��³��������״̬���豸��λ���״̬
#define UPPRO_ERR_STAT      0x1111      //�豸���³���ʱ�������˴��󣬵�����û������״̬
#define UPPRO_FLASH_STAT    0xa5a5      //���ڸ��³����״̬
#define UPPRO_END_STAT      0xaaaa      //���³�������ˡ�

//�����ֶԵ��豸����Ȩ��(���豸�����뷢�͵��豸���;���)
#define CMD_EFFECT      0x01            //���ֶ�ֻ��Ϊ�ڲ�ʹ�ã�����λ���Ľ��������
#define CMD_ERR         0x02
#define CMD_PROEFFECT   0x03

typedef struct
{
    Uint8   DMAC[6], SMAC[6];
    Uint16  type, packCheckSum;         // 16�ֽ� packCheckSum�����ͷ�ĺ�У�顣

    Uint32  DestIP;
    Uint32  SourceIP;
    Uint16  CardType;
    Uint16  CardVery;
    Uint16  upProStat;
    Uint16  ErrReqType;

    Uint32  command;                    //������    // 16�ֽ�
    Uint32  SNO;                        //���к�
    Uint32  datalength;
    Uint32  dataCRC;                    //�ȼ���data��crc��������packCheckSum

    Uint16  data[256];                  //512�ֽ�
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

extern Uint16   upProStat;              //�������³���ʱ��״̬

extern Uint32   checkCMD(UPDATAFLASHPRO *data32);
extern Uint16   FlashProChksum(Uint16 *buffer, Uint16 size);
extern void sendErrReq(UPDATAFLASHPRO *updata, Uint16 errType, struct netif *device);
extern void UpDataFlashProgram(Uint32 *pData32, struct netif *device);
extern void BlockErase(Uint32 addr);
extern void FlashErase(Uint32 addr);

// ���ⲿ��ɵĺ�������ҪΪ����ֲ����

//extern void upProBegHandle(void );
#define upProBegHandle()    (OnlyNET())
extern void proFlashBegHandle(void);

extern void flashWriteErrHandle(void);

extern Uint32   writeNetDataToProFlash(Uint32 startAddr, Uint32 *data, Uint32 length);

extern void cardDelay(void);
//extern void UNcardDelay(void);

extern void hardwareReset(void);

extern Uint32   GetCardIP(void);

//extern void upProBegHandle(void );          //�ⲿ��Ҫ��ֲ�ĺ���
//extern void flashWriteErrHandle(void);  //�ⲿ��Ҫ��ֲ�ĺ���
//extern void cardDelay(void);
//extern void UNcardDelay(void);
//extern void hardwareReset(void);

extern Uint32   cardIP;
//extern Uint32   GetCardIP(void);

extern void EarseEPAProInf(void);       //����EPA����ƽ̨������Ϣ������
extern void EarseEPAUserProInf(void);   //����EPA�û�������Ϣ������
void LocalHexInfInit(void);
//��λ�����͸��¿�������ʱ�������ֽڡ�
void UpDataDeviceIDToFlash(void);
extern void upProInit(void);
void    dealTestPack(Uint32 *pd32, struct netif *netif);    //���³��򡢲��Ա���

extern void netTestInit(void);
extern void UNBreakPoint(void);
extern void OnlyNET(void);
extern Uint32   IsNetTestPack(Uint8 *p);
extern void DealNetTest(Uint32 *pData32, struct netif *device);
#endif
