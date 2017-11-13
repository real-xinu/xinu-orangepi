/* signal.c - signal */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  signal  -  Signal a semaphore, releasing a process if one is waiting
 *------------------------------------------------------------------------
 */
syscall	signal(
	  sid32		sem		/* ID of semaphore to signal	*/
	)
{
	intmask mask;			/* Saved interrupt mask		*/
	struct	sentry *semptr;		/* Ptr to sempahore table entry	*/
	pid32	proc;				/* pid of process at head of wait list */

	if (isbadsem(sem)) {
		return SYSERR;
	}
	semptr= &semtab[sem];

	mask = disable();
	lock(semptr->slock);

	if (semptr->sstate == S_FREE) {
		unlock(semptr->slock);
		restore(mask);
		return SYSERR;
	}

	if ((semptr->scount++) < 0) {	/* Release a waiting process */
		proc = dequeue(semptr->squeue);
		unlock(semptr->slock);
		ready(proc);
	} else {
		unlock(semptr->slock);
	}

	restore(mask);
	return OK;
}
