/* unlock.c - unlock */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  unlock  -  Cause current process to unlock a spinlock
 *------------------------------------------------------------------------
 */
status unlock(
		lid32	slk		/* id of spinlock to unlock */
	){

	struct lentry* slkptr;	/* Ptr to spinlock table entry */

	if (isbadslk(slk)) {
		return SYSERR;
	}

	slkptr = &slktab[slk];
	if (slkptr->lstate == SLK_FREE) {
		return SYSERR;
	}

	slkptr->lowner = SLK_NEMO;
	arm_unlock(&(slkptr->lock));

	return OK;
}
