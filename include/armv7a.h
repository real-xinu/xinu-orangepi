/* armv7a.h */

/* CPSR bits */

#define CPSR_A		0x00000100	/* Imprecise data abort disable	*/
#define CPSR_I		0x00000080	/* IRQ interrupts disable	*/
#define CPSR_F		0x00000040	/* FIQ interrupts disable	*/
#define CPSR_MM		0x0000001F	/* Processor Mode Mask		*/
#define CPSR_USR	0x00000010	/* Processor Mode = User	*/
#define CPSR_FIQ	0x00000011	/* Processor Mode = FIQ		*/
#define CPSR_IRQ	0x00000012	/* Processor Mode = IRQ		*/
#define CPSR_SPR	0x00000013	/* Processor Mode = Supervisor	*/ // TODO: 0x53? see create()
#define CPSR_ABT	0x00000017	/* Processor Mode = Abort	*/
#define CPSR_UND	0x0000001B	/* Processor Mode = Undefined	*/
#define CPSR_SYS	0x0000001F	/* Processor Mode = System	*/
#define CPSR_SCM	0x00000016	/* Processor Mode Secure Monitor*/

/* System Control Register (SCTLR) bits */

#define SCTLR_UWXN	0x00100000	/* Unprivilged write permission XN	*/
#define SCTLR_WXN	0x00080000	/* Write permission XN	*/
#define SCTLR_TE	0x40000000	/* Thumb Exception Enable	*/
#define SCTLR_AFE	0x20000000	/* Access Flag Enable	*/
#define SCTLR_TRE	0x10000000	/* TEX Remap Enable	*/
#define SCTLR_EE	0x02000000	/* Exception (and TTL) Endianness	*/
#define SCTLR_Z		0x00008000	/* Branch Prediction Enable	*/
#define SCTLR_V		0x00002000	/* Exception Base Address Control */
#define SCTLR_I		0x00001000	/* Instruction Cache enable	*/
#define SCTLR_C		0x00000004	/* Data Cache enable		*/
#define SCTLR_A		0x00000002	/* Strict alignment enable	*/
#define SCTLR_M		0x00000001	/* MMU enable			*/
#define SCTLR_SW	0x00000400	/* SWP enable			*/

/* Auxiliary Control Register (ACTLR) bits */

#define ACTLR_SMP		0x00000040	/* SMP enable	*/
#define ACTLR_L1PCTL0	0x00000000	/* Prefetch Disabled	*/
#define ACTLR_L1PCTL1	0x00002000	/* 1 outstanding prefetch	*/
#define ACTLR_L1PCTL2	0x00004000	/* 2 outstanding prefetch	*/
#define ACTLR_L1PCTL3	0x00006000	/* 3 outstanding prefetch	*/

/* Multiprocessor Affinity Register (MPIDR) bits */

#define MPIDR_CID	0x00000003

/* Secure Configuration Register (SCR) */

#define SCR_NS	0x00000001	/* Non Secure bit */
#define SCR_IRQ	0x00000002	/* IRQ Monitor Mode */
#define SCR_FIQ	0x00000004	/* FIQ Monitor Mode */
#define SCR_EA	0x00000008	/* External Abort Monitor Mode */
#define SCR_FW	0x00000010	/* CPSR.F bit can be modified non-secure */
#define SCR_AW	0x00000020	/* CPSR.A bit can be modified non-secure */
#define SCR_SCD	0x00000080	/* SMC instruction disabled */
#define SCR_HCE	0x00000100	/* HypCall enable */
#define SCR_SIF	0x00000200	/* Secure Instruction Fetch */

/* Translation Table Base Register Bits */

#define TTBR_S		0x00000002	/* table walk is shareable bit */
#define TTBR_NOS	0x00000020	/* not outer shareable bit (if S == 1) */
#define TTBR_RGN0	0x00000000	/* walk outer non-cacheable */
#define TTBR_RGN1	0x00000008	/* walk outer write-back write-allocate cacheable */
#define TTBR_RGN2	0x00000010	/* walk outer write-through cacheable */
#define TTBR_RGN3	0x00000018	/* walk outer write-back no write-allocate cacheable */
#define TTBR_IRGN0	0x00000000	/* walk inner non-cacheable */
#define TTBR_IRGN1	0x00000001	/* walk inner write-back write-allocate cacheable */
#define TTBR_IRGN2	0x00000040	/* walk inner write-through cacheable */
#define TTBR_IRGN3	0x00000041	/* walk inner write-back no write-allocate cacheable */

#define TTBR_VAL		( (uint32)page_table	| \
								TTBR_S 		| \
								TTBR_RGN1 	| \
								TTBR_IRGN1 	  \
							)

/* Exception Vector Addresses */

#define EV_START	exp_vector	/* Exception vector start addr	*/
#define EH_OFFSET	0x20		/* Exception handler offset */
#define EV_SIZE		0x40			/* Size of exception vector */
#define RSTH_IND	8			/* reset exception handler index */
#define UDIH_IND	9			/* undefined instruction exception handler index */
#define SWIH_IND	10			/* software interrupt exception handler index */
#define PFAH_IND	11			/* prefetch abort exception handler index */
#define DABH_IND	12			/* data abort exception handler index */
#define RSVH_IND	13			/* reserved exception handler index */
#define IRQH_IND	14			/* IRQ exception handler index */
#define FIQH_IND	15			/* FIQ exception handler index */
#define RSTH_OFFSET 0x20		/* reset exception handler offset */
#define UDIH_OFFSET 0x24		/* undefined instruction exception handler offset */
#define SWIH_OFFSET 0x28		/* software interrupt exception handler offset */
#define PFAH_OFFSET 0x2C		/* prefetch abort exception handler offset */
#define DABH_OFFSET 0x30		/* data abort exception handler offset */
#define RSVH_OFFSET 0x34		/* reserved exception handler offset */
#define IRQH_OFFSET 0x38		/* IRQ exception handler offset */
#define FIQH_OFFSET 0x3C		/* FIQ exception handler offset */

#define MAXADDR	0x60000000	/* 512 MB RAM starting from 0x40000000	*/
