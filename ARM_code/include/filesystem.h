/********************************************************************
 *文件说明: 文件系统和外部模块的接口
 *文件作者: 
 *文件版本:
 *修改日期: 2007-10-10
 *版权所有: 
 ********************************************************************/
#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H
#include "global.h"

/*--------------宏定义区----------------*/

/*wFileStat：定义当前文件所处的状态*/
#define FILE_OPEN_R         1
#define FILE_OPEN_W         2
#define FILE_OPEN_RW        3
#define FILE_OPEN_IDLE      4
#define FILE_OPEN_SYS       5

//#define FLASH_FILE_MAX_LEN  ((1024 - 15) * 4) //原来一个文件的大小为4K
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

    Uint32  data[FLASH_FILE_MAX_LEN / 4];   /*使得整个结构体的大小刚好为1k*/
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

//文件扇区控制块
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

    Uint32  data[FLASH_FILE_MAX_LEN / 4];   /*使得整个结构体的大小刚好为4k*/
} FCB_INFO;
//文件控制块
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
* 函数名称:     InitFileSystem
* 函数说明:     初始化文件系统
* 入口参数:     无
* 返回参数:     无
* 备注信息:     检查所有文件系统的正确性，并初始化basic_info,FileFlag。
*********************************************************************/
void    InitFileSystem(void);

/*********************************************************************
* 函数名称:     OpenFile
* 函数说明:     程序应用进程对文件操作时，先进行文件打开操作。
* 入口参数:     字符串文件名，文件打开方式
* 返回参数:     文件控制块
				NULL:打开文件失败				
* 备注信息:     在文件的操作过程中,应用进程通过打开文件操作获取到文件
                控制块后,才能执行相应的文件读取或者文件写入操作。
*********************************************************************/
FCB_MEM_INFO    *OpenFile(const char *filename, Uint16 openmode);

/*********************************************************************
* 函数名称:     ReadFile
* 函数说明:     从Flash文件上，读取文件的数据
* 入口参数:     file:	被读取文件的文件控制块
				readbuf:指向读取文件数据存储的空间，
				length: 从文件中读取数据的长度
* 返回参数:     值不为0: 读取数据的长度
				0: 		 读取失败
* 备注信息:     在读取文件时，如果读取的长度超过了文件的长度，则只能读取
                文件的有效长度，而如果在读取的过程中，发现有扇区数据出错，
                则会通知文件系统对文件扇区数据进行更新，而如果主扇区和备
                份区的数据都不正确时，将导致读取失败。
                在对Flash文件进行数据的读取时，将首先对Flash是否空闲进行
                判断，如果发生在Flash擦除之后，则会对Flash进行擦除挂起操
                作。之后才进行Flash的读取操作。
*********************************************************************/
Uint32  ReadFile(FCB_MEM_INFO *file, Uint8 *readbuf, Uint32 length);


/*********************************************************************
* 函数名称:     WriteFile
* 函数说明:     将写入的数据，首先写入到新申请的文件缓冲区中，之后通知
				文件进程，对Flash文件进行更新。
* 入口参数:     file:	被写文件的文件控制块
				writebuf:指向需要写入到Flash文件的数据区指针
				length: 写入到文件的数据长度
				wCallBack:在将文件更新到Flash之后，执行的回调函数，通过该
				功能用户可以对是否实际更新到Flash上，进行正确的响应，在该
				回调函数中需要传递2个参数，分别为用户应用程序的参数arg和文
				件更新情况的参数errType
				arg: 用户回调函数，应用程序传递的参数。
* 返回参数:     0: 写入到文件缓冲成功
				1: 写入到文件时，没有足够的缓冲区
				2: 写入到文件缓冲区时，发生了其他错误，主要包括文件当前
				状态不可写
* 备注信息:     在写文件时，首先进行文件状态是否可写，之后从内存管理模块
				中申请足够的文件缓冲内存，申请成功之后，将文件数据写入到
				缓冲区中，写入成功之后，再将写入文件扇区后的回调函数注册
				到文件控制块中，之后通知文件系统进程对文件进行更新。
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
* 函数名称:     CloseFile
* 函数说明:     关闭文件，将文件打开状态置为空闲状态
* 入口参数:     file:	需要需要关闭文件的文件控制块
* 返回参数:     无
* 备注信息:     在关闭文件操作中，如果文件控制块中表明文件需要文件系统
				进程进行更新操作，则将文件的操作属性设置为系统打开状态
				FILE_OPEN_SYS，而如果不需要更新则设置为空闲状态
				FILE_OPEN_IDLE，即文件可以被再次打开执行。
*********************************************************************/
void    CloseFile(FCB_MEM_INFO *file);

/*********************************************************************
* 函数名称:     DeleteFile
* 函数说明:     从文件系统中删除文件
* 入口参数:     file:	需要从文件系统中删除的文件的文件控制块
* 返回参数:     无
* 备注信息:     在执行删除文件之后，除将文件的主扇区和备份扇区设置为空
         		文件，即释放文件扇区外，如果文件控制所指向的文件缓冲区
         		不为空，则还需要将该部分内存释放到内存管理模块中。并将
         		文件从文件控制块链表中删除。
*********************************************************************/
void    DeleteFile(FCB_MEM_INFO *file);

/*********************************************************************
* 函数名称:     UpdateFile
* 函数说明:     关闭文件，将文件打开状态置为空闲状态
* 入口参数:     file:	需要进行更新操作的文件的温江控制块指针
* 返回参数:     无
* 备注信息:     UpdateFile由系统初始化，或者在读取文件文件数据，检测到
				文件数据有误，以及在写入文件之后，由文件系统应用进程进
				行调用，执行文件的更新操作。
				如果在更新过程中，发生申请Flash扇区失败时，将删除该文件。
				在执行完毕后，文件操作状态为空，文件状态为空或者数据有效
				状态。
*********************************************************************/
void    UpdateFile(FCB_MEM_INFO *file);

void GetFlashSecID(void);
#endif
