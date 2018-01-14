/* lock.h: definitions and data structures for locks */

#define NSLK	(2*(NSEM + NPROC))	/* Number of locks */

#define SLK_NONE		(-1)	/* Spinlock currently not owned. */

/* Spinlock table entry: each entry must be equal to the size of the ERG
 * in order to prevent false sharing.
 */
struct lentry {
	int32	lock;		/* Actual lock word used by hardware */
	cid32	lowner;		/* id of cpu that currently owns lock */
	uint32	lcount;		/* count locks by lowner */
	int32	lpad[13];	/* padding to reach size of ERG */
};

extern	struct	lentry	locktab[];	/* lock table */

#define	isbadlid(l)	((int32)(l) < 0 || (l) >= NSLK)
