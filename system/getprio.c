/* getprio.c - getprio */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getprio  -  Return the scheduling priority of a process
 *------------------------------------------------------------------------
 */
syscall	getprio(
	  pid32		pid		/* Process ID			*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	uint32	prio;			/* Priority to return		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/

	if (isbadpid(pid)) {
		return  SYSERR;
	}
	prptr = &proctab[pid];

	mask = xsec_beg(prptr->prlock);

	if(prptr->prstate == PR_FREE){
		xsec_end(mask, prptr->prlock);
		return SYSERR;
	}

	prio = prptr->prprio;

	xsec_end(mask, prptr->prlock);
	return prio;
}
