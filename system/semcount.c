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

	mask = disable();
	
	if (isbadsem(semid) || semtab[semid].sstate == S_FREE) {
		unlock(semtablock);
		restore(mask);
		return SYSERR;
	}

	lock(semtab[semid].slock);
	count = semtab[semid].scount;
	unlock(semtab[semid].slock);

	restore(mask);
	return count;
}
