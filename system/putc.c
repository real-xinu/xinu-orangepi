/* putc.c - putc */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  putc  -  Send one character of data (byte) to a device
 *------------------------------------------------------------------------
 */
syscall	putc(
	  did32		descrp,		/* Descriptor for device	*/
	  char		ch		/* Character to send		*/
	)
{
	intmask		mask;		/* Saved interrupt mask		*/
	struct dentry	*devptr;	/* Entry in device switch table	*/
	int32		retval;		/* Value to return to caller	*/

	mask = disable();
	if (isbaddev(descrp)) {
		restore(mask);
		return SYSERR;
	}
	devptr = (struct dentry *) &devtab[descrp];
	wait(devptr->dvmtx);
	retval = (*devptr->dvputc) (devptr, ch);
	signal(devptr->dvmtx);
	restore(mask);
	return retval;
}
