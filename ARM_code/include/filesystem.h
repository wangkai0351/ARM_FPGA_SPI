/********************************************************************
 *�ļ�˵��: �ļ�ϵͳ���ⲿģ��Ľӿ�
 *�ļ�����: 
 *�ļ��汾:
 *�޸�����: 2007-10-10
 *��Ȩ����: 
 ********************************************************************/
#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H
#include "global.h"

/*--------------�궨����----------------*/

/*wFileStat�����嵱ǰ�ļ�������״̬*/
#define FILE_OPEN_R         1
#define FILE_OPEN_W         2
#define FILE_OPEN_RW        3
#define FILE_OPEN_IDLE      4
#define FILE_OPEN_SYS       5

//#define FLASH_FILE_MAX_LEN  ((1024 - 15) * 4) //ԭ��һ���ļ��Ĵ�СΪ4K
#define FLASH_FILE_MAX_LEN  (4096-sizeof(FCB_INFO_HEAD))
#define Block_Size_Bytes    4096
#define Block_Size_Words    1024

#if 0
typedef struct
{
    Uint16  wSectVaildFlag;
    Uint16  wFileVaildFlag;

    char    strFileName[32];

    Uint16  wZero;
    Uint16  wFileID;

    Uint16  wFileLen;
    Uint16  wFileType;

    Uint32  dwFileVer;
    Uint32  dwDataCRC;
    Uint32  dwFCBHCRC;
    Uint32  dwDefFCBHCRC;
} FCB_INFO_HEAD;

typedef struct
{
    Uint16  wSectVaildFlag;
    Uint16  wFileVaildFlag;

    char    strFileName[32];

    Uint16  wZero;
    Uint16  wFileID;

    Uint16  wFileLen;
    Uint16  wFileType;

    Uint32  dwFileVer;
    Uint32  dwDataCRC;
    Uint32  dwFCBHCRC;
    Uint32  dwDefFCBHCRC;

    Uint32  data[FLASH_FILE_MAX_LEN / 4];   /*ʹ�������ṹ��Ĵ�С�պ�Ϊ1k*/
} FCB_INFO;

typedef struct  fcb_mem_info
{
    struct fcb_mem_info *next;
    char    strFileName[32];
    Uint16  wZero;
    Uint16  wFileID;
    Uint16  wOpenStat;
    Uint16  wFileStat;
    Uint16  wFileLen;
    Uint16  wFileWCursor;
    Uint32  dwFileVer;
    FCB_INFO    *lpMainFile;
    FCB_INFO    *lpBackFile;
    FCB_INFO    *lpBuffFile;
    void (*WCallBack) (Uint32 errType, void *callArg);
    void    *callArg;
} FCB_MEM_INFO;
#endif

//�ļ��������ƿ�
typedef struct
{
    Uint32  wSectVaildFlag;
    Uint32  wFileVaildFlag;

    char    strFileName[32];

    Uint32  wZero;
    Uint32  wFileID;

    Uint32  wFileLen;
    Uint32  wFileType;

    Uint32  dwFileVer;
    Uint32  dwDataCRC;
    Uint32  dwFCBHCRC;
    Uint32  dwDefFCBHCRC;
} FCB_INFO_HEAD;

typedef struct
{
    Uint32  wSectVaildFlag;
    Uint32  wFileVaildFlag;

    char    strFileName[32];

    Uint32  wZero;
    Uint32  wFileID;

    Uint32  wFileLen;
    Uint32  wFileType;

    Uint32  dwFileVer;
    Uint32  dwDataCRC;
    Uint32  dwFCBHCRC;
    Uint32  dwDefFCBHCRC;

    Uint32  data[FLASH_FILE_MAX_LEN / 4];   /*ʹ�������ṹ��Ĵ�С�պ�Ϊ4k*/
} FCB_INFO;
//�ļ����ƿ�
typedef struct  fcb_mem_info
{
    struct fcb_mem_info *next;
    char    strFileName[32];
    Uint32  wZero;
    Uint32  wFileID;
    Uint16  wOpenStat;
    Uint16  wFileStat;
    Uint32  wFileLen;
    Uint32  wFileWCursor;
    Uint32  dwFileVer;
    FCB_INFO    *lpMainFile;
    FCB_INFO    *lpBackFile;
    FCB_INFO    *lpBuffFile;
    void (*WCallBack) (Uint32 errType, void *callArg);
    void    *callArg;
} FCB_MEM_INFO;

typedef struct
{
    Uint32  dwFlashSta;
    FCB_MEM_INFO    *lpFileMemInfoH;
} FCB_BASIC_MEM_INFO;

#define FILE_NEW_DATA   11
#define FILE_WRITE_ERR  12
#define FILE_DATA_DEF   0

/*********************************************************************
* ��������:     InitFileSystem
* ����˵��:     ��ʼ���ļ�ϵͳ
* ��ڲ���:     ��
* ���ز���:     ��
* ��ע��Ϣ:     ��������ļ�ϵͳ����ȷ�ԣ�����ʼ��basic_info,FileFlag��
*********************************************************************/
void    InitFileSystem(void);

/*********************************************************************
* ��������:     OpenFile
* ����˵��:     ����Ӧ�ý��̶��ļ�����ʱ���Ƚ����ļ��򿪲�����
* ��ڲ���:     �ַ����ļ������ļ��򿪷�ʽ
* ���ز���:     �ļ����ƿ�
				NULL:���ļ�ʧ��				
* ��ע��Ϣ:     ���ļ��Ĳ���������,Ӧ�ý���ͨ�����ļ�������ȡ���ļ�
                ���ƿ��,����ִ����Ӧ���ļ���ȡ�����ļ�д�������
*********************************************************************/
FCB_MEM_INFO    *OpenFile(const char *filename, Uint16 openmode);

