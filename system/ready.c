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
	intmask mask;	
	uint32 i;	
	cid32 thiscore;

	if (isbadpid(pid)) {
		return SYSERR;
	}
	prptr = &proctab[pid];

	mask = xsec_begn(2, readylock, prptr->prlock);

	if (prptr->prstate == PR_FREE || prptr->prstate == PR_DEAD){
		xsec_endn(mask, 2, readylock, prptr->prlock);
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */

	prptr->prstate = PR_READY;
	insert(pid, readylist, prptr->prprio);
	thiscore = getcid();
	for(i = 0; i < NCPU; i++){ // TODO: change to reschedbroadcast sgi
		if(i == thiscore){
			resched();
		} else {
			sendsgi(GIC_SGI_RESCHED, i);
		}
	}

	

	xsec_endn(mask, 2, readylock, prptr->prlock);
	return OK;
}
