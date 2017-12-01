;/*****************************************************************************/
; OSasm.s: low-level OS commands, written in assembly                       */
; Runs on LM4F120/TM4C123/MSP432
; Lab 3 starter file
; March 2, 2016




        AREA |.text|, CODE, READONLY, ALIGN=2
        THUMB
        REQUIRE8
        PRESERVE8

        EXTERN  RunPt            ; currently running thread
        EXPORT  StartOS
        EXPORT  SysTick_Handler
        IMPORT  Scheduler


SysTick_Handler                ; 1) Saves R0-R3,R12,LR,PC,PSR
    CPSID   I                  ; 2) Prevent interrupt during switch
	PUSH {R4-R11}			   ; 3) Save regs R4-R11
	LDR R0,=RunPt			   ; 4) R0 = RunPt, old thread
	LDR R1, [R0]               ; 5) R1 = Runpt->sp
	STR SP, [R1]			   ; 6) Stores SP to TCB
	PUSH {R0,LR}
	BL Scheduler
	POP {R0, LR}
	LDR R1, [R0]			   ; 7) R1 = RunPt, new thread
	LDR SP, [R1]			   ; 8) new thread, SP = RunPt->sp
	POP {R4-R11}
    CPSIE   I                  ; 9) tasks run with interrupts enabled
    BX      LR                 ; 10) restore R0-R3,R12,LR,PC,PSR

StartOS
   ;YOU IMPLEMENT THIS (same as Lab 2)

	LDR R0,=RunPt			   ; current running pt
	LDR R1, [R0]	           ; R1 = RunPt
	LDR SP, [R1]			   ; SP = Runpt->SP
	POP {R4-R11}	           ; Restore R4-R11
	POP {R0-R3}                ; Restore R0-R3
	POP {R12}
	ADD SP, SP,#4			   ; Discard L.R of initial stack
	POP {LR}                   ; Starting Location
	ADD SP, SP,#4              ; Discard PSR
	CPSIE   I                  ; Enable interrupts at processor level
    BX      LR                 ; start first thread

    ALIGN
    END
