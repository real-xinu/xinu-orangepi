/* cache.h */

/* L1 Prefetch Policy */

#define	L1PF_DISABLED	0	/* Prefetch Disabled */
#define	L1PF_1			1	/* 1 outstanding prefetch permitted */
#define	L1PF_2			2	/* 2 outstanding prefetches permitted */
#define	L1PF_3			3	/* 3 outstanding prefetches permitted */

/* Other Memory System Variables */

#define	CACHE_LINESIZE	16	/* Size of cache line in words */

/* On the Orange Pi, the Exclusive Reservation Granule equals the cache line size */
#define	CACHE_ERG	CACHE_LINESIZE

#define UNLOCKED	0	/* Exclusive memory access unlocked state */
#define LOCKED		1	/* Exclusive memory access locked state */
