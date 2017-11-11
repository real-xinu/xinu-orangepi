/* spinlock.c - lock, unlock, newlock */

#include <xinu.h>

struct	lentry	slktab[NSLK];	/* Spinlock Table				*/

/*------------------------------------------------------------------------
 *  lock  -  Cause current process to lock a spinlock
 *------------------------------------------------------------------------
 */
status lock(
		lid32	slk		/* id of spinlock to lock */
	){
	struct lentry* slkptr;	/* Ptr to spinlock table entry */

	if (isbadlid(slk)) {
		return SYSERR;
	}

	slkptr = &slktab[slk];
	if (slkptr->lstate == SLK_FREE) {
		return SYSERR;
	}

	arm_lock(&(slkptr->lock));
	slkptr->lowner = currpid;

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

	if (isbadlid(slk)) {
		return SYSERR;
	}

	slkptr = &slktab[slk];
	if (slkptr->lstate == SLK_FREE) {
		return SYSERR;
	}

	slkptr->lowner = SLK_NONE;
	arm_unlock(&(slkptr->lock));

	return OK;
}
