#include <armv7a.h>

.globl startcoretest
startcoretest:

	/* Disable interrupts */
	cpsid	i

	/* set actlr bits, including smp bit */
	/* from TRM - this bit must be enable before any TLB or cache
	*  maintenance or enabling MMU */
	mrc p15, 0, r0, c1, c0, 1
	orr r0, #ARMV7A_ACTLR_SMP
	mcr p15, 0, r0, c1, c0, 1
	isb
	bl bputc

	/* disable mmu and caches */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #ARMV7A_SCTLR_I
	bic	r0, r0, #ARMV7A_SCTLR_C
	bic	r0, r0, #ARMV7A_SCTLR_M
	bic	r0, r0, #ARMV7A_SCTLR_Z
	bic	r0, r0, #ARMV7A_SCTLR_A
	mcr	p15, 0, r0, c1, c0, 0
	isb
	bl bputc

	/* Invalidate L1 I/D */
	mov	r0, #0			@ set up for MCR
	mcr	p15, 0, r0, c8, c7, 0	@ invalidate TLBs
	mcr	p15, 0, r0, c7, c5, 0	@ invalidate icache
	mcr	p15, 0, r0, c7, c5, 6	@ invalidate BP array
	dsb
	isb
	bl bputc

	/* get stack to run on */
	isb
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

	//bl printSCTLR
	//bl printACTLR
	//bl printTTBR0

	/* change to supervisor mode */
	cps #ARMV7A_CPSR_SPR
	msr cpsr, #ARMV7A_CPSR_SPR
	isb
	bl bputc

	/* clear the fcseidr */
	ldr r0, =0x0
	isb
	mcr p15, 0, r0, c13, c0, 0
	isb
	bl bputc

	/* intialize mmu registers */
	ldr r0, =0x0
	isb
	mcr p15, 0, r0, c3, c0, 0	/* TTBCTRL */
	isb
	bl bputc

	ldr r0, =0xFFFFFFFF
	isb
	mcr p15, 0, r0, c13, c0, 1	/* DACR */
	isb
	bl bputc

	// CRASH
	ldr r0, =0x7DFF0059
	isb
	mcr p15, 0, r0, c2, c0, 0	/* TTBR0 */
	mcr p15, 0, r0, c2, c0, 1	/* TTBR1 */
	isb
	bl bputc


	/* set system control register bits */
	ldr r0, =0xC5187F
	isb
	mcr	p15, 0, r0, c1, c0, 0
	isb
	bl bputc

	/* cpu gctrl reg*/
	//ldr r0, =0x01F01D84
	//mov r1, #0
	//str r1, [r0]
	//bl bputc

	bl core_startup

	ldr r1, =0x01F01DA4 @ point back to pcstart
	ldr r2, =0x00C0FFEE @ alive value
	str r2, [r1]		@ store alive value back in pcstart register for probing
	b .					@ forever loop


printSCTLR:
	isb
	mrc	p15, 0, r1, c1, c0, 0
	isb
	bl printr1
	mov pc, lr

printACTLR:
	isb
	mrc	p15, 0, r1, c1, c0, 1
	isb
	bl printr1
	mov pc, lr

printTTBR0:
	isb
	mrc	p15, 0, r1, c2, c0, 1
	isb
	bl printr1
	mov pc, lr

setevecbase:
	/* Set exception vector base address */
	isb
	mrc	p15, 0, r0, c1, c0, 0	/* Read the c1-control register	*/
	bic	r0, r0, #0x00002000		/* V bit = 0, normal exp. base	*/
	mcr	p15, 0, r0, c1, c0, 0 	/* Write the c1-control register	*/
	ldr	r0, =exp_vector  		/* Exception base address		*/
	mcr	p15, 0, r0, c12, c0, 0	/* Store excp. base addr. in c12	*/
	isb
	bl bputc
	mov pc, lr

printr1: /* prints out value of r1 byte by byte */
	ldr r0, =0
	lsr r0, r1, #23
	bl kputc
	ldr r0, =0
	lsr r0, r1, #15
	bl kputc
	ldr r0, =0
	lsr r0, r1, #7
	bl kputc
	ldr r0, =0
	orr r0, r1, #15
	bl kputc
	mov pc, lr
