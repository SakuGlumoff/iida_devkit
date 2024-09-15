.syntax unified

.cpu cortex-m33
.fpu softvfp
.thumb

.word _sidata
.word _sdata
.word _edata
.word _sbss
.word _ebss

.equ BootRAM, 0xF1E0F85F

.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
	ldr sp, =_estack
	movs r1, #0
	b LoopCopyDataInit
CopyDataInit:
	ldr r3, =_sidata
	ldr r3, [r3, r1]
	str r3, [r0, r1]
	adds r1, r1, #4
LoopCopyDataInit:
	ldr r0, =_sdata
	ldr r3, =_edata
	adds r2, r0, r1
	cmp r2, r3
	bcc CopyDataInit
	ldr r2, =_sbss
	b LoopFillZerobss
FillZerobss:
	movs r3, #0
	str r3, [r2], #4
LoopFillZerobss:
	ldr r3, = _ebss
	cmp r2, r3
	bcc FillZerobss
	bl __libc_init_array
	cpsie i
	b main
LoopForever:
	b LoopForever
.size Reset_Handler, .-Reset_Handler
