/* resume.c - resume */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  resume  -  Unsuspend a process, making it ready
 *------------------------------------------------------------------------
 */
pri16	resume(
	  pid32		pid		/* ID of process to unsuspend	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	pri16	prio;			/* Priority to return		*/

	if (isbadpid(pid)) {
		return (pri16)SYSERR;
	}
	prptr = &proctab[pid];

	mask = xsec_beg();
	lock(readylock);
	lock(prptr->prlock);

	if (prptr->prstate != PR_SUSP) {
		unlock(prptr->prlock);
		unlock(readylock);
		xsec_end(mask);
		return (pri16)SYSERR;
	}

	prio = prptr->prprio;		/* Record priority to return	*/
	ready(pid);

	unlock(prptr->prlock);
	unlock(readylock);
	xsec_end(mask);
	return prio;
}
