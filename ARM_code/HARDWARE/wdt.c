/*
*********************************************************************************************************
*         DW Project       
*
*Hardware platform : CM-TAS28 , ATSAM9XE512
*
* File             : wdt.c
* version          : v1.0
* By               : lp
* Modify         : lp
* Date             : 
*********************************************************************************************************
*/
//Modify by lp


//#include "sysdata.h"
//#include "cardConfig.h"
#include "global.h"
#include "includes.h"
#include "main.h"
//#include "led.h"
#include "pio.h"
#include "wdt.h"
#define WDT_DATA_ON 0xa5

// д��������
#define WREN_INST   0x06

// д��ֹ����
#define WRDI_INST   0x04

// д״̬�Ĵ���ָ��
#define WRSR_INST   0x01

// ��״̬�Ĵ���ָ��
#define RDSR_INST   0x05

// д���Ź�Flash�洢��Ԫָ��
#define WRITE_INST  0x02

// �����Ź�Flash�洢��Ԫָ��
#define READ_INST   0x03

// ״̬�Ĵ���
#define STATUS_REG  0x10

// ���ȴ�ʱ�䵥Ԫ����
#define MAX_POLL    0x99

#define  MsToOSTicks(ms)   (OS_TICKS_PER_SEC * ((INT32U)ms + 500L / OS_TICKS_PER_SEC) / 1000L)

Uint8   gWdtBuf[512];   /*���Ź��ɱ��flash���ڴ��е�ӳ��*/

typedef struct
{
    //Uint16  offSetAddr;
    Uint8   bufFlag;
    Uint8   bufLen;
    Uint16  bufCRC16;
} S_WDT_DATA_HEARD;

void    outbyte(Uint8 WDTsenddata);
Uint8   inputbyte(void);
Uint8   rdsr_cmd(void);
void    wrsr_cmd(Uint8 watchtime);
void    wren_cmd(void);
void    wip_poll(void);
void    WDT_SetTime(Uint8 rstTime);

void gDelay(Uint32 i)
{
	Uint32  j;
    for(j = 0; j < i;)
    {
//QQQ   180M�������Ҫ�޸�
        j++;
    }
}
void outbyte(Uint8 WDTsenddata)
{
    Uint32  i;
    for(i = 0; i < 8; i++)
    {
        IOClr(WDTSCK);
        gDelay(10);
        if(WDTsenddata >= 0x80) //���λΪ1
        {
            IOSet(WDTMOSI);
        }
        else
        {
            IOClr(WDTMOSI);
        }

        gDelay(10);
        IOSet(WDTSCK);
        gDelay(10);
        WDTsenddata = WDTsenddata << 1;
    }

    //WDTMOSI = 0;
    IOClr(WDTMOSI);
}

Uint8 inputbyte(void)
{
    Uint8   WDTdataTemp;
    Uint8   my_flag;
    Uint8   i;
    WDTdataTemp = 0x00;
    for(i = 8; i > 0; i--)
    {
        IOClr(WDTSCK);
        gDelay(10);
        my_flag = (Uint8) (IOBITStat(WDTMISO));
        IOSet(WDTSCK);
        gDelay(10);
        WDTdataTemp = WDTdataTemp + (my_flag << (i - 1));
        my_flag = 0x00;
    }

    return WDTdataTemp;
}

Uint8 rdsr_cmd(void)
{
    Uint8   WDTrdsr;
    IOClr(WDTSCK);
    gDelay(20);
    IOClr(WDTCS);
    gDelay(20);
    outbyte(RDSR_INST);
    WDTrdsr = inputbyte();
    IOClr(WDTSCK);
    gDelay(20);
    IOSet(WDTCS);
    gDelay(20);
    return WDTrdsr;
}

void wrsr_cmd(Uint8 watchtime)
{
    IOClr(WDTSCK);
    gDelay(10);
    IOClr(WDTCS);
    gDelay(100);
    outbyte(WRSR_INST);         //Instraction : write Reg

    outbyte(watchtime);         //NOProtectandonesecond;//STATUS_REG;

    IOClr(WDTSCK);
    gDelay(10);
    IOSet(WDTCS);
    gDelay(10);
    wip_poll();
}

void wren_cmd(void)
{
    IOClr(WDTSCK);
    gDelay(10);
    IOClr(WDTCS);
    gDelay(10);
    outbyte(WREN_INST);
    IOClr(WDTSCK);
    gDelay(10);
    IOSet(WDTCS);
}

void wip_poll(void)
{
    Uint32  WDTwip;
    Uint8   my_flag;

    for(WDTwip = 0; WDTwip < MAX_POLL; WDTwip++)
    {
        my_flag = rdsr_cmd();
        if((my_flag & 0x01) == 0)
        {
            break;
        }
    }
}

/****************************************/

/* 25045 BYTE write,aa is the data ,    */

