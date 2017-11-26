/* chprio.c - chprio */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  chprio  -  Change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
pri16	chprio(
	  pid32		pid,		/* ID of process to change	*/
	  pri16		newprio		/* New priority			*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	pri16	oldprio;		/* Priority to return		*/

	if (isbadpid(pid)) {
		return (pri16) SYSERR;
	}
	prptr = &proctab[pid];

	mask = xsec_beg(prptr->prlock);

	if(prptr->prstate == PR_FREE){
		xsec_end(prptr->prlock, mask);
		return SYSERR;
	}

	oldprio = prptr->prprio;
	prptr->prprio = newprio;

	xsec_end(prptr->prlock, mask);
	return oldprio;
}
