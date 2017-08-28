/* gpiowrite.c - gpiowrite */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  gpiowrite  -  Write value of gpio pins
 *------------------------------------------------------------------------
 */
devcall	gpiowrite(
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char *value,			/* Pointer to 32-bit buffer	*/
	  int32 pinmask			/* Pins to be written written	*/
	)
{
	struct gpio_csreg *csrptr;	/* Pointer to GPIO CSRs		*/
	uint32 val;			/* Value to be written		*/
	uint32 status;			/* Current status of the pins	*/

	csrptr = (struct gpio_csreg *)(devptr->dvcsr);
	val = * (uint32 *)value;
	status = csrptr->data;
	
	/* Set bits that should become one */

	csrptr->data = ((status & ~pinmask) | (pinmask & val));	

	return OK;
}
