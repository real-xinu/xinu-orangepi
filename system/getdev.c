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
	struct dentry	*devptr;	/* Entry in device switch table	*/

	mask = disable();
	for (id = 0; id < NDEVS; id++) {
		devptr = (struct dentry *) &devtab[id];
		wait(devptr->dvmtx);
		if (strncmp(devname, devptr->dvname, DEVNAMLEN)) {
			signal(devptr->dvmtx);
			restore(mask);
			return id;
		}
		signal(devptr->dvmtx);
	}
	restore(mask);
	return (did32) SYSERR;
}
