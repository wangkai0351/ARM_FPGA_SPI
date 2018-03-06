/*
*********************************************************************************************************
*         dw Project       
*
* (c) Copyright 2008, Atmel Corporation
*     All Rights Reserved
*
*Hardware platform : CM-TAS28 , ATSAM9XE512
*
* File             : emac.c
* version          : v1.0
* By               : Atmel Corporation
* Modify         : lp
* Date             : 
*********************************************************************************************************
*/

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <board.h>
#include <pio/pio_sam9.h>

//------------------------------------------------------------------------------
//         Local macros
//------------------------------------------------------------------------------
#define AT91C_MATRIX_CS2    (0x1 <<  2)

/// Reads a register value. Useful to add trace information to read  accesses.
#define READ(peripheral, register)          (peripheral->register)
/// Writes data in a register. Useful to add trace information to write accesses.
#define WRITE(peripheral, register, value)  (peripheral->register = value)

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Changes the mapping of the chip so that the remap area mirrors the
/// internal ROM or the EBI CS0 (depending on the BMS input).
//------------------------------------------------------------------------------
void BOARD_RemapRom(void)
{
    WRITE(AT91C_BASE_MATRIX, MATRIX_MRCR, 0);
}

//------------------------------------------------------------------------------
/// Changes the mapping of the chip so that the remap area mirrors the
/// internal RAM.
//------------------------------------------------------------------------------
void BOARD_RemapRam(void)
{
    WRITE(AT91C_BASE_MATRIX,
          MATRIX_MRCR,
          (AT91C_MATRIX_RCA926I | AT91C_MATRIX_RCA926D));
}

void SmcInit(unsigned char busWidth)
{
	//AT91C_BASE_SMC->SMC_SETUP2 = 0x01020102;    //����datasheetʱ������
	AT91C_BASE_SMC->SMC_PULSE2 =0x1f1f1f1f; // 0x7f7f7f7f;
	AT91C_BASE_SMC->SMC_CYCLE2 = 0x00200020;//0x01ff01ff;
	AT91C_BASE_SMC->SMC_CTRL2  = 0x211f1133;//0x00110033;

	if (busWidth == 8) 
	{
	    AT91C_BASE_SMC->SMC_CTRL2 |= AT91C_SMC_DBW_WIDTH_EIGTH_BITS;
	}
	else if (busWidth == 16) 
	{
	    AT91C_BASE_SMC->SMC_CTRL2 |= AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS;
	}
}
//------------------------------------------------------------------------------
/// Initialize and configure the external SDRAM.
//------------------------------------------------------------------------------
void BOARD_ConfigureSdram(unsigned char busWidth)
{
    volatile unsigned int i;
    static const Pin pinsSdram = PINS_SDRAM;
    volatile unsigned int *pSdram = (unsigned int *) AT91C_EBI_SDRAM;
    unsigned short sdrc_dbw = 0;

    switch (busWidth) {
        case 16:
            sdrc_dbw = AT91C_SDRAMC_DBW_16_BITS;
            break;

        case 32:
        default:
            sdrc_dbw = AT91C_SDRAMC_DBW_32_BITS;
            break;

    }

    // Enable corresponding PIOs
    PIO_Configure(&pinsSdram, 1);
    
    // Enable EBI chip select for the SDRAM
    WRITE(AT91C_BASE_MATRIX, MATRIX_EBI, AT91C_MATRIX_CS1A_SDRAMC);
    

    // CFG Control Register
    WRITE(AT91C_BASE_SDRAMC, SDRAMC_CR, AT91C_SDRAMC_NC_8
                                        | AT91C_SDRAMC_NR_12 
                                        | AT91C_SDRAMC_CAS_2 
                                        | AT91C_SDRAMC_NB_4_BANKS
                                        | sdrc_dbw
                                        | AT91C_SDRAMC_TWR_2
                                        | AT91C_SDRAMC_TRC_7
                                        | AT91C_SDRAMC_TRP_2
                                        | AT91C_SDRAMC_TRCD_2
                                        | AT91C_SDRAMC_TRAS_5
                                        | AT91C_SDRAMC_TXSR_8);

    for (i = 0; i < 1000; i++);

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_NOP_CMD);    // Perform NOP
    pSdram[0] = 0x00000000;

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_PRCGALL_CMD);    // Set PRCHG AL
    pSdram[0] = 0x00000000;                        // Perform PRCHG

    for (i = 0; i < 10000; i++);

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 1st CBR
    pSdram[1] = 0x00000001;                        // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 2 CBR
    pSdram[2] = 0x00000002;                        // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 3 CBR
    pSdram[3] = 0x00000003;                       // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 4 CBR
    pSdram[4] = 0x00000004;                      // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 5 CBR
    pSdram[5] = 0x00000005;                      // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 6 CBR
    pSdram[6] = 0x00000006;                    // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 7 CBR
    pSdram[7] = 0x00000007;                    // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 8 CBR
    pSdram[8] = 0x00000008;                    // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_LMR_CMD);        // Set LMR operation
    pSdram[9] = 0xcafedede;                    // Perform LMR burst=1, lat=2

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_TR, (BOARD_MCK * 15) / 1000000);        // Set Refresh Timer ׼ȷֵӦ��Ϊ15.625

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_NORMAL_CMD);    // Set Normal mode
    pSdram[0] = 0x00000000;                        // Perform Normal mode
}

