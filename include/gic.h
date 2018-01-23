/* gic.h : definition for the Generic Interrupt Controller */

#define GIC_BASE 0x01C80000	/* Generic Interrupt Controller Base Address */
#define GIC_DIST_BASE (0x01C80000+0x1000) /* GIC Distributor Base Address */
#define GIC_CPUIF_BASE (0x01C80000 + 0x2000) /* GIC CPU Interface Base Address */

#define GIC_DISABLE 0x00000000
#define GIC_ENABLE 0x00000001

#define GIC_IRQ_MAX	156 /* size of exception vector */
#define GIC_NIRQ (GIC_IRQ_MAX+1)

#define GIC_NSGI 16	/* number of software generated interrupts */
#define GIC_SGI_RESCHED 1

extern uint32 exp_vector[];
extern uint32 irq_vector[GIC_NIRQ];	/* Interrupt vector	*/

struct gic_cpuifreg {	/* GIC CPU Interface Registers */
	reg32 ctrl;			/* CPU Interface Control Register */
	reg32 primask;		/* Interrupt Priority Mask Register */
	reg32 binpoint;		/* Binary Point Register */
	reg32 intack;		/* Interrupt Acknowledge Register */
	reg32 eoi;			/* End of Interrupt Register */
	reg32 runpri;		/* Running Priority Register */
	reg32 hpripnd;		/* Highest Priority Pending Interrupt Register */
	reg32 abinpoint;	/* Aliased Binary Point Register */
	reg32 aintack;		/* Aliased Interrupt Acknowledge Register */
	reg32 aeoi;			/* Aliased End of Interrupt Register */
	reg32 ahpripnd;		/* Aliased Highest Priority Pending Interrupt Register */
	reg32 res1[41];
	reg32 actpri;		/* Active Priority Register */
	reg32 res2[3];
	reg32 nsactpri;		/* Non-Secure Active Priority Register */
	reg32 res3[6];
	reg32 iid;			/* CPU Interface Identification Register */
	reg32 res4[960];
	reg32 deactint;		/* Deactivate Interrupt Register */
};

struct gic_distreg {	/* GIC Distributor Registers */
	reg32 ctrl;			/* Distributor Control Register */
	reg32 type;			/* Interrupt Controller Type Register */
	reg32 iid;			/* Distributor Implementer Identification Register */
	reg32 res1[29];
	reg32 group[16];	/* Interrupt Group Registers */
	reg32 res2[16];		/* reserved */
	reg32 seten[16];	/* Interrupt Set-Enable Registers */
	reg32 res3[16];
	reg32 clren[16];	/* Interrupt Clear-Enable Registers */
	reg32 res4[16];
	reg32 setpnd[16];	/* Interrupt Set-Pending Registers */
	reg32 res5[16];
	reg32 clrpnd[16];	/* Interrupt Clear-Pending Registers */
	reg32 res6[16];
	reg32 setact[16];	/* Interrupt Set-Active Registers */
	reg32 res7[16];
	reg32 clract[16];	/* Interrupt Clear-Active Registers */
	reg32 res8[16];
	reg32 pri[128];		/* Interrupt Priority Registers */
	reg32 res9[128];
	reg32 pctgt[128];	/* Interrupt Processor Targets Registers */
	reg32 res10[128];
	reg32 config[32];	/* Interrupt Configuration Registers */
	reg32 res11[32];
	reg32 status[16];	/* Interrupt Status Registers */
	reg32 res12[112];
	reg32 sgi;			/* Software Generated Interrupt (SGI) Register */
	reg32 res13[3];
	reg32 clrpndsgi[4];	/* SGI Clear-Pending Registers */
	reg32 setpndsgi[4];	/* SGI Set-Pending Registers */
	reg32 res14[40];
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

#define isbadsgi(x) (x < 0 || x >= GIC_NSGI)