;*******************************************************************************
;     (c) Copyright 2004, SUPCON
;         All Rights Reserved
;
;            ARM9 Port
;            IAR  v5.x
;
; Description 	: Interrupt vector file and uC/OS-II Interrupt Service Routines(ISRs)
; File        	: vectors.s
; Modified By 	: tangjiansheng
; Data        	: 2008.1.15
;********************************************************************************

    

		SECTION .vecfucs:CODE:NOROOT(2)

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#define __ASSEMBLY__
#include "board.h"

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

#define ARM_MODE_ABORT     0x17
#define ARM_MODE_USER	 0x10
#define ARM_MODE_FIQ     0x11
#define ARM_MODE_IRQ     0x12
#define ARM_MODE_SVC     0x13
#define ARM_MODE_UNDEF	 0x1B
#define ARM_MODE_SYS     0x1F

#define I_BIT            0x80
#define F_BIT            0x40


;       ORG 0x00000                   
    	CODE32
    
       EXPORT   ?UndefHandler
       EXPORT   ?SWIHandler
       EXPORT   ?PrefetchAbortHandler      
       EXPORT   ?DataAbortHandler  
/* 
?UndefHandler 		   b    ?UndefHandler
?SWIHandler		       b    ?SWIHandler
?PrefetchAbortHandler  b    ?PrefetchAbortHandler
?DataAbortHandler      b    ?DataAbortHandler    
*/

    IMPORT    ExpBug               
;    MAP 0
ARMErrFlag  	EQU  0
ExpRN       	EQU 4
ExpSP       	EQU 56
ExpLR       	EQU 60
ExpPC       	EQU 64
ExpCPSR     	EQU 68     
ExpPCCode   	EQU 72
/**/        
 ; ************************
; Dummy Exception Handlers
; ************************
; The following handlers do not do anything useful in this example.
; They are set up here for completeness.


    IMPORT    ExpBug
   


    IMPORT    Undefined_instruction_entry
