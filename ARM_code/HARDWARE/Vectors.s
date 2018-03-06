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

    msr   CPSR_c, #ARM_MODE_UNDEF|I_BIT|F_BIT         ; ���ж�

    STMFD sp!,{r0-r2}
    ldr   r1, =ExpBug               ; 

    str   r0, [r1, #ExpRN+ARMErrFlag]          ; ����R0��ֵ��

    SUB   r0, lr, #4
    str   r0, [r1, #ExpPC]          ; ���津���쳣pc
    mrs   r2, SPSR
    str   r2, [r1, #ExpCPSR]        ; ���津���쳣ʱ��CPSR
    ldr   r2, [r0]    ;R0ΪPCֵ
    str   r2, [r1, #ExpPCCode]      ; ���津���쳣ʱ��ָ��ֵ
    LDMFD sp!, {r0-r2}
 
    msr   CPSR_c, #ARM_MODE_SVC|I_BIT|F_BIT        ;  ���ж�,���л���SVCģʽ
    ldr   r0, =ExpBug
    add   r0, r0, #(ExpRN+4)        ;r0�����˽ṹ����r1�ľ��Ե�ַ
    STMIA r0!,{r1-r12, sp, lr}      ;����r1-r12, sp, lr
    
    msr   CPSR_c, #ARM_MODE_UNDEF|I_BIT|F_BIT          ;  ���ж�,���л���UNDģʽ

UND_NOMAL   
    MOV   r0, lr
    msr   CPSR_c, #ARM_MODE_SVC|I_BIT|F_BIT
    BL    Undefined_instruction_entry
    msr   CPSR_c, #ARM_MODE_UNDEF|I_BIT|F_BIT

    LDMIA sp!,{r0-r12,pc}^
  LTORG 
 
;SWI ������Ҫ��������λ 8-23 �к�����(����еĻ�)��λ 0-7 ��  
    IMPORT    Software_interrupt_entry
?SWIHandler
    STMFD sp!,{r0-r12,lr}
//    msr   CPSR_c, #ARM_MODE_SVC|I_BIT|F_BIT   ; ���ж�

    STMFD sp!,{r0-r2}
    ldr   r1, =ExpBug               ; 
    str   r0, [r1, #ExpRN]          ; ����R0��ֵ��
    SUB   r0, lr, #4
    str   r0, [r1, #ExpPC]          ; ���津���쳣pc
    mrs   r2, SPSR
    str   r2, [r1, #ExpCPSR]        ; ���津���쳣ʱ��CPSR
    ldr   r2, [r0]
    str   r2, [r1, #ExpPCCode]      ; ���津���쳣ʱ��ָ��ֵ
    LDMFD sp!, {r0-r2}

    ldr   r0, =ExpBug
    add   r0, r0, #(ExpRN+4)        ;r0�����˽ṹ����r1�ľ��Ե�ַ
    STMIA r0!,{r1-r12, sp, lr}      //����r1-r12, sp, lr �� ExpBug
 
SWI_NOMAL
/*
����thumb��armģʽ
    MRS     r0, spsr               ; Get spsr
    TST     r0, #T_BIT             ; Occurred in Thumb state?
    LDRNEH r0, [lr,#-2]           ; Yes: Load halfword and...
    BICNE   r0, r0, #0xFF00        ; ...extract comment field
    LDREQ   r0, [lr,#-4]           ; No: Load word and...
    BICEQ   r0, r0, #0xFF000000    ; ...extract comment field
*/    
    LDR   r0, [lr,#-4]           ; No: Load word and...
    BIC   r0, r0, #0xFF000000    ; ...extract comment field//�õ��쳣�жϺ�

    MRS    R2,CPSR
    BIC    R2,R2,#I_BIT
    MSR    CPSR_C,R2

  
    BL    Software_interrupt_entry
    LDMFD sp!,{r0-r12,pc}^ 
    LTORG 

    
    IMPORT    Abort_prefetch_entry  
?PrefetchAbortHandler   

;   SUB   lr,lr,#4          ;ֱ��ִ������ĳ��򣬲����еڶ���
    STMFD sp!,{r0-r12,lr}
    msr   CPSR_c, #ARM_MODE_ABORT|I_BIT|F_BIT        ;  ���ж�
  
    STMFD sp!,{r0-r2}
    ldr   r1, =ExpBug               ; 
    str   r0, [r1, #ExpRN]          ; ����R0��ֵ��
    SUB   r0, lr, #4
    str   r0, [r1, #ExpPC]          ; ���津���쳣pc
    mrs   r2, SPSR
    str   r2, [r1, #ExpCPSR]        ; ���津���쳣ʱ��CPSR
;    ldr   r2, [r0]       ; ������ȥȡ����ָ���ˣ�ȡ����ָ������쳣��
;    str   r2, [r1, #ExpPCCode]      ; ���津���쳣ʱ��ָ��ֵ
    LDMFD sp!, {r0-r2}

    msr   CPSR_c, #ARM_MODE_SVC|I_BIT|F_BIT       
    ldr   r0, =ExpBug
    add   r0, r0, #(ExpRN+4)        ;r0�����˽ṹ����r1�ľ��Ե�ַ
    STMIA r0!,{r1-r12, sp, lr}      ;����r1-r12, sp, lr 

    msr   CPSR_c, #ARM_MODE_ABORT|I_BIT|F_BIT          ;  ���ж�,���л���SVCģʽ

Prefetch_NOMAL    
    SUB   r0, lr, #4      ;�����쳣��ָ���ַ
    msr   CPSR_c, #ARM_MODE_SVC|I_BIT|F_BIT 
    BL    Abort_prefetch_entry
    msr   CPSR_c, #ARM_MODE_ABORT|I_BIT|F_BIT
    LDMFD sp!,{r0-r12,pc}^ 
   LTORG 

          
    IMPORT  Abort_data_entry  
?DataAbortHandler
 
  SUB   lr,lr,#4                 //LR=LR-4
    STMFD sp!,{r0-r12,lr}
    msr   CPSR_c, #ARM_MODE_ABORT|I_BIT|F_BIT          ;  ���ж�

    STMFD sp!,{r0-r2}
    ldr   r1, =ExpBug               ; 
   str   r0, [r1, #ExpRN]          ; ����R0��ֵ��
    SUB   r0, lr, #4              //R0=LR-8;����ǰ���һ�Σ�ָ����������ֹ������ָ��
    str   r0, [r1, #ExpPC]          ; ���津���쳣pc
    mrs   r2, SPSR
    str   r2, [r1, #ExpCPSR]        ; ���津���쳣ʱ��CPSR
    ldr   r2, [r0]
    str   r2, [r1, #ExpPCCode]      ; ���津���쳣ʱ��ָ��ֵ
    LDMFD sp!, {r0-r2}

    msr   CPSR_c, #ARM_MODE_SVC|I_BIT|F_BIT        //; ���ж�,���л���SVCģʽ
    ldr   r0, =ExpBug
    add   r0, r0, #(ExpRN+4)        ;r0�����˽ṹ����r1�ľ��Ե�ַ
    STMIA r0!,{r1-r12, sp, lr}      ;����r1-r12, sp, lr

    msr   CPSR_c, #ARM_MODE_ABORT|I_BIT|F_BIT      

DATAABT_NOMAL

   ldr   r0, [lr, #-4]     //�����쳣��ָ���ַ�����ڲ�������
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
;  ���²���Ϊ����ƽ̨�Ժ���ָ����߽ṹ��ָ��
;  ����ʽ�ṩ���û�����ģ��������ṩ16������,
;  ��ʼλ��Ϊ0x40
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;		
;;;;; ���Ե�ַΪ0x40	
;;0x40
        ;--------------������ز�������-----------------;
        IMPORT    InsertODDesc
        IMPORT    InsertVarToOD
        IMPORT    ReadVarFromOD
        IMPORT    WriteVarToOD
    
        IMPORT   ReadReqSend
        IMPORT    WriteReqSend
        ;--------------����ز�������-----------------;
        IMPORT   DomainUReqSend
        IMPORT   DomainDReqSend

        IMPORT   InsertEventToOD

	;--------------�¼���ز�������-----------------;
        IMPORT   EventNoti_Send
        IMPORT   AltEventCndMir_Req
        IMPORT   AckEventNoti_Req
        ;IMPORT   

					
		;  ������ƽ̨�û�ʹ�õĿ⺯��
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
	;--------------������ز�������-----------------;
	DCD		InsertODDesc
	DCD		InsertVarToOD
	DCD		ReadVarFromOD
	DCD		WriteVarToOD
	
	DCD		ReadReqSend
	DCD		WriteReqSend			
	;--------------����ز�������-----------------;
	DCD		DomainUReqSend
	DCD		DomainDReqSend
	
	DCD		0
	DCD		0
	DCD		0
        ;--------------�¼���ز�������-----------------;
	DCD		InsertEventToOD
	
	DCD		EventNoti_Send
	DCD		AltEventCndMir_Req
	DCD		AckEventNoti_Req
	DCD		0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	
;
;  ���²���Ϊ����ƽֱ̨���Ժ�����ʽ�ṩ���û�
;  ����ģ��������ṩ16����������ʼλ��Ϊ0xC0
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;		
;;;;; ���Ե�ַΪ0x4280		
;;;;; 16������ֱ��ʹ�õ�


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

;�ļ�ϵͳ������
 */  
         END


