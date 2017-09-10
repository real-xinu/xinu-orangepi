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

/* Coprocessor c1 - Control Register bits */

#define ARMV7A_C1CTL_V	0x00002000	/* Exception base addr control	*/
#define ARMV7A_C1CTL_I	0x00001000	/* Instruction Cache enable	*/
#define ARMV7A_C1CTL_C	0x00000004	/* Data Cache enable		*/
#define ARMV7A_C1CTL_A	0x00000002	/* Strict alignment enable	*/
#define ARMV7A_C1CTL_M	0x00000001	/* MMU enable			*/

#define MAXADDR	0x60000000	/* 512 MB RAM starting from 0x40000000	*/

/* Exception Vector Addresses */
#define ARMV7A_EV_START	exp_vector	/* Exception vector start addr	*/
#define ARMV7A_EH_OFFSET 0x20		/* Exception handler offset */
#define ARMV7A_IRQH_OFFSET 0x30		/* IRQ exception offset */
#define ARMV7A_EV_SIZE 0x40			/* Size of exception vector */

