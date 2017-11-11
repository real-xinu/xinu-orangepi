/* spinlock.h: definitions and data structures for spinlocks */

#define NSLK	2*NSEM	/* Number of spinlocks */

#define	SLK_ERG	16		/* Size of Exclusive Reservation Granule in words */

#define SLK_UNLOCKED	0		/* Spinlock Unlocked State */
#define SLK_LOCKED		1		/* Spinlock Locked State */

#define	SLK_FREE		0		/* Spinlock table entry is available */
#define	SLK_USED		1		/* Spinlock table entry is available */

#define SLK_NEMO		(-1)	/* Spinlock currently not owned.
 	 	 	 	 	 	 	 	   Nemo is Latin for "no one"	*/

#define SLK_TABLK	0			/* id of lock on the spinlock table */

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

#define	isbadslk(s)	((int32)(s) < 0 || (s) >= NSLK)
