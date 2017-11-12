/* procinit.c - procinit */

#include <xinu.h>

struct	procent	proctab[NPROC];	/* Process table				*/

/*------------------------------------------------------------------------
 *  procinit  - Initialize process variables
 *------------------------------------------------------------------------
 */
status procinit(void){

	uint32 i;					/* iterator over proctab */
	struct	procent	*prptr;		/* Ptr to process table entry	*/

	/* Count the Null processes as the first processes in the system */

	prcount = 4;

	/* Initialize process table entries free */

	for (i = 0; i < NPROC; i++) {
		prptr = &proctab[i];
		prptr->prstate = PR_FREE;
		prptr->prname[0] = NULLCH;
		prptr->prstkbase = NULL;
		prptr->prprio = 0;
	}

	/* Initialize the Null process entries */

	for(i = 0; i < NCORE; i++){
		prptr = &proctab[i];
		prptr->prstate = PR_CURR;
		prptr->prprio = 0;
		strncpy(prptr->prname, "prnullx", 8);
		prptr->prname[6] = i + 0x30; /* convert i to string and append */
		prptr->prstkbase = getstk(NULLSTK);
		prptr->prstklen = NULLSTK;
		prptr->prstkptr = 0;
		prptr->prhrdaff = i;
		prptr->prsftaff = i;
		currpid = NULLPROC;
	}

	return OK;
}
