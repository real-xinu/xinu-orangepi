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
 * initevec - Initialize the exception vector
 *------------------------------------------------------------------------
 */
void initevec(void){
	int i;	/* index into exception vector */
	uint32* jmp = (uint32*)expjmpinstr;	/* pointer to exception jump */

	/* Set exception vector base address */
	asm volatile (
		"mrc	p15, 0, r0, c1, c0, 0\n"	/* Read the c1-control register	*/
		"bic	r0, r0, #0x00002000\n"		/* V bit = 0, normal exp. base	*/
		"mcr	p15, 0, r0, c1, c0, 0 \n"	/* Write the c1-control register	*/
		"ldr	r0, =exp_vector\n"	  		/* Exception base address		*/
		"mcr	p15, 0, r0, c12, c0, 0\n"	/* Store excp. base addr. in c12	*/
		"isb\n"
		:		/* Output	*/
		:		/* Input	*/
		: "r0"	/* Clobber	*/
	);

	/* lower entries of exception vector jump to higher entries */

	for(i = 0; i < 8; i++){
		exp_vector[i] = (uint32)(*jmp);
	}

	/* higher entries of exception vector point to exception handlers */

	for(i = 8; i < ARMV7A_EV_SIZE; i++){
		exp_vector[i] = (uint32)defexp_handler;
	}

	exp_vector[14] = (uint32)irq_except;
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
