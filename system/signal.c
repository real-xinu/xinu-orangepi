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

	mask = disable();
	if (isbadsem(sem)) {
		restore(mask);
		return SYSERR;
	}
	semptr= &semtab[sem];
	if (semptr->sstate == S_FREE) {
		restore(mask);
		return SYSERR;
	}
	lock(semptr->slock);
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
