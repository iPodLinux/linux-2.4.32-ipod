/*
 *  linux/arch/h8300/platform/h8s/generic/crt0_rom.S
 *
 *  Yoshinori Sato <ysato@users.sourceforge.jp>
 *
 *  Platform depend startup for uClinux-2.4.x
 *  Target Archtecture:	generic
 *  Memory Layout     :	ROM
 */

#define ASSEMBLY

#include <linux/config.h>
#include <asm/linkage.h>
	
#if !defined(CONFIG_BLKDEV_RESERVE)
#if defined(CONFIG_GDB_DEBUG)
#define RAMEND (__ramend - 0xc000)
#else
#define RAMEND __ramend
#endif
#else
#define RAMEND CONFIG_BLKDEV_RESERVE_ADDRESS
#endif
	
	.global SYMBOL_NAME(_start)
	.global SYMBOL_NAME(_command_line)
	.global SYMBOL_NAME(_platform_gpio_table)
	.global SYMBOL_NAME(_target_name)
	
	.h8300s
	.section .text
	.file	"crt0_rom.S"

#define INTCR 0xffff31
	
	/* CPU Reset entry */
SYMBOL_NAME_LABEL(_start)
	mov.l	#RAMEND,sp
	ldc	#0x80,ccr
	ldc	#0,exr
	bclr	#4,@INTCR:8
	bset	#5,@INTCR:8	/* Interrupt mode 2 */
	
	/* Peripheral Setup */
	
	/* .bss clear */
	mov.l	#__sbss,er5
	mov.l	er5,er6
	inc.l	#1,er6
	mov.l	#__ebss,er4
	sub.l	er5,er4
	sub.w	r0,r0
	mov.b	r0l,@er5
1:	
	eepmov.w
	dec.w	#1,e4
	bpl	1b

	/* copy .data */
#if !defined(CONFIG_GDB_EXEC)
	mov.l	#__begin_data,er5
	mov.l	#__sdata,er6
	mov.l	#__edata,er4
	sub.l	er6,er4		
1:	
	eepmov.w	
	dec.w	#1,e4
	bpl	1b	
#endif

	/* copy kernel commandline */
	mov.l	#COMMAND_START,er5
	mov.l	#SYMBOL_NAME(_command_line),er6
	mov.w	#512,r4
	eepmov.w

	/* uClinux kernel start */
	ldc	#0x90,ccr	/* running kernel */
	mov.l	#SYMBOL_NAME(init_task_union),sp
	mov.l	sp,@SYMBOL_NAME(_current_task)
	add.l	#0x2000,sp
	jsr	@_start_kernel
_exit:

	jmp	_exit

	rts

	/* I/O port assign information */
__platform_gpio_table:	
	mov.l	#gpio_table,er0
	rts

gpio_table:
	;; P1DDR
	.byte	0x00,0x00
	;; P2DDR
	.byte	0x00,0x00
	;; P3DDR
	.byte	0x00,0x00
	;; P4DDR
	.byte	0x00,0x00
	;; P5DDR
	.byte	0x00,0x00
	;; P6DDR
	.byte	0x00,0x00
	;; dummy
	.byte	0x00,0x00
	;; P8DDR
	.byte	0x00,0x00
	;; PADDR
	.byte	0x00,0x00
	;; PBDDR
	.byte	0x00,0x00
	;; PCDDR
	.byte	0x00,0x00
	;; PDDDR
	.byte	0x00,0x00
	;; PEDDR
	.byte	0x00,0x00
	;; PFDDR
	.byte	0x00,0x00
	;; PGDDR
	.byte	0x00,0x00
	;; PHDDR
	.byte	0x00,0x00

__target_name:	
	.asciz	"generic"
	
	.section .bss
__command_line:	
	.space	512

	/* interrupt vector */
	.section .vectors,"ax"
	.long	__start
	.long	__start
vector	=	2
	.rept	128-2
	.long	_interrupt_redirect_table+vector*4
vector	=	vector + 1
	.endr
