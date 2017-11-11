/*  cpu.c */
#include <xinu.h>

extern uint32 exp_vector[];

/*------------------------------------------------------------------------
 *  cpuinit  -  Initialize CPUs
 *------------------------------------------------------------------------
 */
void cpuinit(void){
	uint32 i;

	//TODO: set csrs
	//cpucfg->genctrl = 0x40; /* set snoop interface active */
	//cpucfg->genctrl |= 0x20; /* apply reset to shared L2 mem controller */

	/* set secondary entry point */
	cpu_set_entry(secondary_start);

	/* wake up secondary cores */
	for(i = 1; i < NCORE; i++){
		cpu_enable(i);
	}

}

/*------------------------------------------------------------------------
 *  getcid  -  Return the ID of the currently executing core
 *------------------------------------------------------------------------
 */
cid32 getcid(void){
	cid32 cid;
	/* Read Multiprocessor Affinity Register */
	asm volatile ( "MRC p15, 0, %0, c0, c0, 5\t\n": "=r"(cid) );
	cid &= MPIDR_CID; /* mask off CPU ID bits */
	return cid;
}

/*------------------------------------------------------------------------
 *  cpu_sev  -  release other cpus from wfe state
 *------------------------------------------------------------------------
 */
void cpu_sev(void){
	asm volatile("sev");
}

/*------------------------------------------------------------------------
 *  cpu_wfe  -  wait for event
 *------------------------------------------------------------------------
 */
void cpu_wfe(void){
	asm volatile("wfe");
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

	/* L1RSTDISABLE hold low TODO: need this sill? */
	cpucfg->genctrl &= ~cpumask;

	/* de-assert core reset */
	cpureg->rstctrl |= CPU_CORE_RST;

	// TODO: wait for cpu to be in wfe state?

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
 *  secondary_run  -  get already initialized and running secondary core
 *                    integrated and running Xinu processes
 *------------------------------------------------------------------------
 */
void secondary_run(void){
	//cache_set_prefetch(L1PF_DISABLED);
	kprintf("Hello from core %d\n!", getcid());
	cache_inv(0); /* invalidate L1 data cache */
	bp_inv();
	tlb_inv_all();
	cache_enable_all();
	mmu_enable();
	evec_set_addr((void*)exp_vector);
	mmu_set_dacr(0xFFFFFFFF);
	mmu_set_ttbr(page_table);
//	cache_set_prefetch(L1PF_3);
	cpu_wfe();
	printf("Hello from core %d\n!", getcid());
	enable();
	gic_enable();
	while(1);
	// TODO: set currpid[getcid()]
	// TODO: create null process with hard affinity for self and then resched
//	resume(create((void *)startup, NULLSTK, 0,
//						"prnull<getcid()>", 0, NULL));
	//resched();
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

