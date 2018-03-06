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

/*
     IAR startup file for AT91SAM9XE microcontrollers.
 */

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION FIQ_STACK:DATA:NOROOT(2)
        SECTION SYS_STACK:DATA:NOROOT(2)
        SECTION IRQ_STACK:DATA:NOROOT(2)
        SECTION ABT_STACK:DATA:NOROOT(2)
        SECTION UND_STACK:DATA:NOROOT(2)
        SECTION CSTACK:DATA:NOROOT(3)

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#define __ASSEMBLY__
#include "board.h"

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

#define ARM_MODE_ABORT   0x17
#define ARM_MODE_USER	 0x10
#define ARM_MODE_FIQ     0x11
#define ARM_MODE_IRQ     0x12
#define ARM_MODE_SVC     0x13
#define ARM_MODE_UNDEF	 0x1B
#define ARM_MODE_SYS     0x1F

#define NO_INT           0xC0
#define I_BIT            0x80
#define F_BIT            0x40

//PCJUMP EQU 0x20031160
PCJUMP EQU 0x200456cc
//PCJUMP EQU 0x200247a8

#define RunInRam 0
FLASH_ADDR EQU 0x200000
SDRAM_ADDR EQU 0x20000000


//------------------------------------------------------------------------------
//         Startup routine
//------------------------------------------------------------------------------

/*
   Exception vectors
 */
        SECTION .vectors:CODE:NOROOT(2)

        PUBLIC  resetVector
        ;PUBLIC  irqHandler


        IMPORT  FIQ_Handler

	IMPORT   ?UndefHandler
       	IMPORT   ?SWIHandler
       	IMPORT   ?PrefetchAbortHandler      
       	IMPORT   ?DataAbortHandler  
       	IMPORT	OSIntEnter
       	IMPORT  OSIntExit
        IMPORT  OSIntNesting
        IMPORT  OSTCBCur
       	EXTERN  OS_CPU_IRQ_ISR_Handler

        ARM

__iar_init$$done:               ; The interrupt vector is not needed
                                ; until after copy initialization is done

resetVector:
        ; All default exception handlers (except reset) are
        ; defined as weak symbol definitions.
        ; If a handler is defined by the application it will take precedence.
        LDR     pc, =resetHandler        ; Reset
        LDR     pc, Undefined_Addr       ; Undefined instructions
        LDR     pc, SWI_Addr             ; Software interrupt (SWI/SYS)
        LDR     pc, Prefetch_Addr        ; Prefetch abort
        LDR     pc, Abort_Addr           ; Data abort
        B       .                        ; RESERVED
        LDR     pc, =OS_CPU_IRQ_ISR;[pc,#-0xF20];=INT_TC0          ; IRQ Read AIC
        LDR     pc, FIQ_Addr             ; FIQ

Undefined_Addr: DCD   ?UndefHandler
SWI_Addr:       DCD   ?SWIHandler
Prefetch_Addr:  DCD   ?PrefetchAbortHandler
Abort_Addr:     DCD   ?DataAbortHandler
FIQ_Addr:       DCD   FIQ_Handler
	
/*
   Handles incoming interrupt requests by branching to the corresponding
   handler, as defined in the AIC. Supports interrupt nesting.
 */
OS_CPU_IRQ_ISR

        STMFD   SP!, {R1-R3}                   ; 工作寄存器压入IRQ堆栈
        MOV     R1, SP                         ; 保存IRQ堆栈指针
        ADD     SP, SP,#12                     ; 调整IRQ堆栈指针
        SUB     R2, LR,#4                      ; 调整PC返回地址

        MRS     R3, SPSR                       ; 将SPSR(即中断任务的CPSR)复制到R3
        
        MSR     CPSR_c, #(NO_INT | ARM_MODE_SVC) ; //切换到管理模式

                                               ; 将任务的上下文(CONTEXT)保存到任务堆栈
        STMFD   SP!, {R2}                      ; 任务的返回地址压栈
        STMFD   SP!, {LR}                      ; 任务的LR压栈
        STMFD   SP!, {R4-R12}                  ; 任务的R12-R4压栈

        LDMFD   R1!, {R4-R6}                   ; 将任务的R1-R3从IRQ堆栈移动到管理堆栈
        STMFD   SP!, {R4-R6}
        STMFD   SP!, {R0}                      ; 任务的R0压入任务堆栈
        STMFD   SP!, {R3}                      ; //任务的CPSR(即IRQ's SPSR)压栈

                                               ; 处理嵌套计数器
        LDR     R0, =OSIntNesting            ; OSIntNesting++;
        LDRB    R1, [R0]
        ADD     R1, R1,#1
        STRB    R1, [R0]

        CMP     R1, #1                         ; if (OSIntNesting == 1) {
        BNE     OS_CPU_IRQ_ISR_1
        LDR     R4, =OSTCBCur                ;   OSTCBCur->OSTCBStkPtr = SP
        LDR     R5, [R4]
        STR     SP, [R5]                       ; }

