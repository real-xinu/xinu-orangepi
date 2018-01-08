/* init.c - init */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  init  -  Initialize a device and its driver
 *------------------------------------------------------------------------
 */
syscall	init(
	  did32		descrp		/* Descriptor for device	*/
	)
{
	intmask		mask;		/* Saved interrupt mask		*/
	struct dentry	*devptr;	/* Entry in device switch table	*/
	int32		retval;		/* Value to return to caller	*/

	if (isbaddev(descrp)) {
		return SYSERR;
	}
	devptr = (struct dentry *) &devtab[descrp];
	mask = xsec_beg(devptr->dvlock);
	retval = (*devptr->dvinit) (devptr);
	xsec_end(mask, devptr->dvlock);
	return retval;
}
