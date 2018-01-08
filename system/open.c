/* open.c - open */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  open  -  Open a device (some devices ignore name and mode parameters)
 *------------------------------------------------------------------------
 */
syscall	open(
	  did32		descrp,		/* Descriptor for device	*/
	  char		*name,		/* Name to use, if any		*/
	  char		*mode		/* Mode for device, if any	*/
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
	retval = (*devptr->dvopen) (devptr, name, mode);
	xsec_end(mask, devptr->dvlock);
	return retval;
}
