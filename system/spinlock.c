/* spinlock.c - lock, unlock, newlock */

#include <xinu.h>

struct	lentry	locktab[NSLK];	/* Spinlock Table				*/

/*------------------------------------------------------------------------
 *  lock  -  Cause current process to lock a spinlock
 *------------------------------------------------------------------------
 */
status lock(
		lid32	lid		/* id of spinlock to lock */
	){
	struct lentry* lockptr;	/* Ptr to spinlock table entry */

	if (isbadlid(lid)) {
		return SYSERR;
	}

	lockptr = &locktab[lid];

	arm_lock(&(lockptr->lock));
	lockptr->lowner = currpid;

	return OK;
}

/*------------------------------------------------------------------------
 *  unlock  -  Cause current process to unlock a spinlock
 *------------------------------------------------------------------------
 */
status unlock(
		lid32	lid		/* id of spinlock to unlock */
	){

	struct lentry* lockptr;	/* Ptr to spinlock table entry */

	if (isbadlid(lid)) {
		return SYSERR;
	}

	lockptr = &locktab[lid];

	lockptr->lowner = SLK_NONE;
	arm_unlock(&(lockptr->lock));

	return OK;
}
