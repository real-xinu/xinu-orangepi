/* gpioinit.c - gpioinit */

#include <xinu.h>

struct	gpiocblk	gpiotab[NGPIO];

/*------------------------------------------------------------------------
 *  gpioinit  -  Initialize settings for a gpio device
 *------------------------------------------------------------------------
 */
devcall	gpioinit(
	  struct dentry	*devptr		/* Entry in device switch table	*/
	)
{
	kprintf("Hello from gpioinit()\n");
	struct gpio_csreg *csrptr;	/* Pointer ot GPIO CSRs		*/
	struct gpio_intreg *intptr;	/* Pointer ot GPIO Interrupt regs	*/
	struct gpiocblk	*gpioptr;	/* Pointer ot GPIO Control block*/
	uint8 cri;			/* Config register index */

	csrptr = (struct gpio_csreg *)(devptr->dvcsr);
	kprintf("csrptr = 0x%x\n", csrptr);
	gpioptr = &gpiotab[devptr->dvminor];
	
	/* Check if GPIO has interrupt capability */
	
	if(csrptr == GPIOA_BASE)	{
		intptr = (struct gpio_intreg *)(GPIOA_INT_BASE);
		
		/* Disable all interrupts */

		intptr->intctl = 0x0;
		kprintf("just disabled...\n");
		/* Clear all pending interrupts */

		intptr->intsta= 0x0;
		
		/* Set interrupt handler in GIC */
		set_evec( devptr->dvirq, (uint32)devptr->dvintr );
		kprintf("after set_evec()...\n");
	}
	else if(csrptr == GPIOG_BASE)	{
		intptr = (struct gpio_intreg *)(GPIOG_INT_BASE);
		
		/* Disable all interrupts */

		intptr->intctl = 0x0;
		
		/* Clear all pending interrupts */

		intptr->intsta= 0x0;

		/* ToDo: Set interrupt handler in GIC */
		set_evec( devptr->dvirq, (uint32)devptr->dvintr );
	}
	
	/* Set all pins as IO disabled */
//	for( cri = 0; cri < 4; cri++ )	{
//		kprintf("cri = %d\n", cri);
//		csrptr->config[cri] = 0x77777777;
//	}

	/* Clear all pins */
	csrptr->data = 0x0;
	
	/* Clear the interrupt callback function */
	gpioptr->gphookfn = NULL;

	kprintf("returning\n");
	return OK;
}