//------------------------------------------------------------------------------
/// Initialize and configure the SDRAM for a 48 MHz MCK (ROM code clock settings).
//------------------------------------------------------------------------------
void BOARD_ConfigureSdram48MHz(unsigned char busWidth)
{
    volatile unsigned int i;
    static const Pin pinsSdram = PINS_SDRAM;
    volatile unsigned int *pSdram = (unsigned int *) AT91C_EBI_SDRAM;
    unsigned short sdrc_dbw = 0;

    switch (busWidth) {
        case 16:
            sdrc_dbw = AT91C_SDRAMC_DBW_16_BITS;
            break;

        case 32:
        default:
            sdrc_dbw = AT91C_SDRAMC_DBW_32_BITS;
            break;

    }

    // Enable corresponding PIOs
    PIO_Configure(&pinsSdram, 1);
    
    // Enable EBI chip select for the SDRAM
    WRITE(AT91C_BASE_MATRIX, MATRIX_EBI, AT91C_MATRIX_CS1A_SDRAMC);
    

    // CFG Control Register
    WRITE(AT91C_BASE_SDRAMC, SDRAMC_CR, AT91C_SDRAMC_NC_9
                                        | AT91C_SDRAMC_NR_13 
                                        | AT91C_SDRAMC_CAS_2 
                                        | AT91C_SDRAMC_NB_4_BANKS
                                        | sdrc_dbw
                                        | AT91C_SDRAMC_TWR_1
                                        | AT91C_SDRAMC_TRC_4
                                        | AT91C_SDRAMC_TRP_1
                                        | AT91C_SDRAMC_TRCD_1
                                        | AT91C_SDRAMC_TRAS_2
                                        | AT91C_SDRAMC_TXSR_3);

    for (i = 0; i < 1000; i++);

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_NOP_CMD); // Perform NOP
    pSdram[0] = 0x00000000;

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_PRCGALL_CMD); // Set PRCHG AL
    pSdram[0] = 0x00000000;                     // Perform PRCHG

    for (i = 0; i < 10000; i++);

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 1st CBR
    pSdram[1] = 0x00000001;                     // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 2 CBR
    pSdram[2] = 0x00000002;                     // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 3 CBR
    pSdram[3] = 0x00000003;                    // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 4 CBR
    pSdram[4] = 0x00000004;                   // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 5 CBR
    pSdram[5] = 0x00000005;                   // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 6 CBR
    pSdram[6] = 0x00000006;                 // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 7 CBR
    pSdram[7] = 0x00000007;                 // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 8 CBR
    pSdram[8] = 0x00000008;                 // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_LMR_CMD);     // Set LMR operation
    pSdram[9] = 0xcafedede;                 // Perform LMR burst=1, lat=2

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_TR, (48000000 * 7) / 1000000);      // Set Refresh Timer

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_NORMAL_CMD);  // Set Normal mode
    pSdram[0] = 0x00000000;                     // Perform Normal mode
}

//------------------------------------------------------------------------------
/// Configures the EBI for NandFlash access. Pins must be configured after or
/// before calling this function.
//------------------------------------------------------------------------------
void BOARD_ConfigureNandFlash(unsigned char busWidth)
{
    // Configure EBI
    AT91C_BASE_MATRIX->MATRIX_EBI |= AT91C_MATRIX_CS3A_SM;

    // Configure SMC
    AT91C_BASE_SMC->SMC_SETUP3 = 0x00010001;
    AT91C_BASE_SMC->SMC_PULSE3 = 0x03030303;
    AT91C_BASE_SMC->SMC_CYCLE3 = 0x00050005;
    AT91C_BASE_SMC->SMC_CTRL3  = 0x00020003;

    if (busWidth == 8) {

        AT91C_BASE_SMC->SMC_CTRL3 |= AT91C_SMC_DBW_WIDTH_EIGTH_BITS;
    }
    else if (busWidth == 16) {
 
        AT91C_BASE_SMC->SMC_CTRL3 |= AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS;
    }
}

