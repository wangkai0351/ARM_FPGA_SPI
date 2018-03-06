#include "AT91SAM9XE512.h"

//write register function definetion
void SPI_WRITE_CR(unsigned int enable_1,
                           unsigned int software_reset_1,
                           unsigned int last_transfer_1)
{
  AT91C_BASE_SPI0->SPI_CR = (AT91C_SPI_SPIEN & (enable_1))
                          | (AT91C_SPI_SWRST & (software_reset_1<< 7))
                          | (AT91C_SPI_LASTXFER & (last_transfer_1<< 24));
}

void SPI_WRITE_MR(
                           unsigned int peripheral_select_1,
                           unsigned int chip_select_decode_1,
                           unsigned int mode_fault_detection_1,
                           unsigned int local_loopback_enable_1,
                           unsigned int peripheral_chip_select_4,
                           unsigned int delay_between_chip_selects_8)
{
  AT91C_BASE_SPI0->SPI_MR = (AT91C_SPI_MSTR & 1)
                          | (AT91C_SPI_PS & (peripheral_select_1 >> 1))
                          | (AT91C_SPI_PCSDEC & (chip_select_decode_1 >> 2))
                          | (AT91C_SPI_FDIV & (0x01 >> 3))
                          | (AT91C_SPI_MODFDIS & (mode_fault_detection_1 >> 4))
                          | (AT91C_SPI_LLB & (local_loopback_enable_1 >> 7))
                          | (AT91C_SPI_PCS &((unsigned int)peripheral_chip_select_4 >> 16))
                          | (AT91C_SPI_DLYBCS &((unsigned int)delay_between_chip_selects_8 >> 24));
}

void SPI_WRITE_TDR(unsigned int transmint_data_16)
{
  AT91C_BASE_SPI0->SPI_TDR = (AT91C_SPI_TD &(transmint_data_16 >> 0));                             
}

void SPI_WRITE_CSR(unsigned int delay_before_spck_8,
                            unsigned int delay_between_consecutive_Transfers_8)
{
    AT91C_BASE_SPI0->SPI_CSR[0] =(AT91C_SPI_CPOL & (1)) //?
                              |(AT91C_SPI_NCPHA & (0x00 << 1))  //空闲搞电平，上升沿采样
                              | (AT91C_SPI_CSAAT & (0x00 << 3))
                              | (AT91C_SPI_BITS & (0x00<< 4))  //一次传输几位
                              | (AT91C_SPI_SCBR & (0x2d << 8)) //0X04=配置成低于25MHz
                              | (AT91C_SPI_DLYBCT & (delay_between_consecutive_Transfers_8 << 24))
                              | (AT91C_SPI_DLYBS & (delay_before_spck_8 << 16)) ;

}


void SPI_Write(unsigned short data)
{
    // Discard contents of RDR register
    //volatile unsigned int discard = spi->SPI_RDR;
    unsigned int discard = AT91C_BASE_SPI0->SPI_RDR; 
    // Send data
    while ((AT91C_BASE_SPI0->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
    AT91C_BASE_SPI0->SPI_TDR = data |((0x1) << 16);
    discard = AT91C_BASE_SPI0->SPI_RDR;
    while ((AT91C_BASE_SPI0->SPI_SR & AT91C_SPI_TDRE) == 0);
}
