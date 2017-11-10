/* cache.c - cache_enable_all, cache_disable_all, cache_inv_all	 */
/*         - tlb_inv_all, cache_get_info			 */

#include <xinu.h>

/*------------------------------------------------------------------------
 * cache_enable_all  -  Enable caches at all levels
 *------------------------------------------------------------------------
 */
void	cache_enable_all (void) {

	asm volatile (
			/* Read the Control Register */

			"mrc	p15, 0, r0, c1, c0, 0\n"

			/* Set the bits for L1 D and I caches */

			"orr	r0, #0x00001000\n"	/* I cache */
			"orr	r0, #0x00000004\n"	/* D cache */
			"orr	r0, #0x00008000\n"	/* branch prediction */

			/* Write the new Control register */

			"mcr	p15, 0, r0, c1, c0, 0\n"

			:		/* Output	*/
			:		/* Input	*/
			: "r0"	/* Clobber	*/
			);
}

/*------------------------------------------------------------------------
 * cache_disable_all  -  Disable caches at all levels
 *------------------------------------------------------------------------
 */
void	cache_disable_all (void) {

	asm volatile (
			/* Read the Control Register */

			"mrc	p15, 0, r0, c1, c0, 0\n"

			/* Reset the bits for D and I caches */

			"bic	r0, #0x00001000\n" /* I cache */
			"bic	r0, #0x00000004\n" /* D cache */

			/* Write the new Control Register */

			"mcr	p15, 0, r0, c1, c0, 0\n"
			"isb\n"

			:		/* Output	*/
			:		/* Input	*/
			: "r0"	/* Clobber	*/
			);
}

/*------------------------------------------------------------------------
 * tlb_inv_all  -  Invalidate all TLB entries (data and instr.)
 *------------------------------------------------------------------------
 */
void	tlb_inv_all (void) {

	asm volatile (
			"mov	r0, #0\n"
			"mcr	p15, 0, r0, c8, c7, 0\n"

			: 	/* No output 	*/
			: 	/* No input	*/
			: "r0"	/* Clobber 	*/
		     );
}

/*------------------------------------------------------------------------
 * cache_inv_all  -  Invalidate all data and intr. caches
 *------------------------------------------------------------------------
 */
void	cache_inv_all (void) {
	uint32	level;			/* Level of cache	*/

	/* Invalidate all Instruction cache entries */

	asm volatile (
			"mov	r0, #0\n"
			"mcr	p15, 0, r0, c7, c5, 0\n"
			"isb\n"
			:
			:
			: "r0"
		     );

	/* Now invalidate all data cache entries */

	for(level = 0; level < 2; level++) {
		cache_inv(level);
	}

}

/*------------------------------------------------------------------------
 * cache_inv  - Invalidate all lines in given cache
 * 					    0  -  L1 cache
 * 					    1  -  L2 cache
 *------------------------------------------------------------------------
 */
void cache_inv(uint32 level) {
	uint32	nsets;			/* No. of sets		*/
	uint32	nways;			/* No. of ways		*/
	uint32	csid;			/* Cache Size ID	*/
	uint32	set;			/* Cache set iterator	*/
	uint32	way;			/* Cache way iterator	*/
	uint32	rtdata;			/* rtdata for cache line clear */

	if(level < 0 || level > 1){return;}

	/* Read the Cache Size ID register for this cache level */

	asm volatile (
			"mcr	p15, 2, %1, c0, c0, 0\n"
			"mrc	p15, 1, %0, c0, c0, 0\n"
			"isb\n"
			: "=r" (csid)		/* Output	*/
			  : "r" (level << 1)	/* Input	*/
				:			/* Clobber	*/
	);

	/* Get no. of sets and ways at this level */

	nways = ((csid >> 3) & 0x3FF) + 1;
	nsets = ((csid >> 13) & 0x7FFF) + 1;

	for(set = 0; set < nsets; set++) {
		for(way = 0; way < nways; way++) {

			/* Generate the rtdata item for	*/
			/* cache invalidation		*/

			rtdata = (level << 1) | (set << 4);// FIXME, used to be set << 6 ?
			if(level == 0) {
				rtdata |= (way << 30);
			}
			else {
				rtdata |= (way << 29);
			}

//			kprintf("invalidating level %d, set %d, way %d, rtdata 0x%X\n",
//					level, set, way, rtdata);

			asm volatile (
					"mcr	p15, 0, %0, c7, c6, 2\n" /* invalidate by set/way */
					"isb\n"
					:		/* Ouptut	*/
					: "r" (rtdata)	/* Input	*/
					  :	/* Clobber	*/
			);
		}
	}

}

/*------------------------------------------------------------------------
 * bp_inv  -  Invalidate branch prediction array
 *------------------------------------------------------------------------
 */
void	bp_inv (void) {

	asm volatile (
			"mov	r0, #0\n"
			"mcr	p15, 0, r0, c7, c5, 6\n"
			"isb\n"
			:
			:
			: "r0"
		     );

}

/*------------------------------------------------------------------------
 * bp_disable	-  Disable branch prediction
 *------------------------------------------------------------------------
 */
void	bp_disable (void) {

	asm volatile (
			/* Read the Control Register */

			"mrc	p15, 0, r0, c1, c0, 0\n"

			/* Reset the Branch Prediction Enable bit */

			"bic	r0, #0x00008000\n"

			/* Write the new Control Register */

			"mcr	p15, 0, r0, c1, c0, 0\n"
			"isb\n"

			:		/* Output	*/
			:		/* Input	*/
			: "r0"	/* Clobber	*/
			);
}

/*------------------------------------------------------------------------
 * bp_enable	-  Enable branch prediction
 *------------------------------------------------------------------------
 */
void	bp_enable (void) {

	asm volatile (
			/* Read the Control Register */

			"mrc	p15, 0, r0, c1, c0, 0\n"

			/* Reset the Branch Prediction Enable bit */

			"bic	r0, #0x00008000\n"

			/* Write the new Control Register */

			"mcr	p15, 0, r0, c1, c0, 0\n"
			"isb\n"

			:		/* Output	*/
			:		/* Input	*/
			: "r0"	/* Clobber	*/
			);
}

///*------------------------------------------------------------------------
// * cache_set_prefetch  -  set catch prefetch policy
// *------------------------------------------------------------------------
// */
//status	cache_set_prefetch (int32 policy) {
//	uint32 actlr;	/* bits for auxiliary control register */
//
//	switch(policy) {
//	case L1PF_DISABLED:
//		actlr = ACTLR_L1PCTL0;
//		break;
//	case L1PF_1:
//		actlr = ACTLR_L1PCTL1;
//		break;
//	case L1PF_2:
//		actlr = ACTLR_L1PCTL2;
//		break;
//	case L1PF_3:
//		actlr = ACTLR_L1PCTL3;
//		break;
//	default:
//		return SYSERR;
//	}
//
//	asm volatile (
//			/* Read the Auxiliary Control Register */
//
//			"mrc	p15, 0, r0, c1, c0, 1\n"
//
//			/* Set the bits for L1 Prefetch Policy */
//
//			"bic	r0, #0x00006000\n"
//
//			"orr	r0, #%0\n"
//
//			/* Write the new Control register */
//
//			"mcr	p15, 0, r0, c1, c0, 1\n"
//
//			:		/* Output	*/
//			: "r" (actlr)	/* Input	*/
//			: "r0"	/* Clobber	*/
//			);
//
//	return OK;
//}
