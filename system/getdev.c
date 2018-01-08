/* getdev.c - getdev */

#include <xinu.h>
#include <string.h>

/*------------------------------------------------------------------------
 *  getdev  -  Convert a device name to a device ID
 *------------------------------------------------------------------------
 */
did32	getdev(
	  char		*devname	/* Name of the device		*/
	)
{
	intmask		mask;		/* Saved interrupt mask		*/
	did32		id;		/* Value to return to caller	*/

	for (id = 0; id < NDEVS; id++) {
		mask = xsec_beg(devtab[id].dvlock);
		if (strncmp(devname, devtab[id].dvname, DEVNAMLEN)) {
			xsec_end(mask, devtab[id].dvlock);
			return id;
		}
	}
	xsec_end(mask, devtab[id].dvlock);
	return (did32) SYSERR;
}
