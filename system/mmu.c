/* mmu.c - mmu_enable, mmu_disable, mmu_set_ttbr */

#include <xinu.h>


/*------------------------------------------------------------------------
 * mmuinit  -  Initialize the MMU
 *------------------------------------------------------------------------
 */
void mmuinit(void){
//	kprintf("In mmuinit\n");
//	bp_disable();
//	kprintf("After bp_disable()\n");
//
//	bp_inv();
//	kprintf("After bp_inv()\n");
//
//	cache_disable_all();
//	kprintf("After cache_disable_all()\n");
//
//	cache_inv_all();
//	kprintf("After cache_inv_all()\n");

	/* Initialize page tables */

	paging_init();
	kprintf("After paging_init()\n");

	/* Set all Domains to Client */

	mmu_set_dacr(0x55555555);

//	tlb_inv_all(); // TODO: invalidate to PoU/PoC?
//	kprintf("After tlb_inv_all()\n");

//	cache_enable_all();

	/* Enable branch prediction */

//	bp_enable();

	/* Make sure all memory operations are completed */

	asm volatile (
			"dsb\n"
			"dmb\n"
	);
}

/*------------------------------------------------------------------------
 * mmu_enable  -  Enable the MMU
 *------------------------------------------------------------------------
 */
void	mmu_enable (void) {

	asm volatile (
			/* Read the Control Register */

			"mrc	p15, 0, r0, c1, c0, 0\n"

			/* Set the MMU Enable bit */

			"orr	r0, #0x00000001\n"

			/* Write the new Control Register */

			"mcr	p15, 0, r0, c1, c0, 0\n"
			"isb\n"

			:	/* Output	*/
			:	/* Input	*/
			: "r0"	/* Clobber	*/
		     );
}

/*------------------------------------------------------------------------
 * mmu_disable  -  Disable the MMU
 *------------------------------------------------------------------------
 */
void	mmu_disable (void) {
	asm volatile (
			/* Read the Control Register */

			"mrc	p15, 0, r0, c1, c0, 0\n"

			/* Reset the MMU Enable bit */

			"bic	r0, #0x00000001\n"

			/* Write the new Control Register */

			"mcr	p15, 0, r0, c1, c0, 0\n"

			"isb\n"
			"dmb\n"
			"dsb\n"

			:	/* Output	*/
			:	/* Input	*/
			: "r0"	/* Clobber	*/
		     );
}

/*------------------------------------------------------------------------
 * mmu_set_ttbr  -  Set the Translation Table Base Register
 *------------------------------------------------------------------------
 */
void	mmu_set_ttbr (
		void	*ttbaddr	/* Translation Tab. Base Address*/
		)
{
	asm volatile (

			/* We want to use TTBR0 only, 16KB page table */

			"ldr	r0, =0x00000000\n"

			/* Write the value into TTBCR */

			"mcr	p15, 0, r0, c2, c0, 2\n"

			/* Load the base address in r0 */

			"mov	r0, %0\n"

			/* Set table walk attributes */

			"orr	r0, #0x00000002\n" /* shareable cacheable */
			"orr	r0, #0x00000008\n" /* outer write-back write-allocate */
			"orr	r0, #0x00000001\n" /* inner write-back write-allocate */

			/* Write the new TTBR0 */

			"mcr	p15, 0, r0, c2, c0, 0\n"

			/* Write the new TTBR1 (just in case) */

			"mcr	p15, 0, r0, c2, c0, 1\n"

			/* Perform memory synchronization */
			"isb\n"
			"dsb\n"
			"dmb\n"

			:		/* Output	*/
			: "r" (ttbaddr)	/* Input	*/
			: "r0"		/* Clobber	*/
		);
}

/*------------------------------------------------------------------------
 * mmu_set_dacr -  Set the Domain Access Control Register
 *------------------------------------------------------------------------
 */
void	mmu_set_dacr (uint32 dacr)
{
	asm volatile (

			/* Load the dacr setting into r0 */

			"mov	r0, %0\n"

			/* Write the new DACR */

			"mcr	p15, 0, r0, c13, c0, 1\n"

			/* Perform memory synchronization */
			"isb\n"

			:		/* Output	*/
			: "r" (dacr)	/* Input	*/
			: "r0"		/* Clobber	*/
		);
}
