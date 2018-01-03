/* signaln.c - signaln */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  signaln  -  Signal a semaphore n times, releasing n waiting processes
 *------------------------------------------------------------------------
 */
syscall	signaln(
	  sid32		sem,		/* ID of semaphore to signal	*/
	  int32		count		/* Number of times to signal	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	sentry	*semptr;	/* Ptr to sempahore table entry */

	if (isbadsem(sem) || (count < 0)) {
		return SYSERR;
	}
	semptr = &semtab[sem];

	mask = xsec_beg();
	lock(semptr->slock);

	if (semptr->sstate == S_FREE) {
		unlock(semptr->slock);
		xsec_end(mask);
		return SYSERR;
	}

	for (; count > 0; count--) {
		if ((semptr->scount++) < 0) {
			ready(dequeue(semptr->squeue));
		}
	}

	unlock(semptr->slock);
	xsec_end(mask);
	return OK;
}
