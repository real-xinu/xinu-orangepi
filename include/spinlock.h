/* spinlock.h: definitions and data structures for spinlocks */

#define NSLK	2*NSEM + NPROC	/* Number of spinlocks */

#define	SLK_FREE		0		/* Spinlock table entry is available */
#define	SLK_USED		1		/* Spinlock table entry is available */

#define SLK_NONE		(-1)	/* Spinlock currently not owned. */

/* Spinlock table entry: each entry must be equal to the size of the ERG
 * in order to prevent false sharing.
 */
struct lentry {
	int32	lock;		/* Actual spinlock word used by hardware */
	pid32	lowner;		/* id of process that currently owns spinlock */
	uint32	lcount;		/* count locks by lowner */
	int32	lpad[13];	/* padding to reach size of ERG */
};

extern	struct	lentry	locktab[];	/* spinlock table */

#define	isbadlid(l)	((int32)(l) < 0 || (l) >= NSLK)
