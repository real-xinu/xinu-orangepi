#include <armv7a.h>

.globl startcoretest
startcoretest:
	wfe
	/* set actlr bits, including smp bit */
	/* from TRM - this bit must be enable before any TLB or cache
	*  maintenance or enabling MMU */
	mrc p15, 0, r0, c1, c0, 1
	orr r0, #ARMV7A_ACTLR_SMP
	mcr p15, 0, r0, c1, c0, 1
	isb
	nop
	nop
	nop
	bl bputc

	//wfi

	/* change to supervisor mode */
	cps #ARMV7A_CPSR_SPR
	msr cpsr, #ARMV7A_CPSR_SPR
	isb
	bl bputc

	/* Disable interrupts */
	cpsid	i

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

	//bl invalidate_dcache_all
	//mov r0, #0
	//bl cache_inv
	//bl bputc

	/* disable mmu, caches, branch prediciton */
	//mrc	p15, 0, r0, c1, c0, 0
	//isb
	//bic	r0, r0, #ARMV7A_SCTLR_I
	//bic	r0, r0, #ARMV7A_SCTLR_C
	//bic	r0, r0, #ARMV7A_SCTLR_M
	//bic	r0, r0, #ARMV7A_SCTLR_Z
	//mcr	p15, 0, r0, c1, c0, 0
	//isb
	//nop
	//nop
	//nop
	//bl bputc

	/* Invalidate L1 I/D */
	//mov	r0, #0			@ set up for MCR
	//mcr	p15, 0, r0, c8, c7, 0	@ invalidate unified TLB
	//mcr	p15, 0, r0, c8, c6, 0	@ invalidate data TLB
	//mcr	p15, 0, r0, c8, c5, 0	@ invalidate instruction TLB
	//mcr	p15, 0, r0, c7, c5, 0	@ invalidate icache
	//mcr	p15, 0, r0, c7, c5, 6	@ invalidate BP array
	//dsb
	//isb
	//bl bputc

	//bl printSCTLR
	//bl printACTLR
	//bl printTTBR0

	/* clear the fcseidr */
	ldr r0, =0x0
	isb
	mcr p15, 0, r0, c13, c0, 0
	isb
	//bl bputc

	/* intialize mmu registers */
	ldr r0, =0x0
	isb
	mcr p15, 0, r0, c3, c0, 0	/* TTBCTRL */
	isb
	bl bputc

	//ldr r0, =0xFFFFFFFF
	ldr r0, =0x55555555
	isb
	mcr p15, 0, r0, c13, c0, 1	/* DACR */
	isb
	bl bputc

	// CRASH
	//ldr r0, =0x7DFF0059
	dsb
	dmb
	ldr r0, =page_table
	orr r0, #ARMV7A_TTBR_C
	orr r0, #ARMV7A_TTBR_S
	orr r0, #ARMV7A_TTBR_RGN1
	orr r0, #ARMV7A_TTBR_IRGN1
	isb
	mcr p15, 0, r0, c2, c0, 0	/* TTBR0 */
	mcr p15, 0, r0, c2, c0, 1	/* TTBR1 */
	isb
	bl bputc


	/* set system control register bits */
	//dsb
	//dmb
	//isb
	//ldr r0, =0xC5187F
	//isb
	//mcr	p15, 0, r0, c1, c0, 0
	//isb
	//nop
	//nop
	//nop
	//bl bputc
	/* enable mmu, caches, branch prediciton */
	//dsb
	//dmb
	//isb
	//mrc	p15, 0, r0, c1, c0, 0
	//isb
	//orr	r0, r0, #ARMV7A_SCTLR_I
	//orr	r0, r0, #ARMV7A_SCTLR_C
	//orr	r0, r0, #ARMV7A_SCTLR_M
	//orr	r0, r0, #ARMV7A_SCTLR_Z
	//mcr	p15, 0, r0, c1, c0, 0
	//isb
	//nop
	//nop
	//nop
	//bl bputc

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

invalidate_dcache_all:
	dmb					@ ensure ordering with previous memory accesses
	mrc	p15, 1, r0, c0, c0, 1		@ read clidr
	mov	r3, r0, lsr #23			@ move LoC into position
	ands	r3, r3, #7 << 1			@ extract LoC*2 from clidr
	beq	inval_finished			@ if loc is 0, then no need to clean
	mov	r10, #0				@ start clean at cache level 0
inval_levels:
	add	r2, r10, r10, lsr #1		@ work out 3x current cache level
	mov	r1, r0, lsr r2			@ extract cache type bits from clidr
	and	r1, r1, #7			@ mask of the bits for current cache only
	cmp	r1, #2				@ see what cache we have at this level
	blt	inval_skip			@ skip if no cache, or just i-cache
	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	isb					@ isb to sych the new cssr&csidr
	mrc	p15, 1, r1, c0, c0, 0		@ read the new csidr
	and	r2, r1, #7			@ extract the length of the cache lines
	add	r2, r2, #4			@ add 4 (line length offset)
	movw	r4, #0x3ff
	ands	r4, r4, r1, lsr #3		@ find maximum number on the way size
	clz	r5, r4				@ find bit position of way size increment
	movw	r7, #0x7fff
	ands	r7, r7, r1, lsr #13		@ extract max number of the index size
inval_loop1:
	mov	r9, r7				@ create working copy of max index
inval_loop2:
 	orr	r11, r10, r4, lsl r5		@ factor way and cache number into r11
 	orr	r11, r11, r9, lsl r2		@ factor index number into r11
	mcr	p15, 0, r11, c7, c6, 2		@ invalidate by set/way
	subs	r9, r9, #1			@ decrement the index
	bge	inval_loop2
	subs	r4, r4, #1			@ decrement the way
	bge	inval_loop1
inval_skip:
	add	r10, r10, #2			@ increment cache number
	cmp	r3, r10
	bgt	inval_levels
inval_finished:
	mov	r10, #0				@ swith back to cache level 0
	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	dsb	st
	isb
	bx	lr
