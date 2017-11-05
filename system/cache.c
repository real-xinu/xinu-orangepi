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
			: "r0", "r1"	/* Clobber	*/
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
			"bic	r0, #0x00008000\n" /* branch prediciton */

			/* Write the new Control Register */

			"mcr	p15, 0, r0, c1, c0, 0\n"
			"isb\n"

			:		/* Output	*/
			:		/* Input	*/
			: "r0", "r1"	/* Clobber	*/
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
// TODO: refactor: cache_inv_L1(), cache_inv_L2()
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

	for(level = 0; level < 2/*cinfo.ncaches*/; level++) {
		cache_inv(level);
	}
//	kprintf("finished invalidating...\n");

}

/*------------------------------------------------------------------------
 * cache_get_info  -  Get Cache information
 *------------------------------------------------------------------------
 */
//void	cache_get_info (
//		struct	cache_info *cinfo	/* Cache information	*/
//		)
//{
//	uint32	clid;	/* Cache Level ID	*/
//
//	/* Read the Cache Level ID Register */
//
//	asm volatile (
//			"mrc	p15, 1, %0, c0, c0, 1\n"
//			: "=r" (clid)	/* Output	*/
//			:		/* Input	*/
//			:		/* Clobber	*/
//		     );
//
//	/* Extract the levels of unification and coherence */
//
//	cinfo->lou = (clid >> 27) & 0x7;
//	cinfo->loc = (clid >> 24) & 0x7;
//
//	/* Compute the number of cache levels */
//
//	cinfo->ncaches = 0;
//	while((clid & 0x7) != 0) {
//		clid >>= 3;
//		cinfo->ncaches++;
//	}
//
//	kprintf("No. of caches: %d\n", cinfo->ncaches);
//	kprintf("Level of Unification: %d\n", cinfo->lou);
//	kprintf("Level of Coherence: %d\n", cinfo->loc);
//}

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
	uint32	data;			/* data for cache line clear */

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
	kprintf("nways = %d\n", nways);
	kprintf("nsets = %d\n", nsets);

	for(set = 0; set < nsets; set++) {
		for(way = 0; way < nways; way++) {

			/* Generate the data item for	*/
			/* cache invalidation		*/

			data = (level << 1) | (set << 6);
			if(level == 0) {
				data |= (way << 30);
			}
			else {
				data |= (way << 29);
			}


			asm volatile (
					"mcr	p15, 0, %0, c7, c6, 2\n" /* invalidate by set/way */
					"isb\n"
					:		/* Ouptut	*/
					: "r" (data)	/* Input	*/
					  :	/* Clobber	*/
			);
			//kprintf("Invalidated cache level %d, set %d, way %d\n", level, set, way);
			//kprintf("data for cache inv: %08x\n", data);

		}
		if (set > nsets){kprintf("set = %d, nsets = %d, WFT?\n"); break;}
	}

}
