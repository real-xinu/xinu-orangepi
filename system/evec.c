/* evec.c -- initintc, set_evec, irq_dispatch */

#include <xinu.h>
#include <stdio.h>

/*#define STKTRACE*/
/*#define REGDUMP*/

extern	void	userret(void);

uint32	intc_vector[GIC_NIRQ];	/* Interrupt vector	*/
uint32 	exp_vector[16];
char	expmsg1[] = "Unhandled exception. Link Register: 0x%x";
char	expmsg2[] = "**** EXCEPTION ****";
//reg32   gic_base = 0;

// FIXME: Temporary GIC sanity check:
void gic_sanity(struct gic_distreg* gicdist, struct gic_cpuifreg* giccpuif){
//	asm volatile ("MRC p15, 4, %0, c15, c0, 0\t\n" : "=r" (gic_base));
//	kprintf("gic_base = 0x%08X (0x%08X ?)\n", gic_base, GIC_BASE);
	kprintf("GIC_CPUIF_BASE = %08X\n", GIC_CPUIF_BASE);
	kprintf("sizeof(struct gic_cpuifreg) = %X\n", sizeof(struct gic_cpuifreg));
	kprintf("&giccpuif->ahpripnd - GIC_CPUIF_BASE = 0x%X (0x28 ?)\n", (int32)&giccpuif->ahpripnd - GIC_CPUIF_BASE);
	kprintf("&giccpuif->actpri - GIC_CPUIF_BASE = 0x%X (0xD0 ?)\n", (int32)&giccpuif->actpri - GIC_CPUIF_BASE);
	kprintf("&giccpuif->nsactpir - GIC_CPUIF_BASE = 0x%X (0xE0 ?)\n", (int32)&giccpuif->nsactpri - GIC_CPUIF_BASE);
	kprintf("&giccpuif->iid - GIC_CPUIF_BASE = 0x%X (0xFC ?)\n", (int32)&giccpuif->iid - GIC_CPUIF_BASE);
	kprintf("&giccpuif->deactint - GIC_CPUIF_BASE = 0x%X (0x1000 ?)\n\n", (int32)&giccpuif->deactint - GIC_CPUIF_BASE);

	kprintf("sizeof(struct gic_distreg) = %X\n", sizeof(struct gic_distreg));
	kprintf("&gicdist->group[0] - GIC_DIST_BASE = 0x%X (0x80 ?)\n", (int32)&gicdist->group[0] - GIC_DIST_BASE);
	kprintf("&gicdist->seten[0] - GIC_DIST_BASE = 0x%X (0x100 ?)\n", (int32)&gicdist->seten[0] - GIC_DIST_BASE);
	kprintf("&gicdist->clren[0] - GIC_DIST_BASE = 0x%X (0x180 ?)\n", (int32)&gicdist->clren[0] - GIC_DIST_BASE);
	kprintf("&gicdist->setpnd[0] - GIC_DIST_BASE = 0x%X (0x200 ?)\n", (int32)&gicdist->setpnd[0] - GIC_DIST_BASE);
	kprintf("&gicdist->clrpnd[0] - GIC_DIST_BASE = 0x%X (0x280 ?)\n", (int32)&gicdist->clrpnd[0] - GIC_DIST_BASE);
	kprintf("&gicdist->setact[0] - GIC_DIST_BASE = 0x%X (0x300 ?)\n", (int32)&gicdist->setact[0] - GIC_DIST_BASE);
	kprintf("&gicdist->clract[0] - GIC_DIST_BASE = 0x%X (0x380 ?)\n", (int32)&gicdist->clract[0] - GIC_DIST_BASE);
	kprintf("&gicdist->pri[0] - GIC_DIST_BASE = 0x%X (0x400 ?)\n", (int32)&gicdist->pri[0] - GIC_DIST_BASE);
	kprintf("&gicdist->pctgt[0] - GIC_DIST_BASE = 0x%X (0x800 ?)\n", (int32)&gicdist->pctgt[0] - GIC_DIST_BASE);
	kprintf("&gicdist->config[0] - GIC_DIST_BASE = 0x%X (0xC00 ?)\n", (int32)&gicdist->config[0] - GIC_DIST_BASE);
	kprintf("&gicdist->status[0] - GIC_DIST_BASE = 0x%X (0xD00 ?)\n", (int32)&gicdist->status[0] - GIC_DIST_BASE);
	kprintf("&gicdist->sgi - GIC_DIST_BASE = 0x%X (0xF00 ?)\n", (int32)&gicdist->sgi - GIC_DIST_BASE);
	kprintf("&gicdist->pid4 - GIC_DIST_BASE = 0x%X (0xFD0 ?)\n", (int32)&gicdist->pid4 - GIC_DIST_BASE);
	kprintf("&gicdist->cid[3] - GIC_DIST_BASE = 0x%X (0xFFC ?)\n", (int32)&gicdist->cid[3] - GIC_DIST_BASE);

	kprintf("Distributor Type Register Contents: 0x%08X\n", gicdist->type);

	kprintf("giccpuif->ctl = %X\n", giccpuif->ctrl);
	kprintf("gicdist->ctl = %X\n", giccpuif->ctrl);
}

