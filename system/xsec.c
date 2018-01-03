/* xsec.c	-	xsec_beg, xsec_end */

#include <xinu.h>

// TODO: error checking? return SYSERR?

/*------------------------------------------------------------------------
 *  xsec_beg  -  Begin XINU kernel critical section
 *------------------------------------------------------------------------
 */
intmask xsec_beg(){
	intmask mask;	/* Saved interrupt mask	*/

	/* Disable interrupts to prevent handler deadlocks */
	mask = disable();

	/* Defer rescheduling so the process doesn't yield the processor
	 * while holding a spinlock.
	 */
	resched_cntl(DEFER_START);

	return mask;
}

/*------------------------------------------------------------------------
 *  xsec_end  -  End XINU kernel critical section
 *------------------------------------------------------------------------
 */
status xsec_end(
		intmask mask	/* saved interrupt mask to restore */
		){

	resched_cntl(DEFER_STOP);

	restore(mask);

	return OK;
}
