#include "global.h"
//#include "epaDebugConfig.h"
#include "at91sam9xe_out.h"
#ifndef _SPI_H_
#define _SPI_H_

/// Calculate the PCS field value given the chip select NPCS value
#define SPI_PCS(npcs)       ((~(1 << npcs) & 0xF) << 16)

extern void SPI_WRITE_CR(unsigned int enable_1,
                           unsigned int software_reset_1,
                           unsigned int last_transfer_1);


extern void  SPI_WRITE_MR(
                           unsigned int peripheral_select_1,
                           unsigned int chip_select_decode_1,
                           //unsigned int clock_selection_1,
                           unsigned int mode_fault_detection_1,
                           unsigned int local_loopback_enable_1,
                           unsigned int peripheral_chip_select_4,
                           unsigned int delay_between_chip_selects_8);

extern void SPI_WRITE_TDR(unsigned int transmint_data_16);

extern void SPI_WRITE_CSR(unsigned int delay_before_spck_8,
                            unsigned int delay_between_consecutive_Transfers_8);


extern void SPI_Write(unsigned short data);
extern void ISR_Spi0(void);
#endif