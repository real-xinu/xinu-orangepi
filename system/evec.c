/* evec.c -- gic_init, set_evec, irq_dispatch */

#include <xinu.h>
#include <stdio.h>

uint32	irq_vector[GIC_NIRQ];	/* Interrupt vector	*/
uint32 	exp_vector[ARMV7A_EV_SIZE];

/*-------------------------------------------------------------------------
 * irq_dispatch - call the handler for specific interrupt
 *-------------------------------------------------------------------------
 */
void	irq_dispatch()
{
	struct gic_cpuifreg* giccpuif = (struct gic_cpuifreg*)GIC_CPUIF_BASE;
	uint32	xnum;		/* Interrupt number of device	*/
	interrupt (*handler)(); /* Pointer to handler function	*/

	/* Get the interrupt number from the Interrupt controller */

	xnum = giccpuif->intack & 0x1FF;

	/* Defer scheduling until interrupt is acknowledged */

	resched_cntl(DEFER_START);

	/* If a handler is set for the interrupt, call it */

	if(irq_vector[xnum]) {
		handler = ( interrupt(*)() )irq_vector[xnum];
		handler(xnum);
	}

	/* Signal end of interrupt */

	giccpuif->eoi |= xnum;

	/* Resume scheduling */

	resched_cntl(DEFER_STOP);
}

/*------------------------------------------------------------------------
 * defexp_handler - Default Exception handler
 *------------------------------------------------------------------------
 */
void defexp_handler(void){
	uint32 lr;	/* link register */

	/* get link register */
	asm volatile ( "mov %0, lr\n" : "=r"(lr));

	kprintf("Unhandled exception. Link Register: 0x%x", lr);
	panic("**** EXCEPTION ****");
}
