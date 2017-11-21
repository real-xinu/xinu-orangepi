/* bufinit.c - bufinit */

#include <xinu.h>

struct	bpentry	buftab[NBPOOLS];		/* Buffer pool table	*/
bpid32	nbpools;
//lid32 buftablock; // TODO/FIXME: putting this here instead of mkbufpool.c causes exception ?!

/*------------------------------------------------------------------------
 *  bufinit  -  Initialize the buffer pool data structure
 *------------------------------------------------------------------------
 */
status	bufinit(void)
{
	nbpools = 0;
	buftablock = newlock();
	return OK;
}
