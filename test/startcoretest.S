#include <armv7a.h>

.globl startcoretest
startcoretest:

	/* Disable interrupts */
	cpsid	i

	/* get stack to run on */

	mrc p15, 0, r11, c0, c0, 5 	/* get cid */
	isb
	and r11, r11, #0x03		   	/* and store in r11 */
	ldr r0, =core_stacks 		/* r0 = core_stacks base */
	mov r1, r11					/* r1 = cid */
	add r1, r1, #1				/* r1 = cid + 1 */
	lsl r2, r1, #12				/* r2 = (cid+1)*4096 */
	add r0, r0, r2				/* use high end of stack */
	mov sp, r0					/* set stack pointer */
	bl bputc

	/* change to supervisor mode */
	cps #ARMV7A_CPSR_SPR
	msr cpsr, #ARMV7A_CPSR_SPR
	isb
	bl bputc

	/* disable mmu and caches */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #ARMV7A_SCTLR_I
	bic	r0, r0, #ARMV7A_SCTLR_C
	bic	r0, r0, #ARMV7A_SCTLR_M
	bic	r0, r0, #ARMV7A_SCTLR_Z
	mcr	p15, 0, r0, c1, c0, 0
	isb
	bl bputc

	/* set actlr bits, including smp bit */
	ldr r0, =0x6040
	mcr p15, 0, r0, c1, c0, 1
	isb
	bl bputc

	/* invalidate caches */
	mov	r0, #0
	mcr	p15, 0, r0, c7, c5, 0
	isb
	bl bputc

	/* set smp bit (must happen before cache enable)*/
	//mrc p15, 0, r0, c1, c0, 1
	//orr r0, r0, #0x40
	//mcr p15, 0, r0, c1, c0, 1
	//bl bputc

	/* invalidate tlb */
	mov	r0, #0
	mcr	p15, 0, r0, c8, c7, 0
	isb
	bl bputc

	/* intialize mmu registers */
	ldr r0, =0x0
	mcr p15, 0, r0, c3, c0, 0	/* TTBCTRL */
	bl bputc
	ldr r0, =0xFFFFFFFF
	mcr p15, 0, r0, c13, c0, 1	/* DACR */
	isb
	bl bputc
	ldr r0, =0x7DFF0059
	mcr p15, 0, r0, c1, c0, 0	/* TTBR0 */
	bl bputc

	mrc	p15, 0, r1, c1, c0, 0
	isb
	ldr	r0, =rpfmt
	bl	kprintf
	bl bputc

	//bl bputc
	/* set system control register bits */
	ldr r0, =0xC5187F
	mcr	p15, 0, r0, c1, c0, 0
	isb
	bl bputc

	bl core_startup

	ldr r1, =0x01F01DA4 @ point back to pcstart
	ldr r2, =0x00C0FFEE @ alive value
	str r2, [r1]		@ store alive value back in pcstart register for probing
	b .					@ forever loop

