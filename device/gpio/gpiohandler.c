/* gpiohandler.c - gpiohandler */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  gpiohandler  -  Handle an interrupt for a gpio device
 *------------------------------------------------------------------------
 */
void gpiohandler(
		uint32	xnum	/* IRQ number	*/
	)
{
	struct gpio_intreg *intptr;	/* GPIO CSR INT pointer		*/
	gpiointhook gphookfn;		/* The interrupt function	*/

	/* Clear all interrupts */

	if(xnum == GPIOA_INT)	{
		intptr = (struct gpio_intreg *)(GPIOA_INT_BASE);
		gphookfn = gpiotab[0].gphookfn;
		if(gphookfn != NULL)
			gphookfn(xnum, intptr->intsta);
		intptr->intsta= 0xFFFFFFFF;
		return;
	}
	if(xnum == GPIOG_INT)	{
		intptr = (struct gpio_intreg *)(GPIOG_INT_BASE);
		gphookfn = gpiotab[5].gphookfn;
		if(gphookfn != NULL)
			gphookfn(xnum, intptr->intsta);
		intptr->intsta= 0xFFFFFFFF;
		return;
	}

}
