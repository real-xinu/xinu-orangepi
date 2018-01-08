/* close.c - close */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  close  -  Close a device
 *------------------------------------------------------------------------
 */
syscall	close(
	  did32		descrp		/* Descriptor for device	*/
	)
{
	intmask		mask;		/* Saved interrupt mask		*/
	struct	dentry	*devptr;	/* Entry in device switch table	*/
	int32		retval;		/* Value to return to caller	*/

	if (isbaddev(descrp)) {
		return SYSERR;
	}
	devptr = (struct dentry *) &devtab[descrp];
	mask = xsec_beg(devptr->dvlock);
	retval = (*devptr->dvclose) (devptr);
	xsec_end(mask, devptr->dvlock);
	return retval;
}
