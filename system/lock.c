/* lock.c - lock */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  lock  -  Cause current process to lock a spinlock
 *------------------------------------------------------------------------
 */
syscall lock(
		lid32	slk		/* id of spinlock to lock */
	){
	struct lentry* slkptr;	/* Ptr to spinlock table entry */

	if (isbadslk(slk)) {
		return SYSERR;
	}

	slkptr = &slktab[slk];
	if (slkptr->lstate == SLK_FREE) {
		return SYSERR;
	}

	arm_lock(&(slkptr->lock));
	slkptr->lowner = currpid; // TODO: currpid[getcid()]

	return OK;
}
