/* slkcreate.c - slkcreate, newslk */

#include <xinu.h>

local	lid32	newslk(void);

/*------------------------------------------------------------------------
 *  slkcreate  -  Create a new spinlock and return the ID to the caller
 *------------------------------------------------------------------------
 */
lid32	slkcreate(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	lid32	slk;			/* spinlock ID to return	*/

	mask = disable();
	lock(SLK_TABLK);

	if ((slk=newslk())==SYSERR) {
		unlock(SLK_TABLK);
		restore(mask);
		return SYSERR;
	}
	slktab[slk].lock = SLK_UNLOCKED;
	slktab[slk].lowner = SLK_NEMO;

	unlock(SLK_TABLK);
	restore(mask);
	return slk;
}

/*------------------------------------------------------------------------
 *  newslk  -  Allocate an unused spinlock and return its index
 *------------------------------------------------------------------------
 */
local	lid32	newslk(void)
{
	static	lid32	nextslk = 1;	/* Next spinlock index to try	*/
	lid32	slk;			/* spinlock ID to return	*/
	int32	i;			/* Iterate through # entries	*/

	for (i=0 ; i<NSLK ; i++) {
		slk = nextslk++;
		if (nextslk >= NSLK)
			nextslk = 0;
		if (slktab[slk].lstate == S_FREE) {
			slktab[slk].lstate = S_USED;
			return slk;
		}
	}
	return SYSERR;
}
