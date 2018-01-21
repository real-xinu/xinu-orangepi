/* ptcreate.c - ptcreate */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptcreate  -  Create a port that allows "count" outstanding messages
 *------------------------------------------------------------------------
 */
syscall	ptcreate(
	  int32		count		/* Size of port			*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	int32	i;			/* Counts all possible ports	*/
	int32	ptnum;			/* Candidate port number to try	*/
	struct	ptentry	*ptptr;		/* Pointer to port table entry	*/

	if (count < 0){
		return SYSERR;
	}

	mask = xsec_beg(pttablock);

	for (i=0 ; i<NPORTS ; i++) {	/* Count all table entries	*/
		ptnum = ptnextid;	/* Get an entry to check	*/
		if (++ptnextid >= NPORTS) {
			ptnextid = 0;	/* Reset for next iteration	*/
		}

		/* Check table entry that corresponds to ID ptnum */

		ptptr= &porttab[ptnum];
		lock(ptptr->ptlock);
		if (ptptr->ptstate == PT_FREE) {
			ptptr->ptstate = PT_ALLOC;
			ptptr->ptssem = semcreate(count);
			ptptr->ptrsem = semcreate(0);
			ptptr->pthead = ptptr->pttail = NULL;
			ptptr->ptseq++;
			ptptr->ptmaxcnt = count;
			unlock(ptptr->ptlock);
			xsec_end(mask, pttablock);
			return ptnum;
		}
		unlock(ptptr->ptlock);
	}
	
	xsec_end(mask, pttablock);
	return SYSERR;
}