?UndefHandler 
    STMDB sp!,{r0-r12,lr}  

    msr   CPSR_c, #ARM_MODE_UNDEF|I_BIT|F_BIT         ; 关中断

    STMFD sp!,{r0-r2}
    ldr   r1, =ExpBug               ; 

    str   r0, [r1, #ExpRN+ARMErrFlag]          ; 保存R0的值．

    SUB   r0, lr, #4
    str   r0, [r1, #ExpPC]          ; 保存触发异常pc
    mrs   r2, SPSR
    str   r2, [r1, #ExpCPSR]        ; 保存触发异常时的CPSR
    ldr   r2, [r0]    ;R0为PC值
    str   r2, [r1, #ExpPCCode]      ; 保存触发异常时的指令值
    LDMFD sp!, {r0-r2}
 
    msr   CPSR_c, #ARM_MODE_SVC|I_BIT|F_BIT        ;  关中断,并切换回SVC模式
    ldr   r0, =ExpBug
    add   r0, r0, #(ExpRN+4)        ;r0保存了结构体中r1的绝对地址
    STMIA r0!,{r1-r12, sp, lr}      ;保存r1-r12, sp, lr
    
    msr   CPSR_c, #ARM_MODE_UNDEF|I_BIT|F_BIT          ;  关中断,并切换回UND模式

UND_NOMAL   
    MOV   r0, lr
    msr   CPSR_c, #ARM_MODE_SVC|I_BIT|F_BIT
    BL    Undefined_instruction_entry
    msr   CPSR_c, #ARM_MODE_UNDEF|I_BIT|F_BIT

    LDMIA sp!,{r0-r12,pc}^
  LTORG 
 
;SWI 包含需要的例程在位 8-23 中和数据(如果有的话)在位 0-7 中  
    IMPORT    Software_interrupt_entry
?SWIHandler
    STMFD sp!,{r0-r12,lr}
//    msr   CPSR_c, #ARM_MODE_SVC|I_BIT|F_BIT   ; 关中断

    STMFD sp!,{r0-r2}
    ldr   r1, =ExpBug               ; 
    str   r0, [r1, #ExpRN]          ; 保存R0的值．
    SUB   r0, lr, #4
    str   r0, [r1, #ExpPC]          ; 保存触发异常pc
    mrs   r2, SPSR
    str   r2, [r1, #ExpCPSR]        ; 保存触发异常时的CPSR
    ldr   r2, [r0]
    str   r2, [r1, #ExpPCCode]      ; 保存触发异常时的指令值
    LDMFD sp!, {r0-r2}

    ldr   r0, =ExpBug
    add   r0, r0, #(ExpRN+4)        ;r0保存了结构体中r1的绝对地址
    STMIA r0!,{r1-r12, sp, lr}      //保存r1-r12, sp, lr 到 ExpBug
 
SWI_NOMAL
/*
兼容thumb和arm模式
    MRS     r0, spsr               ; Get spsr
    TST     r0, #T_BIT             ; Occurred in Thumb state?
    LDRNEH r0, [lr,#-2]           ; Yes: Load halfword and...
    BICNE   r0, r0, #0xFF00        ; ...extract comment field
    LDREQ   r0, [lr,#-4]           ; No: Load word and...
    BICEQ   r0, r0, #0xFF000000    ; ...extract comment field
*/    
    LDR   r0, [lr,#-4]           ; No: Load word and...
    BIC   r0, r0, #0xFF000000    ; ...extract comment field//得到异常中断号

    MRS    R2,CPSR
    BIC    R2,R2,#I_BIT
    MSR    CPSR_C,R2

  
    BL    Software_interrupt_entry
    LDMFD sp!,{r0-r12,pc}^ 
    LTORG 

    
    IMPORT    Abort_prefetch_entry  
?PrefetchAbortHandler   

;   SUB   lr,lr,#4          ;直接执行下面的程序，不进行第二次
    STMFD sp!,{r0-r12,lr}
    msr   CPSR_c, #ARM_MODE_ABORT|I_BIT|F_BIT        ;  关中断
  
    STMFD sp!,{r0-r2}
    ldr   r1, =ExpBug               ; 
    str   r0, [r1, #ExpRN]          ; 保存R0的值．
    SUB   r0, lr, #4
    str   r0, [r1, #ExpPC]          ; 保存触发异常pc
    mrs   r2, SPSR
    str   r2, [r1, #ExpCPSR]        ; 保存触发异常时的CPSR
;    ldr   r2, [r0]       ; 不能再去取这条指令了，取这条指令发生了异常。
;    str   r2, [r1, #ExpPCCode]      ; 保存触发异常时的指令值
    LDMFD sp!, {r0-r2}

    msr   CPSR_c, #ARM_MODE_SVC|I_BIT|F_BIT       
    ldr   r0, =ExpBug
    add   r0, r0, #(ExpRN+4)        ;r0保存了结构体中r1的绝对地址
    STMIA r0!,{r1-r12, sp, lr}      ;保存r1-r12, sp, lr 

    msr   CPSR_c, #ARM_MODE_ABORT|I_BIT|F_BIT          ;  关中断,并切换回SVC模式

Prefetch_NOMAL    
    SUB   r0, lr, #4      ;触发异常的指令地址
    msr   CPSR_c, #ARM_MODE_SVC|I_BIT|F_BIT 
    BL    Abort_prefetch_entry
    msr   CPSR_c, #ARM_MODE_ABORT|I_BIT|F_BIT
    LDMFD sp!,{r0-r12,pc}^ 
   LTORG 

          
    IMPORT  Abort_data_entry  
?DataAbortHandler
 
  SUB   lr,lr,#4                 //LR=LR-4
    STMFD sp!,{r0-r12,lr}
    msr   CPSR_c, #ARM_MODE_ABORT|I_BIT|F_BIT          ;  关中断

    STMFD sp!,{r0-r2}
    ldr   r1, =ExpBug               ; 
   str   r0, [r1, #ExpRN]          ; 保存R0的值．
    SUB   r0, lr, #4              //R0=LR-8;加上前面的一次；指向导致数据终止的那条指令
    str   r0, [r1, #ExpPC]          ; 保存触发异常pc
    mrs   r2, SPSR
    str   r2, [r1, #ExpCPSR]        ; 保存触发异常时的CPSR
    ldr   r2, [r0]
    str   r2, [r1, #ExpPCCode]      ; 保存触发异常时的指令值
    LDMFD sp!, {r0-r2}

    msr   CPSR_c, #ARM_MODE_SVC|I_BIT|F_BIT        //; 关中断,并切换回SVC模式
    ldr   r0, =ExpBug
    add   r0, r0, #(ExpRN+4)        ;r0保存了结构体中r1的绝对地址
    STMIA r0!,{r1-r12, sp, lr}      ;保存r1-r12, sp, lr

    msr   CPSR_c, #ARM_MODE_ABORT|I_BIT|F_BIT      

DATAABT_NOMAL

   ldr   r0, [lr, #-4]     //触发异常的指令地址，用于参数传递
   msr   CPSR_c, #ARM_MODE_SVC|I_BIT|F_BIT 
   BL    Abort_data_entry
   msr   CPSR_c, #ARM_MODE_ABORT|I_BIT|F_BIT
 LDMFD sp!,{r0-r12,pc}^ 
 
   LTORG
   
    
/*    
     COMMON    FctList:CODE:ROOT(2)
     ORG 0x00000         
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	
;
;  以下部分为仿真平台以函数指针或者结构体指针
;  的形式提供给用户程序的，共可以提供16个函数,
;  起始位置为0x40
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;		
;;;;; 绝对地址为0x40	
;;0x40
        ;--------------变量相关操作函数-----------------;
        IMPORT    InsertODDesc
        IMPORT    InsertVarToOD
        IMPORT    ReadVarFromOD
        IMPORT    WriteVarToOD
    
        IMPORT   ReadReqSend
        IMPORT    WriteReqSend
        ;--------------域相关操作函数-----------------;
        IMPORT   DomainUReqSend
        IMPORT   DomainDReqSend

        IMPORT   InsertEventToOD

	;--------------事件相关操作函数-----------------;
        IMPORT   EventNoti_Send
        IMPORT   AltEventCndMir_Req
        IMPORT   AckEventNoti_Req
        ;IMPORT   

					
		;  供仿真平台用户使用的库函数
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
          
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
          
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
          
        
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
          
      DATA        
	;--------------变量相关操作函数-----------------;
	DCD		InsertODDesc
	DCD		InsertVarToOD
	DCD		ReadVarFromOD
	DCD		WriteVarToOD
	
	DCD		ReadReqSend
	DCD		WriteReqSend			
	;--------------域相关操作函数-----------------;
	DCD		DomainUReqSend
	DCD		DomainDReqSend
	
	DCD		0
	DCD		0
	DCD		0
        ;--------------事件相关操作函数-----------------;
	DCD		InsertEventToOD
	
	DCD		EventNoti_Send
	DCD		AltEventCndMir_Req
	DCD		AckEventNoti_Req
	DCD		0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	
;
;  以下部分为仿真平台直接以函数形式提供给用户
;  程序的，共可以提供16个函数，起始位置为0xC0
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;		
;;;;; 绝对地址为0x4280		
;;;;; 16个函数直接使用的


        IMPORT    OpenFile
        IMPORT    ReadFile
        IMPORT    WriteFile
        IMPORT    CloseFile
        
        IMPORT    DeleteFile
        IMPORT    memset
        IMPORT    memcpy
        IMPORT    memcmp
    					
        IMPORT    strlen
        IMPORT    strcpy
        IMPORT    strcmp
        IMPORT    srand		

        IMPORT    rand
        IMPORT    __rt_udiv
        IMPORT    __rt_sdiv
        IMPORT    __rt_div0
        
        
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
          
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
          
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
          
        
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
         ldr         pc, [pc, #0x3C]          ; 
                 


	DATA														;;0x100
	
	DCD		OpenFile
	DCD		ReadFile
	DCD		WriteFile
	DCD		CloseFile
	
	DCD		DeleteFile
	DCD		memset
	DCD		memcpy
	DCD		memcmp

	DCD		strlen
	DCD		strcpy
	DCD		strcmp
	DCD		srand

	DCD		rand
	DCD		0//__rt_udiv
	DCD		0//__rt_sdiv
	DCD		0//__rt_div0

;文件系统的内容
 */  
         END


