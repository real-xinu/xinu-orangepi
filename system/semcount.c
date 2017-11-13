/* semcount.c - semcount */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  semcount  -  Return the count of a semaphore (because any integer is
 *		   possible, return of SYSERR may be ambiguous)
 *------------------------------------------------------------------------
 */
syscall semcount(
	  sid32		semid		/* ID of semaphore to use	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	int32	count;			/* Current sempahore count	*/

	if(isbadsem(semid)){
		return SYSERR;
	}

	mask = disable();
	lock(semtab[semid].slock);
	
	if (semtab[semid].sstate == S_FREE) {
		unlock(semtab[semid].slock);
		restore(mask);
		return SYSERR;
	}

	count = semtab[semid].scount;

	unlock(semtab[semid].slock);
	restore(mask);
	return count;
}
