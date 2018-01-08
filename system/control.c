/* control.c - control */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  control  -  Control a device or a driver (e.g., set the driver mode)
 *------------------------------------------------------------------------
 */
syscall	control(
	  did32		descrp,		/* Descriptor for device	*/
	  int32		func,		/* Specific control function	*/
	  int32		arg1,		/* Specific argument for func	*/
	  int32		arg2		/* Specific argument for func	*/
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
	retval = (*devptr->dvcntl) (devptr, func, arg1, arg2);
	xsec_end(mask, devptr->dvlock);
	return retval;
}