/*********************************************************************
* ��������:     ReadFile
* ����˵��:     ��Flash�ļ��ϣ���ȡ�ļ�������
* ��ڲ���:     file:	����ȡ�ļ����ļ����ƿ�
				readbuf:ָ���ȡ�ļ����ݴ洢�Ŀռ䣬
				length: ���ļ��ж�ȡ���ݵĳ���
* ���ز���:     ֵ��Ϊ0: ��ȡ���ݵĳ���
				0: 		 ��ȡʧ��
* ��ע��Ϣ:     �ڶ�ȡ�ļ�ʱ�������ȡ�ĳ��ȳ������ļ��ĳ��ȣ���ֻ�ܶ�ȡ
                �ļ�����Ч���ȣ�������ڶ�ȡ�Ĺ����У��������������ݳ���
                ���֪ͨ�ļ�ϵͳ���ļ��������ݽ��и��£�������������ͱ�
                ���������ݶ�����ȷʱ�������¶�ȡʧ�ܡ�
                �ڶ�Flash�ļ��������ݵĶ�ȡʱ�������ȶ�Flash�Ƿ���н���
                �жϣ����������Flash����֮������Flash���в��������
                ����֮��Ž���Flash�Ķ�ȡ������
*********************************************************************/
Uint32  ReadFile(FCB_MEM_INFO *file, Uint8 *readbuf, Uint32 length);


/*********************************************************************
* ��������:     WriteFile
* ����˵��:     ��д������ݣ�����д�뵽��������ļ��������У�֮��֪ͨ
				�ļ����̣���Flash�ļ����и��¡�
* ��ڲ���:     file:	��д�ļ����ļ����ƿ�
				writebuf:ָ����Ҫд�뵽Flash�ļ���������ָ��
				length: д�뵽�ļ������ݳ���
				wCallBack:�ڽ��ļ����µ�Flash֮��ִ�еĻص�������ͨ����
				�����û����Զ��Ƿ�ʵ�ʸ��µ�Flash�ϣ�������ȷ����Ӧ���ڸ�
				�ص���������Ҫ����2���������ֱ�Ϊ�û�Ӧ�ó���Ĳ���arg����
				����������Ĳ���errType
				arg: �û��ص�������Ӧ�ó��򴫵ݵĲ�����
* ���ز���:     0: д�뵽�ļ�����ɹ�
				1: д�뵽�ļ�ʱ��û���㹻�Ļ�����
				2: д�뵽�ļ�������ʱ������������������Ҫ�����ļ���ǰ
				״̬����д
* ��ע��Ϣ:     ��д�ļ�ʱ�����Ƚ����ļ�״̬�Ƿ��д��֮����ڴ����ģ��
				�������㹻���ļ������ڴ棬����ɹ�֮�󣬽��ļ�����д�뵽
				�������У�д��ɹ�֮���ٽ�д���ļ�������Ļص�����ע��
				���ļ����ƿ��У�֮��֪ͨ�ļ�ϵͳ���̶��ļ����и��¡�
*********************************************************************/
Uint32  WriteFile
        (
            FCB_MEM_INFO    *file,
            const Uint8 *writebuf,
            Uint32  length,
            void (*wCallBack) (Uint32 errType, void *callArg),
            void *arg
        );


/*********************************************************************
* ��������:     CloseFile
* ����˵��:     �ر��ļ������ļ���״̬��Ϊ����״̬
* ��ڲ���:     file:	��Ҫ��Ҫ�ر��ļ����ļ����ƿ�
* ���ز���:     ��
* ��ע��Ϣ:     �ڹر��ļ������У�����ļ����ƿ��б����ļ���Ҫ�ļ�ϵͳ
				���̽��и��²��������ļ��Ĳ�����������Ϊϵͳ��״̬
				FILE_OPEN_SYS�����������Ҫ����������Ϊ����״̬
				FILE_OPEN_IDLE�����ļ����Ա��ٴδ�ִ�С�
*********************************************************************/
void    CloseFile(FCB_MEM_INFO *file);

/*********************************************************************
* ��������:     DeleteFile
* ����˵��:     ���ļ�ϵͳ��ɾ���ļ�
* ��ڲ���:     file:	��Ҫ���ļ�ϵͳ��ɾ�����ļ����ļ����ƿ�
* ���ز���:     ��
* ��ע��Ϣ:     ��ִ��ɾ���ļ�֮�󣬳����ļ����������ͱ�����������Ϊ��
         		�ļ������ͷ��ļ������⣬����ļ�������ָ����ļ�������
         		��Ϊ�գ�����Ҫ���ò����ڴ��ͷŵ��ڴ����ģ���С�����
         		�ļ����ļ����ƿ�������ɾ����
*********************************************************************/
void    DeleteFile(FCB_MEM_INFO *file);

/*********************************************************************
* ��������:     UpdateFile
* ����˵��:     �ر��ļ������ļ���״̬��Ϊ����״̬
* ��ڲ���:     file:	��Ҫ���и��²������ļ����½����ƿ�ָ��
* ���ز���:     ��
* ��ע��Ϣ:     UpdateFile��ϵͳ��ʼ���������ڶ�ȡ�ļ��ļ����ݣ���⵽
				�ļ����������Լ���д���ļ�֮�����ļ�ϵͳӦ�ý��̽�
				�е��ã�ִ���ļ��ĸ��²�����
				����ڸ��¹����У���������Flash����ʧ��ʱ����ɾ�����ļ���
				��ִ����Ϻ��ļ�����״̬Ϊ�գ��ļ�״̬Ϊ�ջ���������Ч
				״̬��
*********************************************************************/
void    UpdateFile(FCB_MEM_INFO *file);

void GetFlashSecID(void);
#endif
