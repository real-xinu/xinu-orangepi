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

	if (isbadsem(sem)) {
		return SYSERR;
	}
	semptr = &semtab[sem];
	
	mask = xsec_beg(semptr->slock);

	if (semptr->sstate == S_FREE) {
		xsec_end(semptr->slock, mask);
		return SYSERR;
	}

	semptr->sstate = S_FREE;

	while (semptr->scount++ < 0) {	/* Free all waiting processes	*/
		ready(getfirst(semptr->squeue));
	}

	xsec_end(semptr->slock, mask);
	return OK;
}
