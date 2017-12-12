/* lock.c - lock, unlock, newlock */

#include <xinu.h>

struct	lentry	locktab[NSLK];	/* Lock Table				*/

/*------------------------------------------------------------------------
 *  lock  -  Cause current process to lock a lock
 *------------------------------------------------------------------------
 */
status lock(
		lid32	lid		/* id of lock to lock */
	){
	struct lentry* lockptr;	/* Ptr to lock table entry */
	pid32 cpid;				/* ID of process currently executing
	                           on this core */

	if (isbadlid(lid)) {
		return SYSERR;
	}

	lockptr = &locktab[lid];
	cpid = currpid;

	if(lockptr->lowner == cpid){
		lockptr->lcount++;
		return OK;
	}

	spin_lock(&(lockptr->lock));
	lockptr->lowner = cpid;
	lockptr->lcount++;

	return OK;
}

/*------------------------------------------------------------------------
 *  unlock  -  Cause current process to unlock a lock
 *------------------------------------------------------------------------
 */
status unlock(
		lid32	lid		/* id of lock to unlock */
	){

	struct lentry* lockptr;	/* Ptr to lock table entry */

	if (isbadlid(lid)) {
		return SYSERR;
	}

	lockptr = &locktab[lid];

	if(--lockptr->lcount == 0){
		lockptr->lowner = SLK_NONE;
		spin_unlock(&(lockptr->lock));
	}

	return OK;
}

/*------------------------------------------------------------------------
 *  newlock  -  Allocate and initialize a lock in the lock table
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
	locktab[l].lcount = 0;

	return l;
}
