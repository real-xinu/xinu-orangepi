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

/*------------------------------------------------------------------------
 *  newlock  -  Allocate and initialize a lock in the spinlock table
 *------------------------------------------------------------------------
 */
lid32	newlock(void)
{
	/* FIXME: should be nextlid = 0, but that causes prefetch abort */
	static lid32	nextlid = 1;	/* Next lock in locktab to use	*/
	lid32		l;		/* ID of allocated lock 	*/

	l = nextlid++;

	if (l >= NSLK) {		/* Check for table overflow	*/
		return SYSERR;
	}

	/* Initialize lock */

	locktab[l].lock = UNLOCKED;
	locktab[l].lowner = SLK_NONE;

	return l;
}
