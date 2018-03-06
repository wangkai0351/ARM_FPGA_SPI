#ifndef _IRQ_H
#define _IRQ_H


void    DisableInt(unsigned char n);
void    EnableInt(unsigned char n);
void    SetInt(unsigned char n);
void    ClrInt(unsigned char n);
void    Int_Install(void);
unsigned int    ReadIntPnd(void);
unsigned int    MaskGlobalInt(void);
void    RestoreGlobalInt(unsigned int i);

extern void TC0_Handler(void);
extern void TC1_Handler(void);
extern void TC2_Handler(void);
extern void TC3_Handler(void);
extern void TC4_Handler(void);
extern void EMAC_Handler(void);
extern void PIOB_Handler(void);
extern void RT_Handler(void);
extern void EX2_Handler(void);

extern volatile int OSTickSemCount;
extern volatile int CSMETickSemCount;
extern volatile int CSMEPktSemCount;
//extern volatile int  PktSendSemCount;
extern volatile unsigned int gEPASysTimeTick;


#define TIME_OK     1
#define TIME_FALSE  2
extern volatile unsigned int   g_TimeOK;



#endif
 
 
