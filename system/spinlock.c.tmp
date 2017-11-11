/* spinlock.c - lock, unlock, newlock*/

#include <xinu.h>

struct	lentry	locktab[NSLK];	/* Spinlock Table				*/

/*------------------------------------------------------------------------
 *  lock  -  Cause current process to lock a spinlock
 *------------------------------------------------------------------------
 */
status lock(
		lid32	lid		/* id of spinlock to lock */
	){

	if(isbadlid(lid)){
		return SYSERR;
	}

	arm_lock(&(locktab[lid].lock));
	locktab[lid].lowner = currpid;

	return OK;
}

/*------------------------------------------------------------------------
 *  unlock  -  Cause current process to unlock a spinlock
 *------------------------------------------------------------------------
 */
status unlock(
		lid32	lid		/* id of spinlock to unlock */
	){

	if(isbadlid(lid)){
		return SYSERR;
	}

	locktab[lid].lowner = SLK_NONE;
	arm_unlock(&(locktab[lid].lock));

	return OK;
}

/*------------------------------------------------------------------------
 *  newlock  -  Create a new spinlock and return the ID to the caller
 *------------------------------------------------------------------------
 */
lid32	newlock(void)
{
	static lid32	nextlid=0;	/* Next lock in locktab to use */
	lid32	l;					/* id of allocated lock */

	l = nextlid++;		/* Increment index for next call */
	if (l >= NSLK) {	/* Check for table overflow */
		return SYSERR;
	}

	locktab[l].lock = UNLOCKED;
	locktab[l].lowner = SLK_NONE;

	return l;
}
