;/*****************************************************************************/
; OSasm.s: low-level OS commands, written in assembly                       */
; Runs on LM4F120/TM4C123/MSP432
; Lab 4 starter file
; March 25, 2016

;


        AREA |.text|, CODE, READONLY, ALIGN=2
        THUMB
        REQUIRE8
        PRESERVE8

        EXTERN  RunPt            ; currently running thread
        EXPORT  StartOS
        EXPORT  SysTick_Handler
        IMPORT  Scheduler
        EXPORT  PendSV_Handler

SysTick_Handler                ; 1) Saves R0-R3,R12,LR,PC,PSR
    CPSID   I                  ; 2) Prevent interrupt during switch
    ; same as Lab 3
	PUSH {R4-R11}			   ; 3) Save rem. R4-R11
	LDR R0,=RunPt			   ; 4) R0 = RunPt, old thread
	LDR R1, [R0]			   ; 5) R1= RunPt->sp
	STR SP, [R1]			   ; 6) Store sp to tcb
	PUSH {R0,LR}			   
	BL Scheduler
	POP {R0, LR}
	LDR R1, [R0]			   ; 7) R1 = RunPt, new thread
	LDR SP, [R1]			   ; 8) new thread, SP= RunPt->sp
	POP {R4-R11}			   
	CPSIE   I                  ; 9) tasks run with interrupts enabled
    BX      LR                 ; 10) restore R0-R3,R12,LR,PC,PSR

StartOS
    LDR     R0, =RunPt         ; currently running thread
    ; same as Lab 3
	LDR R1, [R0]			   ; R1 = RunPt
	LDR SP, [R1]			   ; SP = RunPt->sp
	POP {R4-R11}			   ; Restore reg R4-R11
	POP {R0-R3}				   ; Restore R0-R3
	POP {R12}				   ; Restore R12
	ADD SP,SP,#4			   ; Discard LR of Initial Stack
	POP {LR}				   ; Starting location
	ADD SP,SP,#4			   ; Discard PSR
    CPSIE   I                  ; Enable interrupts at processor level
    BX      LR                 ; start first thread

PendSV_Handler
    LDR     R0, =RunPt         ; run this thread next
    LDR     R2, [R0]           ; R2 = value of RunPt
    LDR     SP, [R2]           ; new thread SP; SP = RunPt->stackPointer;
    POP     {R4-R11}           ; restore regs r4-11
    LDR     LR,=0xFFFFFFF9
    BX      LR                 ; start next thread
	
    ALIGN
    END
