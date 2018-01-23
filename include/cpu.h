/* cpu.h */

#define NCPU 4	/* number of cores in SMP system */

/* CPU Config Control and Status Registers */

#define CPUCFG_BASE	0x01F01C00	/* Base address */

#define CPU_CORE_RST 0x00000002
#define CPU_RST 0x00000001

/* csr for individual cpu */
struct cpu_csreg {
	reg32 rstctrl;		/* reset control register */
	reg32 ctrl;			/* control register */
	reg32 stat;			/* status register */
	reg32 pad[13];		/* padding */
};

/* csr config bank for all cpus */
struct cpucfg_csreg {
	reg32 rstctlall;			/* reset control register for all cpus */
	reg32 pad1[15];				/* padding */
	struct cpu_csreg cpu[4];	/* individual csr's for each cpu */
	reg32 sysrst;				/* cpu system reset register */
	reg32 clkgating;			/* cpu clock gating register */
	reg32 pad2[15];				/* padding */
	reg32 genctrl;				/* general control register */
	reg32 pad3[6];				/* padding */
	reg32 supstanflg;			/* super standby flag register */
	reg32 pcstart;				/* program counter start register */
	reg32 private1;				/* program counter start register */
	reg32 pad4[53];				/* padding */
	reg32 cnt64ctrl;			/* 64-bit counter control register */
	reg32 cnt64low;				/* 64-bit counter low register */
	reg32 cnt64high;			/* 64-bit counter high register */
};

/* Hold current pid executing on each processor */

struct cpuent {
	pid32 cpid;					/* ID of currently executing process */
	pid32 ppid;					/* ID of previously executing process */
	struct deferent defer;		/* Deferred scheduling for cpu */
	uint32 preempt;				/* count 1000 ms for cpu */ 
	int32 cpidpad[11];			/* Pad to size of ERG to avoid false sharing */
};

extern 	struct	cpuent	cputab[];

#define CPU_NONE	-1
#define CPU_ALL		-2

#define isbadcid(x)	(x < 0 || x >= NCPU)

