/* semdelete.c - semdelete */

#include <xinu.h>

/*------------------------------------------------------------------------
 * semdelete  -  Delete a semaphore by releasing its table entry
 *------------------------------------------------------------------------
 */
syscall	semdelete(
	  sid32		sem		/* ID of semaphore to delete	*/
	)
{
	intmask mask;			/* Saved interrupt mask		*/
	struct	sentry *semptr;		/* Ptr to semaphore table entry	*/

	mask = disable();
	if (isbadsem(sem)) {
		restore(mask);
		return SYSERR;
	}
	
	semptr = &semtab[sem];
	lock(semtablock);
	if (semptr->sstate == S_FREE) {
		unlock(semtablock);
		restore(mask);
		return SYSERR;
	}
	semptr->sstate = S_FREE;

	lock(semptr->slock);
	resched_cntl(DEFER_START);
	while (semptr->scount++ < 0) {	/* Free all waiting processes	*/
		ready(getfirst(semptr->squeue));
	}
	unlock(semptr->slock);
	unlock(semtablock);
	resched_cntl(DEFER_STOP);
	restore(mask);
	return OK;
}
