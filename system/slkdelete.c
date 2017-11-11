/* slkdelete.c - slkdelete */

#include <xinu.h>

/*------------------------------------------------------------------------
 * slkdelete  -  Delete a spinlock by releasing its table entry
 *------------------------------------------------------------------------
 */
syscall	slkdelete(
	  lid32		slk		/* ID of spinlock to delete	*/
	)
{
	intmask mask;			/* Saved interrupt mask		*/
	struct	lentry *slkptr;		/* Ptr to spinlock table entry	*/

	mask = disable();
	lock(SLK_TABLK);

	if (isbadslk(slk)) {
		unlock(SLK_TABLK);
		restore(mask);
		return SYSERR;
	}

	slkptr = &slktab[slk];
	if (slkptr->lstate == SLK_FREE) {
		unlock(SLK_TABLK);
		restore(mask);
		return SYSERR;
	}
	slkptr->lstate = SLK_FREE;

	unlock(SLK_TABLK);
	restore(mask);
	return OK;
}
