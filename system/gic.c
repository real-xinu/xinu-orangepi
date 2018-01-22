/* gic.c */

#include <xinu.h>

extern uint32 exp_vector[];
extern uint32 irq_vector[GIC_NIRQ];	/* Interrupt vector	*/

/*------------------------------------------------------------------------
 * gicinit - Initialize the Interrupt Controller
 *------------------------------------------------------------------------
 */
int32	gicinit()
{
	struct gic_cpuifreg* giccpuif = (struct gic_cpuifreg*)GIC_CPUIF_BASE;
	struct gic_distreg* gicdist = (struct gic_distreg*)GIC_DIST_BASE;
	int i;	/* index into GIC arrays */

	/* Reset the interrupt controller */
	giccpuif->ctrl = GIC_DISABLE;
	gicdist->ctrl = GIC_DISABLE;

	/* Clear irq_vector */
	for(i = 0; i < GIC_NIRQ; i++){ irq_vector[i] = (uint32)NULL; };

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
	for(i = 0; i < 128; i++){ gicdist->pctgt[i] = 0xFFFFFFFF/*TODO:0x01010101*/; }
	/* make all interrupts level-sensitive */
	for(i = 0; i < 32; i++){ gicdist->config[i] = 0; }

	/* Initialize CPU Interface */

	/* Set priority filter to accept all priority levels */
	giccpuif->primask = 0xFFFFFFFF;

	return OK;
}

/*------------------------------------------------------------------------
 * gic_enable - Enable interrupts from the GIC
 *------------------------------------------------------------------------
 */
void gic_enable(void){
	struct gic_distreg* gicdist = (struct gic_distreg*)GIC_DIST_BASE;
	struct gic_cpuifreg* giccpuif = (struct gic_cpuifreg*)GIC_CPUIF_BASE;
	gicdist->ctrl = GIC_ENABLE;
	giccpuif->ctrl = GIC_ENABLE;
}

/*------------------------------------------------------------------------
 * set_irq_handler - set irq vector to point to an interrupt handler
 *------------------------------------------------------------------------
 */
int32	set_irq_handler(uint32 xnum, uint32 handler)
{
	struct gic_distreg* gicdist = (struct gic_distreg*)GIC_DIST_BASE;
	uint32	bank;	/* bank number in int controller	*/
	uint32	mask;	/* used to set bits in bank		*/

	/* There are only 127 interrupts allowed 0-126 */

	if(xnum > GIC_IRQ_MAX) {
		return SYSERR;
	}

	/* Install the handler */

	irq_vector[xnum] = handler;

	/* Get the bank number based on interrupt number */

	bank = (xnum/32);

	/* Get the bit inside the bank */

	mask = (0x00000001 << (xnum%32));

	/* Reset the bit to enable that interrupt number */

	gicdist->seten[bank] |= mask;

	return OK;
}

/*------------------------------------------------------------------------
 * gic_dump - Dump the contents of the GIC control and status registers
 * 			  for debugging.
 *------------------------------------------------------------------------
 */
void gic_dump(void){
	struct gic_cpuifreg* giccpuif = (struct gic_cpuifreg*)GIC_CPUIF_BASE;
	struct gic_distreg* gicdist = (struct gic_distreg*)GIC_DIST_BASE;
	kprintf("expjmpinstr = 0x%08X\n", expjmpinstr);
	kprintf("defexp_handler = 0x%08X\n", defexp_handler);
	kprintf("irq_except = 0x%08X\n", irq_except);
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

	int i;
	for (i = 0; i < 16; i++){
		kprintf("exp_vector[i] = 0x%08X\n", exp_vector[i]);
	}
}
