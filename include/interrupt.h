/* interrupt.h */

extern	uint16	girmask;	/* mask of interrupts being serviced	*/
				/* a bit of 1 corresponds to an		*/
				/* interrupt that is allowed.		*/

struct intc_bank {
	uint32	itr;
	uint32	mir;
	uint32	mir_clear;
	uint32	mir_set;
	uint32	isr_set;
	uint32	isr_clear;
	uint32	pending_irq;
	uint32	pending_fiq;
};

struct intc_csreg {
	uint32	revision;
	uint32	res1[3];
	uint32	sysconfig;
	uint32	sysstatus;
	uint32	res2[10];
	uint32	sir_irq;
	uint32	sir_fiq;
	uint32	control;
	uint32	protection;
	uint32	idle;
	uint32	res3[3];
	uint32	irq_priority;
	uint32	fiq_priority;
	uint32	threshold;
	uint32	res4[5];
	struct	intc_bank banks[4];
	uint32	ilr[128];
};

#define INTC_SYSCONFIG_SOFTRESET	0x00000002
#define INTC_SYSSTATUS_RESETDONE	0x00000001

#define INTC_SIR_IRQ_MASK	0x0000007F

#define INTC_CONTROL_NEWIRQAGR	0x00000001


/* TODO: Orange Pi stuff below this point */

// TODO: double check this
struct gic_cpuifreg {	/* GIC CPU Interface Registers */
	reg32 ctl;			/* CPU Interface Control Register */
	reg32 pm;			/* Interrupt Priority Mask Register */
	reg32 bp;			/* Binary Point Register */
	reg32 ia;			/* Interrupt Acknowledge Register */
	reg32 eoi;			/* End of Interrupt Register */
	reg32 rpi;			/* Running Priority Register */
	reg32 hppi;			/* Highest Priority Pending Interrupt Register */
	reg32 abp;			/* Aliased Binary Point Register */
	reg32 aia;			/* Aliased Interrupt Acknowledge Register */
	reg32 aeoi;			/* Aliased End of Interrupt Register */
	reg32 ahppi;		/* Aliased Highest Priority Pending Interrupt Register */
	reg32 res1[41];		/* reserved */
	reg32 ap;			/* Active Priority Register */
	reg32 res2[3];		/* reserved */
	reg32 nsap;			/* Non-Secure Active Priority Register */
	reg32 res3[6];		/* reserved */
	reg32 iid;			/* CPU Interface Identification Register */
	reg32 res4[960];	/* reserved */ // TODO: this seems weird... see page 3-10 of GIC TRM
	reg32 di;			/* Deactivate Interrupt Register */
};

// TODO: double check this
struct gic_distreg {	/* GIC Distributor Registers */
	reg32 ctl;			/* Distributor Control Register */
	reg32 type;			/* Interrupt Controller Type Register */
	reg32 iid;			/* Distributor Implementer Identification Register */
	reg32 res1[29];		/* reserved */
	reg32 group[15];	/* Interrupt Group Registers */
	reg32 res2[16];		/* reserved */
	reg32 sgippien;		/* Interrupt Set-Enable Register for SGI and PPI */
	reg32 spien[14];	/* Interrupt Set-Enable Registers for Shared Peripheral Interrupts */
	reg32 res3[10];		/* reserved */
	reg32 isp[15];		/* Interrupt Set-Pending Registers */
	reg32 res4[16];		/* reserved */
	reg32 icp[15];		/* Interrupt Clear-Pending Registers */
	reg32 res5[16];		/* reserved */
	reg32 isa;			/* Interrupt Set-Active Registers */
	reg32 res6[16];		/* reserved */
	reg32 ica[15];		/* Interrupt Clear-Active Registers */
	reg32 res7[16];		/* reserved */
	reg32 ipri[127];	/* Interrupt Priority Registers */
	reg32 res8[128];	/* reserved */
	reg32 sgippitgt[7];	/* Interrupt Processor Targets Registers for SGI and PPI */
	reg32 spitgt[119];	/* Interrupt Processor Targets Registers for for SPI */
	reg32 sgicfg;		/* SGI Configuration Register */
	reg32 ppicfg;		/* PPI Configuration Register */
	reg32 spicfg[29];	/* SPI Configuration Registers */
	reg32 res9[32];		/* reserved */
	reg32 ppistat;		/* PPI Status Register */
	reg32 spistat[14];	/* SPI Status Registers */
	reg32 res10[70];	/* reserved */
	reg32 sgi;			/* Software Generated Interrupt (SGI) Register */
	reg32 res11[3];		/* reserved */
	reg32 sgicp[4];		/* SGI Clear-Pending Registers */
	reg32 sgisp[4];		/* SGI Set-Pending Registers */
	reg32 res12[40];	/* reserved */
	reg32 pid4;			/* Peripheral ID 4 Register */
	reg32 pid5;			/* Peripheral ID 5 Register */
	reg32 pid6;			/* Peripheral ID 6 Register */
	reg32 pid7;			/* Peripheral ID 7 Register */
	reg32 pid0;			/* Peripheral ID 0 Register */
	reg32 pid1;			/* Peripheral ID 1 Register */
	reg32 pid2;			/* Peripheral ID 2 Register */
	reg32 pid3;			/* Peripheral ID 3 Register */
	reg32 cid[4];		/* Component ID Registers */
};

extern reg32 gic_base;

#define GIC_BASE 0x01C80000	/* Generic Interrupt Controller Base Address */
#define GIC_DIST_BASE (0x01C80000+0x1000) /* GIC Distributor Base Address */
#define GIC_CPUIF_BASE (0x01C80000 + 0x2000) /* GIC CPU Interface Base Address */

#define GIC_CTL_RESET 0x00000000
#define GIC_IIDR_RESET	0x0020043B
