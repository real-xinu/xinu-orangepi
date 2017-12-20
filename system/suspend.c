/* suspend.c - suspend */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  suspend  -  Suspend a process, placing it in hibernation
 *------------------------------------------------------------------------
 */
syscall suspend(
	pid32 pid /* ID of process to suspend	*/
)
{
	intmask mask;		   /* Saved interrupt mask		*/
	struct procent *prptr; /* Ptr to process's table entry	*/
	pri16 prio;			   /* Priority to return		*/

	if (isbadpid(pid) || isnullpid(pid)){
		return SYSERR;
	}
	prptr = &proctab[pid];

	mask = xsec_beg(readylock); /* locks out kill */

	/* Only suspend a process that is current or ready */
	if ((prptr->prstate != PR_CURR) && (prptr->prstate != PR_READY)){
		xsec_end(readylock, mask);
		return SYSERR;
	}
	
	prio = prptr->prprio;

	if (prptr->prstate == PR_READY){ 
		getitem(pid); /* Remove a ready process	*/
					  /*   from the ready list	*/
		prptr->prstate = PR_SUSP;
	} else {
		if(pid == currpid){ /* process currently on this core */
			prptr->prstate = PR_SUSP; /* Mark the current process	*/
			resched();				  /*   suspended and resched.	*/
		} else { /* process on a different core */
			sendintr(prptr->prcpu, SUSPEND);
		}
	}


	xsec_end(readylock, mask);
	return prio;
}
