/* spinlock.c - lock, unlock, newlock */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  lock  -  Cause current process to lock a spinlock
 *------------------------------------------------------------------------
 */
status lock(
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
