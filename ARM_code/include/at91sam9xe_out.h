#ifndef AT91SAM9XE_H
#define AT91SAM9XE_H
#include "at91sam9xe512.h"
#include "aic/aic.h"
#include "tc.h"
//#include "pio.h"

__no_init AT91S_RSTC RSTC @ 0xFFFFFD00;
__no_init AT91S_PITC PIT @ 0xFFFFFD30;
__no_init AT91S_WDTC WDT @ 0xFFFFFD40;
//__no_init AT91S_VREG VERG @ 0xFFFFFD60;
//__no_init AT91S_MC MC @ 0xFFFFFF00;
__no_init AT91S_AIC AIC @ 0xFFFFF000;
__no_init AT91S_PMC PMC @ 0xFFFFFC00;
__no_init AT91S_PIO PIO @ 0xFFFFF400;
//__no_init StructPIO PIOB @ 0xFFFFF600;
//__no_init StructSPI SPI0 @ 0xFFFE0000;
//__no_init StructSPI SPI1 @ 0xFFFE4000;
__no_init StructTCBlock TCBlock @ 0xFFFA0000;
__no_init StructTCBlock TCBlock1 @ 0xFFFDC000;
//__no_init StructEMAC EMAC @ 0xFFFDC000;


#define MYExtern1 extern 


MYExtern1 AT91S_RSTC RSTC;
MYExtern1 AT91S_PITC PIT;
MYExtern1 AT91S_WDTC WDT;
//MYExtern1 StructVREG VERG;
//MYExtern1 StructMC MC;
//MYExtern1 StructPDC PDC;
MYExtern1 AT91S_AIC AIC;
MYExtern1 AT91S_PMC PMC;
MYExtern1 AT91S_PIO PIO;
//MYExtern1 StructSPI SPI0,SPI1;
MYExtern1 StructTCBlock TCBlock;
MYExtern1 StructTCBlock TCBlock1;
//MYExtern1 StructEMAC EMAC;
#endif
