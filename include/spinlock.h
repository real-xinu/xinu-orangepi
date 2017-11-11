/* spinlock.h: definitions and data structures for spinlocks */

#define NSLK	2*NSEM	/* Number of spinlocks */

#define	SLK_FREE		0		/* Spinlock table entry is available */
#define	SLK_USED		1		/* Spinlock table entry is available */

#define SLK_NONE		(-1)	/* Spinlock currently not owned. */

/* Spinlock table entry: each entry must be equal to the size of the ERG
 * in order to prevent false sharing.
 */
struct lentry {
	int32	lock;		/* Actual spinlock word used by hardware */
	int32	lstate;		/* Whether entry is SLK_FREE or SLK_USED */
	pid32	lowner;		/* id of process that currently owns spinlock */
	int32	lpad[13];	/* padding to reach size of ERG */
};

extern	struct	lentry	slktab[];	/* spinlock table */

#define	isbadlid(l)	((int32)(l) < 0 || (l) >= NSLK)
