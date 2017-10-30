/* paging.h - Paging related definitions */

#define	PDE_PTSEC	0x00000002 	/* Page Table Entry is 1 MB Section */
#define	PDE_B		0x00000004  /* Bufferable */
#define	PDE_C		0x00000008	/* Cacheable */
#define	PDE_XN		0x00000010  /* Execute Never */
#define	PDE_DOM		0x000001E0	/* Domain Bits */
#define	PDE_S		0x00010000	/* Shareable */
#define	PDE_NG		0x00020000	/* Not Global */
#define	PDE_NS		0x00080000	/* Non-Secure */
#define	PDE_BASEADDR	0xFFF00000

/* Access Permissions */
#define	PDE_AP		0x00008C00	/* Access Permission Bits */
#define	PDE_AP0		0x00000000
#define	PDE_AP1		0x00000400
#define	PDE_AP2		0x00000800
#define	PDE_AP3		0x00000C00	/* Full Access */
#define	PDE_AP4		0x00008000
#define	PDE_AP5		0x00008400
#define	PDE_AP6		0x00008800
#define	PDE_AP7		0x00008C00

/* Memory Region Attributes */
#define	PDE_TEX		0x00007000
#define	PDE_TEX0	0x00000000
#define	PDE_TEX1	0x00001000
#define	PDE_TEX2	0x00002000
#define	PDE_TEX3	0x00003000
#define	PDE_TEX4	0x00004000
#define	PDE_TEX5	0x00005000
#define	PDE_TEX6	0x00006000
#define	PDE_TEX7	0x00007000

/* page table, defined in start.S */
extern	uint32	page_table[];
