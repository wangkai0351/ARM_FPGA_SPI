/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

// peripherals/emac/emac.h

#ifndef EMAC_H
#define EMAC_H

//-----------------------------------------------------------------------------
/// \dir
/// !Purpose
///
///     Definition of methods and structures for using EMAC
///     
/// !Usage
///
/// -# Initialize EMAC with EMAC_Init with MAC address.
/// -# Then the caller application need to initialize the PHY driver before further calling EMAC
///      driver.
/// -# Get a packet from network
///      -# Interrupt mode: EMAC_Set_RxCb to register a function to process the frame packet
///      -# Polling mode: EMAC_Poll for a data packet from network 
/// -# Send a packet to network with EMAC_Send.
///
/// Please refer to the list of functions in the #Overview# tab of this unit
/// for more detailed information.
//-----------------------------------------------------------------------------
#define EMAC_TEN_M	0
#define EMAC_HUNDRED_M       1  
#define EMAC_FULL_DUPLEX       1
#define EMAC_HALF_DUPLEX	    0
//-----------------------------------------------------------------------------
//         Headers
//-----------------------------------------------------------------------------
#include "includes.h"

#define    PHY_ADDR  0x01
#define    BMCR  0x00
#define    RBR  0x17
#define    PHYCR  0x19
#define    PHYSTS 0x10
#define    ANAR   0x04
#define    BMSR   0x01
#define    MICR    0x11
#define    MISR    0x12
#define    LEDCR  0x18
//-----------------------------------------------------------------------------
//         Definitions
//-----------------------------------------------------------------------------
/// Number of buffer for RX, be carreful: MUST be 2^n
#define RX_BUFFERS  256
/// Number of buffer for TX, be carreful: MUST be 2^n
#define TX_BUFFERS   50

/// Buffer Size
#define EMAC_RX_UNITSIZE            128     /// Fixed size for RX buffer
#define EMAC_TX_UNITSIZE            1518    /// Size for ETH frame length

// The MAC can support frame lengths up to 1536 bytes.
#define EMAC_FRAME_LENTGH_MAX       1536


//-----------------------------------------------------------------------------
//         Types
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Describes the statistics of the EMAC.
//-----------------------------------------------------------------------------
typedef struct _EmacStats {

    // TX errors
    unsigned int tx_packets;    /// Total Number of packets sent
    unsigned int tx_comp;       /// Packet complete
    unsigned int tx_errors;     /// TX errors ( Retry Limit Exceed )
    unsigned int collisions;    /// Collision
    unsigned int tx_exausts;    /// Buffer exhausted
    unsigned int tx_underruns;  /// Under Run, not able to read from memory
    // RX errors
    unsigned int rx_packets;    /// Total Number of packets RX
    unsigned int rx_eof;        /// No EOF error
    unsigned int rx_ovrs;       /// Over Run, not able to store to memory
    unsigned int rx_bnas;       /// Buffer is not available

} EmacStats, *PEmacStats;


//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------
/// The buffer addresses written into the descriptors must be aligned so the
/// last few bits are zero.  These bits have special meaning for the EMAC
/// peripheral and cannot be used as part of the address.
#define EMAC_ADDRESS_MASK   ((unsigned int)0xFFFFFFFC)
#define EMAC_LENGTH_FRAME   ((unsigned int)0x0FFF)    /// Length of frame mask

// receive buffer descriptor bits
#define EMAC_RX_OWNERSHIP_BIT   (1 <<  0)
#define EMAC_RX_WRAP_BIT        (1 <<  1)
#define EMAC_RX_SOF_BIT         (1 << 14)
#define EMAC_RX_EOF_BIT         (1 << 15)

// Transmit buffer descriptor bits
#define EMAC_TX_LAST_BUFFER_BIT (1 << 15)
#define EMAC_TX_WRAP_BIT        (1 << 30)
#define EMAC_TX_USED_BIT        (1 << 31)

//-----------------------------------------------------------------------------
// Circular buffer management
//-----------------------------------------------------------------------------
// Return count in buffer
#define CIRC_CNT(head,tail,size) (((head) - (tail)) & ((size)-1))

// Return space available, 0..size-1
// We always leave one free char as a completely full buffer 
// has head == tail, which is the same as empty
#define CIRC_SPACE(head,tail,size) CIRC_CNT((tail),((head)+1),(size))

// Return count up to the end of the buffer.  
// Carefully avoid accessing head and tail more than once,
// so they can change underneath us without returning inconsistent results
#define CIRC_CNT_TO_END(head,tail,size) \
   ({int end = (size) - (tail); \
     int n = ((head) + end) & ((size)-1); \
     n < end ? n : end;})

// Return space available up to the end of the buffer
#define CIRC_SPACE_TO_END(head,tail,size) \
   ({int end = (size) - 1 - (head); \
     int n = (end + (tail)) & ((size)-1); \
     n <= end ? n : end+1;})

// Increment head or tail
#define CIRC_INC(headortail,size) \
        headortail++;             \
        if(headortail >= size) {  \
            headortail = 0;       \
        }

