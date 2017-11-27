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

	mask = xsec_beg(prptr->prlock);

	if (prptr->prstate != PR_SUSP) {
		xsec_end(prptr->prlock, mask);
		return (pri16)SYSERR;
	}

	prio = prptr->prprio;		/* Record priority to return	*/
	ready(pid);

	xsec_end(prptr->prlock, mask);
	return prio;
}