/* dd is the addr:from 000 to 1FF	*/

/****************************************/
void Fct_ByteWrite(Uint8 wData, Uint16 wAddr)
{
     OS_CPU_SR   cpu_sr;
	
    OS_ENTER_CRITICAL();
    wren_cmd();
    IOClr(WDTSCK);
    gDelay(100);
    IOClr(WDTCS);
    gDelay(100);
    if(wAddr >= 256)
    {
        outbyte(WRITE_INST + 0x08);
    }
    else
    {
        outbyte(WRITE_INST);
    }

    outbyte((Uint8) (wAddr));
    outbyte(wData);
    IOClr(WDTSCK);
    IOSet(WDTCS);
    gDelay(100);
    wip_poll();
    OS_EXIT_CRITICAL();
    return;
}

/****************************************/

/* 25045 BYTE read,dd is the addr       */

/****************************************/
Uint8 Fct_ByteRead(Uint16 rAddr)
{
    Uint8   cc;
    OS_CPU_SR   cpu_sr;
	
    OS_ENTER_CRITICAL();
	
    IOClr(WDTSCK);
    gDelay(10);
    IOClr(WDTCS);
    gDelay(10);
    if(rAddr >= 256)
    {
        outbyte(READ_INST + 0x08);
    }
    else
    {
        outbyte(READ_INST);
    }
    //gDelay(10);
    outbyte((Uint8) (rAddr));
    cc = inputbyte();
    IOClr(WDTSCK);
    gDelay(10);
    IOSet(WDTCS);
    gDelay(10);
	
    OS_EXIT_CRITICAL();
	
    return cc;
}

#if 0
void write_x5045(Uint8 writer_data)
{
	Uint8 i;
	for(i=0;i<8;i++){
		if(writer_data >= 0x80) //���λΪ1
	        {
	            IOSet(WDTMOSI);
	        }
	        else
	        {
	            IOClr(WDTMOSI);
	        }
		IOClr(WDTSCK);
		IOSet(WDTSCK);
		writer_data<<=1;
	}
	IOClr(WDTMOSI);
}

Uint8 read_x5045(void)
{
	Uint8 i,read_data;
	for(i=0;i<8;i++)
		{
			IOSet(WDTSCK);
			IOClr(WDTSCK);
			read_data<<=1;
			read_data|=(Uint8)(IOBITStat(WDTMISO));
		}
	return read_data;
}
Uint8 Fct_ByteRead(Uint8 addr)
{
	Uint8 read_data;
	IOClr(WDTSCK);
	IOClr(WDTCS);
	write_x5045(0x03);
	write_x5045(addr);
	read_data=read_x5045();
	IOClr(WDTSCK);
	IOSet(WDTCS);
	return read_data;
	
	
}
#endif
/****************************************/

/*" 25045 PAGE write		        */

/****************************************/
void page_write(Uint8 *WDTpdata, Uint16 WDTpaddr)
{
    wren_cmd();
    IOClr(WDTSCK);
    gDelay(10);
    IOClr(WDTCS);
    gDelay(10);
    if(WDTpaddr >= 256)
    {
        outbyte(WRITE_INST + 0x08);
    }
    else
    {
        outbyte(WRITE_INST);
    }

    outbyte((Uint8) (WDTpaddr));

    outbyte(*WDTpdata);
    outbyte(*(WDTpdata + 1));
    outbyte(*(WDTpdata + 2));
    outbyte(*(WDTpdata + 3));
    IOClr(WDTSCK);
    gDelay(10);
    IOSet(WDTCS);
    gDelay(10);
    wip_poll();
}

/*��������ȼ�����������*/
void WDT_FeedDog(void)
{
    gDelay(10);
    IOClr(WDTCS);
    gDelay(10);
    IOSet(WDTCS);
    gDelay(10);
}

void WDT_SetTime(Uint8 rstTime)
{
    switch(rstTime)
    {
    case WDT_1P4SWDT:
    case WDT_600MSWDT:
    case WDT_200MSWDT:
        //rstTime = WDT_1P4SWDT;
        break;
    case WDT_DISABLEWDT:
        break;

    default:
        rstTime = WDT_DISABLEWDT;
        break;
    }

    IOClr(WDTSCK);
    gDelay(100);
    wren_cmd();         //write enable
    gDelay(100);
    wrsr_cmd(rstTime);  //watch time programming
    IOSet(WDTCS);
    gDelay(10);
    wip_poll();
}

Uint16 wdt_checksum(Uint8 *pBuf, Uint16 length)
{
    Uint16  result;
    result = (Uint16) crc32_check(pBuf, length, 0x55aa);
    return result;
}

SOFT_WATCH_DOG_TIMER SoftWatchDogTimerList[MAX_SWDT_ID] = {0}; 
Uint8 StopWDTFedMake=0;

