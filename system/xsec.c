/* xsec.c	-	xsec_beg, xsec_end */

#include <xinu.h>

// TODO: error checking? return SYSERR?

/*------------------------------------------------------------------------
 *  xsec_beg  -  Begin XINU kernel critical section
 *------------------------------------------------------------------------
 */
intmask xsec_beg(
		lid32 lid	/* id of spinlock to lock */
		){
	intmask mask;	/* Saved interrupt mask	*/

	/* Disable interrupts to prevent handler deadlocks */
	mask = disable();

	/* Defer rescheduling so the process doesn't yield the processor
	 * while holding a spinlock.
	 */
	resched_cntl(DEFER_START);

	/* Lock the spinlock necessary to protect XINU critical section */
	lock(lid);

	return mask;
}

/*------------------------------------------------------------------------
 *  xsec_end  -  End XINU kernel critical section
 *------------------------------------------------------------------------
 */
status xsec_end(
		lid32 lid,		/* id of spinlock to lock */
		intmask mask	/* saved interrupt mask to restore */
		){

	unlock(lid);

	resched_cntl(DEFER_STOP);

	restore(mask);

	return OK;
}
