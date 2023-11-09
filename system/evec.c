/* evec.c -- gic_init, set_evec, irq_dispatch */

#include <xinu.h>
#include <stdio.h>

uint32	irq_vector[GIC_NIRQ];	/* Interrupt vector	*/
uint32 	exp_vector[EV_SIZE];

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
	giccpuif += getcid(); 
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

	evec_set_addr((void*)exp_vector);

	/* lower entries of exception vector jump to higher entries */

	for(i = 0; i < 8; i++){
		exp_vector[i] = (uint32)(*jmp);
	}

	/* higher entries of exception vector point to exception handlers */

	for(i = 8; i < EV_SIZE; i++){
		exp_vector[i] = (uint32)defexp_handler;
	}
	/* overwrite with specific exception handlers as needed */
	exp_vector[RSTH_IND] = (uint32)rst_except;
	exp_vector[UDIH_IND] = (uint32)udi_except;
	exp_vector[SWIH_IND] = (uint32)swi_except;
	exp_vector[PFAH_IND] = (uint32)pfa_except;
	exp_vector[DABH_IND] = (uint32)dab_except;
	exp_vector[IRQH_IND] = (uint32)irq_except;
	exp_vector[FIQH_IND] = (uint32)fiq_except;
}

/*------------------------------------------------------------------------
 * evec_set_addr - set exception vector base address in coprocessor
 *------------------------------------------------------------------------
 */
void evec_set_addr(void* addr){
	asm volatile (
		"mrc	p15, 0, r0, c1, c0, 0\n"	/* Read the c1-control register	*/
		"bic	r0, r0, #0x00002000\n"		/* V bit = 0, normal exp. base	*/
		"mcr	p15, 0, r0, c1, c0, 0 \n"	/* Write the c1-control register	*/
		"mov	r0, %0 \n"	  		/* Exception base address		*/
		"mcr	p15, 0, r0, c12, c0, 0\n"	/* Store excp. base addr. in c12	*/
		"isb\n"
		"dsb\n"
		"dmb\n"
		:				/* Output	*/
		: "r" (addr)	/* Input	*/
		: "r0"			/* Clobber	*/
	);
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

/*------------------------------------------------------------------------
 * rst_handler - reset exception handler
 *------------------------------------------------------------------------
 */
void rst_handler(void){
	kputc('R');
	panic("**** RESET EXCEPTION ****");
	return;
}

/*------------------------------------------------------------------------
 * udi_handler - undefined instruction exception handler
 *------------------------------------------------------------------------
 */
void udi_handler(void){
	kputc('U');
	panic("**** UNDEFINED INSTRUCTION EXCEPTION ****");
	return;
}

/*------------------------------------------------------------------------
 * swi_handler - software interrupt exception handler
 *------------------------------------------------------------------------
 */
void swi_handler(void){
	kputc('S');
	panic("**** SOFTWARE INTERRUPT EXCEPTION ****");
	return;
}

/*------------------------------------------------------------------------
 * pfa_handler - prefetch abort exception handler
 *------------------------------------------------------------------------
 */
void pfa_handler(void){
	kputc('P');
	panic("**** PREFETCH ABORT EXCEPTION ****");
	return;
}

/*------------------------------------------------------------------------
 * dab_handler - data abort exception handler
 *------------------------------------------------------------------------
 */
void dab_handler(void){
	kputc('D');
	panic("**** DATA ABORT EXCEPTION ****");
	return;
}

/*------------------------------------------------------------------------
 * rsv_handler - reserved exception handler
 *------------------------------------------------------------------------
 */
void rsv_handler(void){
	kputc('+');
	return;
}

/*------------------------------------------------------------------------
 * fiq_handler - FIQ exception handler
 *------------------------------------------------------------------------
 */
void fiq_handler(void){
	kputc('F');
	panic("**** FIQ EXCEPTION ****");
	return;
}