/*------------------------------------------------------------------------
 * initintc - Initialize the Interrupt Controller
 *------------------------------------------------------------------------
 */
int32	initintc()
{
	// TODO: old stuff from bbb
//	struct	intc_csreg *csrptr = (struct intc_csreg *)0x48200000;
//
//	/* Reset the interrupt controller */
//
//	csrptr->sysconfig |= (INTC_SYSCONFIG_SOFTRESET);
//
//	/* Wait until reset is complete */
//
//	while((csrptr->sysstatus & INTC_SYSSTATUS_RESETDONE) == 0);

//	kprintf("In initintc()\n");
	struct gic_cpuifreg* giccpuif = (struct gic_cpuifreg*)GIC_CPUIF_BASE;
	struct gic_distreg* gicdist = (struct gic_distreg*)GIC_DIST_BASE;
	int i;	/* index into GIC arrays */

	// TODO: temporary sanity check
//	gic_sanity(gicdist, giccpuif);

	/* Reset the interrupt controller */
	// TODO: This just disables for now...?
	giccpuif->ctrl = GIC_DISABLE;
	gicdist->ctrl = GIC_DISABLE;

	/* Initialize Distributor */

	/* set all interrupts to group 0 */
	for(i = 0; i < 16; i++){ gicdist->group[i] = 0; }
	/* clear enable bit for all interrupts */
	for(i = 0; i < 16; i++){ gicdist->clren[i] = 0xFFFFFFFF; }
	/* clear pending bit for all interrupts */
	for(i = 0; i < 16; i++){ gicdist->clrpnd[i] = 0xFFFFFFFF; }
	/* clear active bit for all interrupts */
	for(i = 0; i < 16; i++){ gicdist->clract[i] = 0xFFFFFFFF; }
	/* FIXME: for now, set all interrupt priorities to the same max value */
	for(i = 0; i < 128; i++){ gicdist->pri[i] = 0; }
	/* FIXME: for now, forward all interrupts to cpu interface 0*/
	for(i = 0; i < 128; i++){ gicdist->pctgt[i] = 0x01010101; }
	/* make all interrupts level-sensitive */
	for(i = 0; i < 32; i++){ gicdist->config[i] = 0; }

	/* Initialize CPU Interface */

	/* Set priority filter to accept all priority levels */
	giccpuif->primask = 0xFFFFFFFF;



	return OK;
}

///*------------------------------------------------------------------------
// * initevec - Initialize the exception vector
// *------------------------------------------------------------------------
// */
//void initevec(void) {
//	int i;		/* index into exception vector */
//
//	/* Store excp. base addr. in c12 */
//	asm volatile ("MCR p15, 0, %0, c1, c0, 0\t\n" :: "r" (0));
//	asm volatile ("MCR p15, 0, %0, c1, c0, 0\t\n" :: "r" (exp_vector));
//
//	for(i=0; i < 8; i++){
//		exp_vector[i] = expjmpinstr;
//	}
//	/* install the default exception handler for all exceptions */
//	for(; i < 16; i++){
//		exp_vector[i] = defexp_handler;
//	}
//	/* install the IRQ handler to override the default exception handler */
//	exp_vector[EVEC_IRQ_IND] = irq_except;
//}

/*------------------------------------------------------------------------
 * set_evec - set exception vector to point to an exception handler
 *------------------------------------------------------------------------
 */
