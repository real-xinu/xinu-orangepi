/* ready.c - ready */

#include <xinu.h>

qid16	readylist;			/* Index of ready list		*/
lid32	readylock;			/* Index of ready list spinlock	*/

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	register struct procent *prptr;

	if (isbadpid(pid) || proctab[pid].prstate == PR_FREE) {
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */

	prptr = &proctab[pid];
	prptr->prstate = PR_READY;

	lock(readylock);
	insert(pid, readylist, prptr->prprio);
	unlock(readylock);

	resched();

	return OK;
}