OS_CPU_IRQ_ISR_1
        MSR     CPSR_c, #(NO_INT | ARM_MODE_IRQ) ; 切换到IRQ模式(使用IRQ堆栈处理中断)

        LDR     R0, =OS_CPU_IRQ_ISR_Handler   ; OS_CPU_IRQ_ISR_Handler();
        MOV     LR, PC
        BX      R0

        LDR     lr, =AT91C_BASE_AIC
        STR     lr, [r14, #AIC_EOICR]

        MSR     CPSR_c, #(NO_INT | ARM_MODE_SVC) ; 切换到管理模式

        LDR     R0, =OSIntExit               ; OSIntExit();
        MOV     LR, PC
        BX      R0

                                               ; 恢复新任务的上下文(CONTEXT)
        LDMFD   SP!, {R4}                      ; 新任务的CPSR出栈
        MSR     SPSR_cxsf, R4

        LDMFD   SP!, {R0-R12,LR,PC}^           ; 新任务的上下文(context)出栈


/*
   After a reset, execution starts here, the mode is ARM, supervisor
   with interrupts disabled.
   Initializes the chip and branches to the main() function.
 */
        SECTION .cstartup:CODE:NOROOT(2)

        PUBLIC  resetHandler
        EXTERN  LowLevelInit
        EXTERN  ?main
        REQUIRE resetVector
        ARM



resetHandler:

        /* Set pc to actual code location (i.e. not in remap zone) */
 	    LDR     pc, =label

        /* Perform low-level initialization of the chip using LowLevelInit() */
label:
        
        ;LDR     r0, =LowLevelInit
        ;LDR     r4, =SFE(CSTACK)
        ;MOV     sp, r4
        ;BLX     r0
            
		/* Set up the FIQ stack pointer. */
        MSR     cpsr_c, #ARM_MODE_FIQ | I_BIT | F_BIT      ; Change the mode
        LDR     sp, =SFE(FIQ_STACK)

        /* Set up the interrupt stack pointer. */
        MSR     cpsr_c, #ARM_MODE_IRQ | I_BIT | F_BIT      ; Change the mode
        LDR     sp, =SFE(IRQ_STACK)

        /* Set up the SYS stack pointer. */
        MSR     cpsr_c, #ARM_MODE_SYS | I_BIT | F_BIT      ; Change the mode
        LDR     sp, =SFE(SYS_STACK)   

        /* Set up the Abort stack pointer. */
        MSR     cpsr_c, #ARM_MODE_ABORT | I_BIT | F_BIT      ; Change the mode
        LDR     sp, =SFE(ABT_STACK)

        /* Set up the Undef stack pointer. */
        MSR     cpsr_c, #ARM_MODE_UNDEF | F_BIT 
        ; Change the mode
        LDR     sp, =SFE(UND_STACK)
/*-------------------------------------------------*/
        /* Set up the SVC stack pointer. */
        MSR     cpsr_c, #ARM_MODE_SVC | F_BIT      ; Change the mode
        LDR     r0, =LowLevelInit
        LDR     r4, =SFE(CSTACK)
        MOV     sp, r4
        BLX     r0
/*-------------------------------------------------*/       
        /* Branch to main() */
        //LDR    r0, =PCJUMP //先到flash跑一下，此时注释掉前2句，要写到内存在恢复，跑完记录地址，去改上面的PCJUMP
                                            
#if RunInRam
       LDR     R5, =FLASH_ADDR
       LDR     R6, =?main
       SUB     r0, R6, R5
       ADD     r0, r0, #SDRAM_ADDR
       MOV    pc, r0
#endif

        //MOV    pc, r0
        LDR     r0, =?main
        BLX     r0
        
        /* Loop indefinitely when program is finished */
loop4:
        B       loop4

        END