//------------------------------------------------------------------------------
/// Configures the EBI for NandFlash access at 48MHz. Pins must be configured
/// after or before calling this function.
//------------------------------------------------------------------------------
void BOARD_ConfigureNandFlash48MHz(unsigned char busWidth)
{
    // Configure EBI
    AT91C_BASE_CCFG->CCFG_EBICSA |= AT91C_EBI_CS3A_SM;

    // Configure SMC
    AT91C_BASE_SMC->SMC_SETUP3 = 0x00010001;
    AT91C_BASE_SMC->SMC_PULSE3 = 0x04030302;
    AT91C_BASE_SMC->SMC_CYCLE3 = 0x00070004;
    AT91C_BASE_SMC->SMC_CTRL3  = (AT91C_SMC_READMODE
                                 | AT91C_SMC_WRITEMODE
                                 | AT91C_SMC_NWAITM_NWAIT_DISABLE
                                 | ((0x1 << 16) & AT91C_SMC_TDF));
    
    if (busWidth == 8) {

        AT91C_BASE_SMC->SMC_CTRL3 |= AT91C_SMC_DBW_WIDTH_EIGTH_BITS;
    }
    else if (busWidth == 16) {
 
        AT91C_BASE_SMC->SMC_CTRL3 |= AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS;
    }
}

//------------------------------------------------------------------------------
/// Configures the EBI for NorFlash access
/// \Param busWidth Bus width 
//------------------------------------------------------------------------------
void BOARD_ConfigureNorFlash(unsigned char busWidth)
{
    // Configure SMC

    AT91C_BASE_SMC->SMC_SETUP0 = 0x00000002;
    AT91C_BASE_SMC->SMC_PULSE0 = 0x0A0A0A06;
    AT91C_BASE_SMC->SMC_CYCLE0 = 0x000A000A;
    AT91C_BASE_SMC->SMC_CTRL0  = (AT91C_SMC_READMODE
                                  | AT91C_SMC_WRITEMODE
                                  | AT91C_SMC_NWAITM_NWAIT_DISABLE
                                  | ((0x1 << 16) & AT91C_SMC_TDF));

    if (busWidth == 8) {
        AT91C_BASE_SMC->SMC_CTRL0 |= AT91C_SMC_DBW_WIDTH_EIGTH_BITS;
    }
    else if (busWidth == 16) {
        AT91C_BASE_SMC->SMC_CTRL0 |= AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS;
    }
    else if (busWidth == 32) {
        AT91C_BASE_SMC->SMC_CTRL0 |= AT91C_SMC_DBW_WIDTH_THIRTY_TWO_BITS;
    }
}

//------------------------------------------------------------------------------
/// Configures the EBI for NorFlash access at 48MHz.
/// \Param busWidth Bus width 
//------------------------------------------------------------------------------
void BOARD_ConfigureNorFlash48MHz(unsigned char busWidth)
{
    // Configure SMC
    AT91C_BASE_SMC->SMC_SETUP0 = 0x00000001;
    AT91C_BASE_SMC->SMC_PULSE0 = 0x07070703;
    AT91C_BASE_SMC->SMC_CYCLE0 = 0x00070007;
    AT91C_BASE_SMC->SMC_CTRL0  = (AT91C_SMC_READMODE
                                  | AT91C_SMC_WRITEMODE
                                  | AT91C_SMC_NWAITM_NWAIT_DISABLE
                                  | ((0x1 << 16) & AT91C_SMC_TDF));
                           
    if (busWidth == 8) {

        AT91C_BASE_SMC->SMC_CTRL0 |= AT91C_SMC_DBW_WIDTH_EIGTH_BITS;
    }
    else if (busWidth == 16) {
 
        AT91C_BASE_SMC->SMC_CTRL0 |= AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS;
    }
    else if (busWidth == 32) {
 
        AT91C_BASE_SMC->SMC_CTRL0 |= AT91C_SMC_DBW_WIDTH_THIRTY_TWO_BITS;
    }
}

//------------------------------------------------------------------------------
/// Configures the EBI for NandFlash access. Pins must be configured after or
/// before calling this function.
//------------------------------------------------------------------------------
void BOARD_ConfigureDPRAM(unsigned char busWidth)
{
    // Configure SMC
    AT91C_BASE_SMC->SMC_SETUP2 = 0x01020102;    //����datasheetʱ������
    AT91C_BASE_SMC->SMC_PULSE2 = 0x04020402;
    AT91C_BASE_SMC->SMC_CYCLE2 = 0x00060006;
    AT91C_BASE_SMC->SMC_CTRL2  = 0x00110033;

    if (busWidth == 8) {

        AT91C_BASE_SMC->SMC_CTRL2 |= AT91C_SMC_DBW_WIDTH_EIGTH_BITS;
    }
    else if (busWidth == 16) {
 
        AT91C_BASE_SMC->SMC_CTRL2 |= AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS;
    }
}

//------------------------------------------------------------------------------
/// Set flash wait states in the EFC for 48MHz
//------------------------------------------------------------------------------
void BOARD_ConfigureFlash48MHz(void)
{
    // Set flash wait states
    //----------------------
    AT91C_BASE_EFC->EFC_FMR = 6 << 8;
}