#define CIRC_EMPTY(circ)     ((circ)->head == (circ)->tail)
#define CIRC_CLEAR(circ)     ((circ)->head = (circ)->tail = 0)

//-----------------------------------------------------------------------------
//         EMAC Exported functions
//-----------------------------------------------------------------------------
/// Callback used by send function
typedef void (*EMAC_TxCallback)(unsigned int status);
typedef void (*EMAC_RxCallback)(unsigned int status);
typedef void (*EMAC_WakeupCallback)(void);


//------------------------------------------------------------------------------
//      Structures
//------------------------------------------------------------------------------
#ifdef __ICCARM__          // IAR
#pragma pack(4)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR
/// Describes the type and attribute of Receive Transfer descriptor.
typedef struct _EmacRxTDescriptor {
    unsigned int addr;
    unsigned int status;
} __attribute__((packed, aligned(8))) EmacRxTDescriptor, *PEmacRxTDescriptor;

/// Describes the type and attribute of Transmit Transfer descriptor.
typedef struct _EmacTxTDescriptor {
    unsigned int addr;
    unsigned int status;
} __attribute__((packed, aligned(8))) EmacTxTDescriptor, *PEmacTxTDescriptor;
#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

//QQQ ?-¡ä¨²??¨®D?¡¥??¡ê?The reason is that the #pragma directive tries to associate to the first declaration it sees: the declaration of the anonymous struct!
typedef struct {
   EmacRxTDescriptor td[RX_BUFFERS];
   EMAC_RxCallback rxCb; /// Callback function to be invoked once a frame has been received
   unsigned short idx;
} RxTd;

//QQQ
typedef struct {
   EmacTxTDescriptor td[TX_BUFFERS];
   EMAC_TxCallback txCb[TX_BUFFERS];    /// Callback function to be invoked once TD has been processed
   EMAC_WakeupCallback wakeupCb;        /// Callback function to be invoked once several TD have been released
   unsigned short wakeupThreshold; /// Number of free TD before wakeupCb is invoked
   unsigned short head;            /// Circular buffer head pointer incremented by the upper layer (buffer to be sent)
   unsigned short tail;            /// Circular buffer head pointer incremented by the IT handler (buffer sent)
} TxTd;

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------

/// Send Buffer
// Section 3.6 of AMBA 2.0 spec states that burst should not cross 1K Boundaries.
// Receive buffer manager writes are burst of 2 words => 3 lsb bits of the address shall be set to 0
#ifdef __ICCARM__          // IAR
#pragma data_alignment=8   // IAR
#endif                     // IAR

extern  EmacStats EmacStatistics;
extern  RxTd rxTd;
extern  TxTd txTd; 
//-----------------------------------------------------------------------------
//         PHY Exported functions
//-----------------------------------------------------------------------------
extern unsigned char EMAC_SetMdcClock( unsigned int mck );

extern void EMAC_EnableMdio( void );

extern void EMAC_DisableMdio( void );

extern void EMAC_EnableMII( void );

extern void EMAC_EnableRMII( void );

extern unsigned char EMAC_ReadPhy(unsigned char PhyAddress,
                                  unsigned char Address,
                                  unsigned int *pValue,
                                  unsigned int retry);

extern unsigned char EMAC_WritePhy(unsigned char PhyAddress,
                                   unsigned char Address,
                                   unsigned int Value,
                                   unsigned int retry);

extern void EMAC_SetLinkSpeed(unsigned char speed,
                              unsigned char fullduplex);


extern void EMAC_Init( unsigned char id, const unsigned char *pMacAddress,
                unsigned char enableCAF, unsigned char enableNBC );
#define EMAC_CAF_DISABLE  0
#define EMAC_CAF_ENABLE   1
#define EMAC_NBC_DISABLE  0
#define EMAC_NBC_ENABLE   1

extern void EMAC_ISR(void);

/*start 2013-12-20 modified by xjx, */
void EMAC_Send(void *pBuffer, 
                               unsigned int size, 
                               NET_ERR     *perr);
/*end 2013-12-20 modified by xjx*/

/// Return for EMAC_Send function
#define EMAC_TX_OK                     0
#define EMAC_TX_BUFFER_BUSY            1
#define EMAC_TX_INVALID_PACKET         2

/*start 2013-12-20 modified by xjx, */
extern unsigned char EMAC_Poll(unsigned char *pFrame,
                               unsigned int frameSize,
                               CPU_INT16U *pRcvSize);
/*end 2013-12-20 modified by xjx*/
/// Return for EMAC_Poll function
#define EMAC_RX_OK                   0
#define EMAC_RX_NO_DATA              1
#define EMAC_RX_FRAME_SIZE_TOO_SMALL 2

extern void EMAC_GetStatistics(EmacStats *pStats, unsigned char reset);

extern INT16U GetInputPacketLen(void);
extern void PHY83848_Init(void);
extern void EMAC_AutoLink();

#endif // #ifndef EMAC_H

