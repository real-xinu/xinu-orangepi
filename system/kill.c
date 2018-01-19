/* kill.c - kill */

#include <xinu.h>

/* functions used for kill locking */
status ksec_beg(pid32);
status ksec_end(pid32);

/*------------------------------------------------------------------------
 *  kill  -  Kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
syscall	kill(
	  pid32		pid		/* ID of process to kill	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;	/* Ptr to process's table entry	*/
	int32	i;				/* Index into descriptors	*/
	int16	prdesc[3];	/* Saved descriptors	*/

	// TODO: add hook to handle common suicide case

	if (isbadpid(pid) || isnullpid(pid)) {
		return SYSERR;
	}
	prptr = &proctab[pid];

	mask = disable();
	ksec_beg(pid); 

	if(prptr->prstate == PR_FREE || prptr->prstate == PR_DEAD){
		ksec_end(pid);
		restore(mask);
		return SYSERR;
	}

	if (--prcount <= 1) {		/* Last user process completes	*/
		xdone();
	}

	send(prptr->prparent, pid);
	for (i=0; i<3; i++) {
		prdesc[i] = prptr->prdesc[i];
	}

	switch (prptr->prstate) {
	case PR_CURR:
		prptr->prstate = PR_DEAD;
		if(pid == currpid){ /* process currently on this core */
			resched();
		} else { 			/* process on a different core */
			sendintr(prptr->prcpu, RESCHED);
		}
		break;

	case PR_SLEEP:
	case PR_RECTIM:
		unsleep(pid);
		prptr->prstate = PR_DEAD;
		break;

	case PR_WAIT: // TODO: flag if waiting, signal sem later...
		semtab[prptr->prsem].scount++;
		/* Fall through */

	case PR_READY:
		getitem(pid);		/* Remove from queue */
		/* Fall through */

	default:
		prptr->prstate = PR_DEAD;
	}
	ksec_end(pid);

	for (i=0; i<3; i++) {
		close(prdesc[i]);
	}

	restore(mask);

	return OK;
}

/*------------------------------------------------------------------------
 *  ksec_beg	-	lock all locks required for the kill critical section
 * 					in the appropriate nested/layered order
 *------------------------------------------------------------------------
 */
status ksec_beg(
		pid32 pid	/* id of the process we are attempting to kill */
		){
	sid32 s;	/* index into semtab */

	/* prevent resched while holding locks */
	resched_cntl(DEFER_START);

	/* lock everywhere a process could be */
	lock(sleepqlock);
	lock(semtablock);
	for(s = 0; s < NSEM; s++){
		lock(semtab[s].slock);
	}
	lock(readylock);
	lock(proctablock);
	lock(proctab[pid].prlock);

	return OK;
}

/*------------------------------------------------------------------------
 *  ksec_end	-	unlock all locks required by the kill critical section
 * 					in the appropriate nested/layered order
 *------------------------------------------------------------------------
 */
status ksec_end(
		pid32 pid	/* id of the process we are attempting to kill */
		){
	sid32 s;	/* index into semtab */

	/* unlock locks used in reverse order of ksec_beg */
	unlock(proctab[pid].prlock);
	unlock(proctablock);
	unlock(readylock);
	for(s = (NSEM-1); s >= 0; s--){
		unlock(semtab[s].slock);
	}
	unlock(semtablock);
	unlock(sleepqlock);

	/* allow resched now that no locks are held */
	resched_cntl(DEFER_STOP);

	return OK;
}