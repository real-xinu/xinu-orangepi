/* slkinit.c - slckinit	*/

#include <xinu.h>

/*------------------------------------------------------------------------
 *  slkinit  -  Initialize the Spinlock Table
 *------------------------------------------------------------------------
 */
void	slkinit(void)
{
	int32 i;					/* iterator over spinlock table entries */
	struct	lentry	*slkptr;	/* Ptr to spinlock table entry	*/

	/* Initialize the spinlock table entries */

	for (i = 0; i < NSLK; i++){
		slkptr = &slktab[i];
		slkptr->lock = SLK_UNLOCKED;
		slkptr->lstate = SLK_FREE;
		slkptr->lowner = SLK_NEMO;
	}

	/* Initialize the table lock */

	slkptr = &slktab[SLK_TABLK];
	slkptr->lock = SLK_UNLOCKED;
	slkptr->lstate = SLK_USED;
	slkptr->lowner = SLK_NEMO;
}
