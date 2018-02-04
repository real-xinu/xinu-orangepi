/* start.S - start, bzero */

#include <armv7a.h>
#define UART0LSR 0x01C28014
#define UART0RBR 0x01C28000
#define TX_READY 0x40

/*------------------------------------------------------------------------
 * start  -  Initial entry point for a Xinu image (ARM) for primary core
 *------------------------------------------------------------------------
 */
	.text
	.globl	start			/* Declare entry point global	*/

start:

	/* Disable interrupts */
	cpsid	iaf

	/* Load the stack pointer with end of memory */

	ldr	sp, =MAXADDR

	/* Use bzero (below) to zero out the BSS area */

	ldr	r0, =edata
	ldr	r1, =end
	bl	bzero

	/* Call nulluser to initialize the Xinu system  */
	/*    (Note: the call never returns)		*/

	bl	nulluser

	/* Function to zero memory (r0 is lowest addr; r1 is highest)	*/

bzero:
	mov	r2, #0			/* Round address to multiple	*/
	add	r0, r0, #3		/*   of four by adding 3 and	*/
	and	r0, r0, #0xFFFFFFFC	/*   taking the result module 4	*/
bloop:	cmp	r0, r1			/* Loop until last address	*/
	bhs	bexit			/*   has been reached		*/
	str	r2, [r0]		/* Zero four-byte word of memory*/
	add	r0, r0, #4		/* Move to next word		*/
	b	bloop			/* Continue to iterate		*/
bexit:	mov	pc, lr			/* Return to caller		*/


/*------------------------------------------------------------------------
 * secondary_start  -  entry point for secondary cores
 *------------------------------------------------------------------------
 */
	.text
	.globl	secondary_start			/* Declare entry point global	*/

secondary_start:
	bl bputc

	/* Disable interrupts */
	cpsid	iaf
	bl bputc

	/* Disable thumb exception, AFE, TRE */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, #SCTLR_TE
	bic	r0, #SCTLR_AFE
	bic	r0, #SCTLR_TRE
	mcr	p15, 0, r0, c1, c0, 0
	bl bputc

	/* change to supervisor mode */
	cps #CPSR_SPR
	msr cpsr, #CPSR_SPR
	bl bputc

	/* set actlr bits, including smp bit */
	/* from TRM - this bit must be enable before any TLB or cache
	*  maintenance or enabling MMU */
	mrc p15, 0, r0, c1, c0, 1
	orr r0, #ACTLR_SMP
	mcr p15, 0, r0, c1, c0, 1
	isb
	dsb
	dmb
	bl bputc

	ldr r0, =page_table
	bl mmu_set_ttbr
	bl bputc

	ldr r0, =exp_vector
	bl evec_set_addr
	bl bputc

	bl bp_inv
	bl bputc
	bl tlb_inv_all
	bl bputc
	bl cache_enable_all
	bl bputc

	ldr r0, =0xFFFFFFFF
	bl mmu_set_dacr
	bl bputc

	bl mmu_enable
	bl bputc

	/* get stack to run on */
	mrc p15, 0, r11, c0, c0, 5 	/* r11 = cpu id register */
	and r11, r11, #0x03		   	/* r11 = cid of this core */
	ldr r0, =null_stack 		/* r0 = null_stack[] */
	ldr r1, [r0, +r11]			/* r1 = null_stack[cid]	*/
	mov sp, r0					/* sp = null_stack[cid] */
	bl bputc

	/* clear the fcseidr */
	//ldr r0, =0x0
	//mcr p15, 0, r0, c13, c0, 0
	//isb
	//dsb
	//dmb
	//bl bputc

	bl secondary_run	/* never returns */

/*------------------------------------------------------------------------
 * Page Table
 *------------------------------------------------------------------------
 */
.data
/* Page table base address must be aligned at 16 KB boundary */
	.align	14
	.globl page_table
page_table:
	.space	(0x1000 * 4)

/*------------------------------------------------------------------------
 * bputc	-	simple polled output, prints C to screen for assembly
 				debugging when kputc won't work
 *------------------------------------------------------------------------
 */
.globl bputc
bputc:
	ldr r0, =UART0LSR		/* point to uart line status reg */
	ldr r1, [r0]			/* get value of line status reg */
	and r1, r1, #TX_READY	/* mask tx ready bit bit */
    cmp r1, #TX_READY		/* check for bit set */
	bne bputc				/* keep poling until bit is set */
	ldr r0, =UART0RBR		/* point to TX register */
	mov r1, #67				/* decimal 67 is capitol C */
	str r1, [r0]
	mov pc, lr