void Soft_HardwareWDTInit(void){ 

memset(SoftWatchDogTimerList, 0, sizeof(SOFT_WATCH_DOG_TIMER)*MAX_SWDT_ID); 
StopWDTFedMake = 0; 
//��ʼ��Ӳ�����Ź� 
//WDT_SetTime(WDT_600MSWDT); 
} 


//��ʼ��������Ź� 
Uint8 SoftWdtInit(SWDT_ID SwdtId, Uint32 TimerTop)
{ 
	SOFT_WATCH_DOG_TIMER *SoftWatchDogTimerPtr = SoftWatchDogTimerList; 
	Uint16 osTick = 0; 

	if(SwdtId >= MAX_SWDT_ID){ 
	return 0; 
	} 
	SoftWatchDogTimerPtr += SwdtId; 
	if(SoftWatchDogTimerPtr->watchDogState == SWDT_STAT_IDLE){ 
	    
	   osTick = MsToOSTicks(TimerTop);//��mSʱ�任���ϵͳʱ�ӽ��� 
	   SoftWatchDogTimerPtr->watchDogTimeOut = osTick; 
	   SoftWatchDogTimerPtr->watchDogTime = osTick; 
	   SoftWatchDogTimerPtr->watchDogState = SWDT_STAT_RUN; 

	   return 1; 
	}
	else{ 
	  return 0; 
	} 
} 

void SoftWdtFed(SWDT_ID SwdtId)
{ 
	 CPU_SR  cpu_sr;
	SOFT_WATCH_DOG_TIMER *SoftWatchDogTimerPtr = SoftWatchDogTimerList; 
	if(SwdtId >= MAX_SWDT_ID){ 
		return; 
	} 
	SoftWatchDogTimerPtr += SwdtId;  
	
	OS_ENTER_CRITICAL(); 
	SoftWatchDogTimerPtr->watchDogCount++; 
	//��֤������ֵʹ�ղ���� 
	if(SoftWatchDogTimerPtr->watchDogCount == SoftWatchDogTimerPtr->watchDogCountBack){ 
		SoftWatchDogTimerPtr->watchDogCount++; 
	} 
	OS_EXIT_CRITICAL(); 
} 

void SuspenWdt(SWDT_ID SwdtId)
{
	CPU_SR  cpu_sr;
	SOFT_WATCH_DOG_TIMER *SoftWatchDogTimerPtr = SoftWatchDogTimerList; 
	if(SwdtId >= MAX_SWDT_ID){ 
		return; 
	} 
	SoftWatchDogTimerPtr += SwdtId;  
	OS_ENTER_CRITICAL(); 
	SoftWatchDogTimerPtr->watchDogState=SWDT_STAT_SUSPEN;
	OS_EXIT_CRITICAL(); 

}

void RunWdt(SWDT_ID SwdtId)
{
	CPU_SR  cpu_sr;
	SOFT_WATCH_DOG_TIMER *SoftWatchDogTimerPtr = SoftWatchDogTimerList; 
	if(SwdtId >= MAX_SWDT_ID){ 
		return; 
	} 
	SoftWatchDogTimerPtr += SwdtId;  
	OS_ENTER_CRITICAL(); 
	SoftWatchDogTimerPtr->watchDogState=SWDT_STAT_RUN;
	OS_EXIT_CRITICAL(); 
}
void SoftWdtISR(void)
{ 

	SOFT_WATCH_DOG_TIMER *SoftWatchDogTimerPtr = SoftWatchDogTimerList; 
	Uint8 i = 0; 

	if(StopWDTFedMake == TRUE){ 
		return; 
	} 
	for(i=0; i<MAX_SWDT_ID; i++){ 
		//�Թ���Ϳ��еĿ��Ź���ʱ�������м��� 
		if(SoftWatchDogTimerPtr->watchDogState == SWDT_STAT_RUN){ 
			//������Ź���ιʳ����װ���Ź��������� 
			if(SoftWatchDogTimerPtr->watchDogCount != SoftWatchDogTimerPtr->watchDogCountBack){ 

			   SoftWatchDogTimerPtr->watchDogCountBack = SoftWatchDogTimerPtr->watchDogCount; 
			   SoftWatchDogTimerPtr->watchDogTime = SoftWatchDogTimerPtr->watchDogTimeOut; 
			}
			else if(--SoftWatchDogTimerPtr->watchDogTime == 0){ //û��ι�������Ź���ʱ����һ���� 
			   //������һ�������ŵĿ��Ź���ʱֹͣιӲ�����Ź� 
			   StopWDTFedMake = TRUE; 
			   return; 
			} 
		  } 
		SoftWatchDogTimerPtr++; 
	} 
	//����Ӳ��ι�� 
	WDT_FeedDog(); 
} 
/*
void OSTimeTickHook (void) 
{ 
	SoftWdtISR();  //��ʱ��ѯÿһ��������Ź�ι��״���� 
} 
*/