int32	set_evec(uint32 xnum, uint32 handler)
{
	// TODO: old stuff
//	struct	intc_csreg *csrptr = (struct intc_csreg *)0x48200000;
//	uint32	bank;	/* bank number in int controller	*/
//	uint32	mask;	/* used to set bits in bank		*/
//
//	/* There are only 127 interrupts allowed 0-126 */
//
//	if(xnum > 127) {
//		return SYSERR;
//	}
//
//	/* Install the handler */
//
//	intc_vector[xnum] = handler;
//
//	/* Get the bank number based on interrupt number */
//
//	bank = (xnum/32);
//
//	/* Get the bit inside the bank */
//
//	mask = (0x00000001 << (xnum%32));
//
//	/* Reset the bit to enable that interrupt number */
//
//	csrptr->banks[bank].mir &= (~mask);

	// TODO: orangepi stuff here: is it right?

	struct gic_distreg* gicdist = (struct gic_distreg*)GIC_DIST_BASE;
	uint32	bank;	/* bank number in int controller	*/
	uint32	mask;	/* used to set bits in bank		*/

	/* There are only 127 interrupts allowed 0-126 */

	if(xnum > GIC_IRQ_MAX) {
		return SYSERR;
	}

	/* Install the handler */

	intc_vector[xnum] = handler;

	/* Get the bank number based on interrupt number */

	bank = (xnum/32);

	/* Get the bit inside the bank */

	mask = (0x00000001 << (xnum%32));

	/* Reset the bit to enable that interrupt number */

	gicdist->seten[bank] |= mask;

	return OK;
}

/*-------------------------------------------------------------------------
 * irq_dispatch - call the handler for specific interrupt
 *-------------------------------------------------------------------------
 */
void	irq_dispatch()
{
//	struct	intc_csreg *csrptr = (struct intc_csreg *)0x48200000;
	struct gic_cpuifreg* giccpuif = (struct gic_cpuifreg*)GIC_CPUIF_BASE;
	uint32	xnum;		/* Interrupt number of device	*/
	interrupt (*handler)(); /* Pointer to handler function	*/

	/* Get the interrupt number from the Interrupt controller */

//	xnum = csrptr->sir_irq & 0x7F;
	xnum = giccpuif->intack & 0x1FF;
//	kprintf("Hello from irq_dispatch(): xnum = %d\n", xnum); while(1);

	/* Defer scheduling until interrupt is acknowledged */

	resched_cntl(DEFER_START);

	/* If a handler is set for the interrupt, call it */

	if(intc_vector[xnum]) {
		handler = ( interrupt(*)() )intc_vector[xnum];
		handler(xnum);
	}

//	/* Acknowledge the interrupt */
//
//	csrptr->control |= (INTC_CONTROL_NEWIRQAGR);
	giccpuif->eoi |= xnum;

	/* Resume scheduling */

	resched_cntl(DEFER_STOP);
}

#if 0
static long *fp;
/*------------------------------------------------------------------------
 * trap -- print some debugging info when a trap occurred
 * Note: Need some more work.
 *------------------------------------------------------------------------
*/
#define REGDUMP

void trap(int inum)
{
	long		*sp;
	intmask 	mask;

	mask = disable();
	kprintf("TRAP\n");
	//asm("movl	%ebp,fp");
	sp = fp + 15;	/* eflags/CS/eip/ebp/regs/trap#/Xtrap/ebp */
	kprintf("Xinu trap!\n");
	if (inum < 16)
		kprintf("exception %d (%s) currpid %d (%s)\n", inum,
			inames[inum], currpid, proctab[currpid].prname);
	else
		kprintf("exception %d currpid %d (%s)\n", inum, currpid,
			proctab[currpid].prname);
#ifdef REGDUMP
	kprintf("eflags %X ", *sp--);
	sp--;	/* skip OLD CS */
	kprintf("eip %X ", *sp);
	sp--;
	if (inum == 8 || (inum >= 10 && inum <= 14)) {
		kprintf("error code %08x (%u)\n", *sp, *sp);
		sp--;
	}
	sp--;	/* skip FP for Xint call */
	kprintf("register dump:\n");
	kprintf("eax %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("ecx %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("edx %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("ebx %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("esp %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("ebp %08X (%u)\n", *sp, *sp);
	fp = sp;
	sp--;
	kprintf("esi %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("edi %08X (%u)\n", *sp, *sp);
	sp--;
#endif // REGDUMP
#ifdef STKTRACE
	stacktrace(currpid);
#endif // STKTRACE

	panic("Trap processing complete...\n");
}
#endif
