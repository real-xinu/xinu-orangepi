/*  cpu.c */
#include <xinu.h>

/*------------------------------------------------------------------------
 *  cpuinit  -  Initialize CPUs
 *------------------------------------------------------------------------
 */
void cpuinit(void){
	struct cpucfg_csreg* cpucfg = (struct cpucfg_csreg*)CPUCFG_BASE;

	//cpucfg->genctrl = 0x40; /* set snoop interface active */
	//cpucfg->genctrl |= 0x20; /* apply reset to shared L2 mem controller */
	//TODO: set csrs
	//TODO: set secondary entry point
	//TODO: wake up secondary cores (they might go to wfi/wfe)?
}

/*------------------------------------------------------------------------
 *  getcid  -  Return the ID of the currently executing core
 *------------------------------------------------------------------------
 */
cid32 getcid(void){
	cid32 cid;
	/* Read Multiprocessor Affinity Register */
	asm volatile ( "MRC p15, 0, %0, c0, c0, 5\t\n": "=r"(cid) );
	cid &= ARMV7A_MPIDR_CID; /* mask off CPU ID bits */
	return cid;
}

/*------------------------------------------------------------------------
 *  cpu_enable  -  reset and turn on given core
 *------------------------------------------------------------------------
 */
syscall cpu_enable(cid32 cid){
	struct cpucfg_csreg* cpucfg;
	struct cpu_csreg* cpureg;
	uint32 cpumask;

	/* Check for valid core */
	if(isbadcid(cid)){ return SYSERR; }

	/* Get core csrs and mask */
	cpucfg = (struct cpucfg_csreg*)CPUCFG_BASE;
	cpureg = &cpucfg->cpu[cid];
	cpumask = (1 << cid);

	/* assert cpu core reset */
	cpureg->rstctrl &= ~CPU_CORE_RST;

	/* L1RSTDISABLE hold low */
	cpucfg->genctrl &= ~cpumask;

	/* de-assert core reset */
	cpureg->rstctrl |= CPU_CORE_RST;

	cpucfg->genctrl &= ~cpumask;
	asm volatile("sev");
	return OK;
}

/*------------------------------------------------------------------------
 *  cpu_set_entry  -  set cpu entry point
 *------------------------------------------------------------------------
 */
void cpu_set_entry(void* entry){
	struct cpucfg_csreg* cpucfg = (struct cpucfg_csreg*)CPUCFG_BASE;
	cpucfg->pcstart = (uint32)entry;
}

/*------------------------------------------------------------------------
 *  printreg  -  print out name, address, and contents of device register
 *------------------------------------------------------------------------
 */
void printreg(char* name, reg32* addr){
	kprintf("%27s:\t0x%08X\t0x%08X\n", name, addr, *addr);
}

/*------------------------------------------------------------------------
 *  cpu_dump  -  dump contents of cpu csrs
 *------------------------------------------------------------------------
 */
void cpu_dump(void){
	int i;
	static int num = 1;
	struct cpucfg_csreg* cpucfg = (struct cpucfg_csreg*)CPUCFG_BASE;

	kprintf("********************* CPU CSR DUMP %d *********************\n", num++);

	kprintf("%28s\t%8s\t%8s\n", "Register", "Address", "Value");
	kprintf("----------------------------\t");
	kprintf("--------\t");
	kprintf("--------\n");
	printreg(	"rstctlall", 		&cpucfg->rstctlall);
	for(i = 0; i < 4; i++){
		kprintf("\t      CPU %d RstCtrl:\t0x%08X\t0x%08X\n", i, &cpucfg->cpu[i].rstctrl, cpucfg->cpu[i].rstctrl);
		kprintf("\t      CPU %d Control:\t0x%08X\t0x%08X\n", i, &cpucfg->cpu[i].ctrl,cpucfg->cpu[i].ctrl);
		kprintf("\t      CPU %d Status :\t0x%08X\t0x%08X\n", i, &cpucfg->cpu[i].stat,cpucfg->cpu[i].stat);
	}
	printreg(	"sysrst", 						&cpucfg->sysrst		);
	printreg(	"clock gating", 				&cpucfg->clkgating	);
	printreg(	"general control", 				&cpucfg->genctrl	);
	printreg(	"super standby flag", 			&cpucfg->supstanflg	);
	printreg(	"secondary entry (private 0)", 	&cpucfg->pcstart	);
	printreg(	"private 1", 					&cpucfg->private1	);
	printreg(	"counter control", 				&cpucfg->cnt64ctrl	);
	printreg(	"counter low", 					&cpucfg->cnt64low	);
	printreg(	"counter high", 				&cpucfg->cnt64high	);

	kprintf("**********************************************************\n");
}
