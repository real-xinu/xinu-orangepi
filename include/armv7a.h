/* armv7a.h */

/* CPSR bits */

#define ARMV7A_CPSR_A	0x00000100	/* Imprecise data abort disable	*/
#define ARMV7A_CPSR_I	0x00000080	/* IRQ interrupts disable	*/
#define ARMV7A_CPSR_F	0x00000040	/* FIQ interrupts disable	*/
#define ARMV7A_CPSR_MM	0x0000001F	/* Processor Mode Mask		*/
#define ARMV7A_CPSR_USR	0x00000010	/* Processor Mode = User	*/
#define ARMV7A_CPSR_FIQ	0x00000011	/* Processor Mode = FIQ		*/
#define ARMV7A_CPSR_IRQ	0x00000012	/* Processor Mode = IRQ		*/
#define ARMV7A_CPSR_SPR	0x00000013	/* Processor Mode = Supervisor	*/
#define ARMV7A_CPSR_ABT	0x00000017	/* Processor Mode = Abort	*/
#define ARMV7A_CPSR_UND	0x0000001B	/* Processor Mode = Undefined	*/
#define ARMV7A_CPSR_SYS	0x0000001F	/* Processor Mode = System	*/
#define ARMv7A_CPSR_SCM	0x00000016	/* Processor Mode Secure Monitor*/

/* System Control Register (SCTLR) bits */

#define ARMV7A_SCTLR_UWXN	0x00100000	/* Unprivilged write permission XN	*/
#define ARMV7A_SCTLR_WXN	0x00080000	/* Write permission XN	*/
#define ARMV7A_SCTLR_TE		0x40000000	/* Thumb Exception Enable	*/
#define ARMV7A_SCTLR_AFE	0x20000000	/* Access Flag Enable	*/
#define ARMV7A_SCTLR_TRE	0x10000000	/* TEX Remap Enable	*/
#define ARMV7A_SCTLR_EE		0x02000000	/* Exception (and TTL) Endianness	*/
#define ARMV7A_SCTLR_Z		0x00008000	/* Branch Prediction Enable	*/
#define ARMV7A_SCTLR_V		0x00002000	/* Exception Base Address Control */
#define ARMV7A_SCTLR_I		0x00001000	/* Instruction Cache enable	*/
#define ARMV7A_SCTLR_C		0x00000004	/* Data Cache enable		*/
#define ARMV7A_SCTLR_A		0x00000002	/* Strict alignment enable	*/
#define ARMV7A_SCTLR_M		0x00000001	/* MMU enable			*/
#define ARMV7A_SCTLR_SW		0x00000400	/* SWP enable			*/

/* Auxiliary Control Register (ACTLR) bits */

#define ARMV7A_ACTLR_SMP	0x00000040	/* SMP enable			*/

/* Multiprocessor Affinity Register (MPIDR) bits */

#define ARMV7A_MPIDR_CID	0x00000003

/* Translation Table Base Register Bits */

#define ARMV7A_TTBR_C		0x00000001	/* table walk is inner cacheable bit */
#define ARMV7A_TTBR_S		0x00000002	/* table walk is shareable bit */
#define ARMV7A_TTBR_NOS		0x00000020	/* not outer shareable bit (if S == 1) */
#define ARMV7A_TTBR_RGN0	0x00000000	/* walk outer non-cacheable */
#define ARMV7A_TTBR_RGN1	0x00000008	/* walk outer write-back write-allocate cacheable */
#define ARMV7A_TTBR_RGN2	0x00000010	/* walk outer write-through cacheable */
#define ARMV7A_TTBR_RGN3	0x00000018	/* walk outer write-back no write-allocate cacheable */
#define ARMV7A_TTBR_IRGN0	0x00000000	/* walk inner non-cacheable */
#define ARMV7A_TTBR_IRGN1	0x00000001	/* walk inner write-back write-allocate cacheable */
#define ARMV7A_TTBR_IRGN2	0x00000040	/* walk inner write-through cacheable */
#define ARMV7A_TTBR_IRGN3	0x00000041	/* walk inner write-back no write-allocate cacheable */

/* Exception Vector Addresses */
#define ARMV7A_EV_START	exp_vector	/* Exception vector start addr	*/
#define ARMV7A_EH_OFFSET 0x20		/* Exception handler offset */
#define ARMV7A_EV_SIZE 0x40			/* Size of exception vector */
#define ARMV7A_RSTH_IND 8			/* reset exception handler index */
#define ARMV7A_UDIH_IND 9			/* undefined instruction exception handler index */
#define ARMV7A_SWIH_IND 10			/* software interrupt exception handler index */
#define ARMV7A_PFAH_IND 11			/* prefetch abort exception handler index */
#define ARMV7A_DABH_IND 12			/* data abort exception handler index */
#define ARMV7A_RSVH_IND 13			/* reserved exception handler index */
#define ARMV7A_IRQH_IND 14			/* IRQ exception handler index */
#define ARMV7A_FIQH_IND 15			/* FIQ exception handler index */
#define ARMV7A_RSTH_OFFSET 0x20		/* reset exception handler offset */
#define ARMV7A_UDIH_OFFSET 0x24		/* undefined instruction exception handler offset */
#define ARMV7A_SWIH_OFFSET 0x28		/* software interrupt exception handler offset */
#define ARMV7A_PFAH_OFFSET 0x2C		/* prefetch abort exception handler offset */
#define ARMV7A_DABH_OFFSET 0x30		/* data abort exception handler offset */
#define ARMV7A_RSVH_OFFSET 0x34		/* reserved exception handler offset */
#define ARMV7A_IRQH_OFFSET 0x38		/* IRQ exception handler offset */
#define ARMV7A_FIQH_OFFSET 0x3C		/* FIQ exception handler offset */

#define MAXADDR	0x60000000	/* 512 MB RAM starting from 0x40000000	*/
