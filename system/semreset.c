/* semreset.c - semreset */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  semreset  -  Reset a semaphore's count and release waiting processes
 *------------------------------------------------------------------------
 */
syscall	semreset(
	  sid32		sem,		/* ID of semaphore to reset	*/
	  int32		count		/* New count (must be >= 0)	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	sentry *semptr;	/* Ptr to semaphore table entry */
	qid16	semqueue;		/* Semaphore's process queue ID	*/
	pid32	pid;			/* ID of a waiting process	*/

	if(count < 0 || isbadsem(sem)){
		return SYSERR;
	}
	semptr = &semtab[sem];

	mask = xsec_beg();
	lock(semptr->slock);

	if (semptr->sstate==S_FREE) {
		unlock(semptr->slock);
		xsec_end(mask);
		return SYSERR;
	}

	semqueue = semptr->squeue;	/* Free any waiting processes */

	while ((pid=getfirst(semqueue)) != EMPTY){
		ready(pid);
	}

	semptr->scount = count;		/* Reset count as specified */

	unlock(semptr->slock);
	xsec_end(mask);
	return OK;
}
