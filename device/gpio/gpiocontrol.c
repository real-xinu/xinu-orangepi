/* gpiocontrol.c - gpiocontrol */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  gpiocontrol  -  Control a gpio device pin settings
 *------------------------------------------------------------------------
 */
devcall	gpiocontrol(
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  int32	 func,			/* Function to perform		*/
	  int32	 arg1,			/* Argument 1 for request	*/
	  int32	 arg2			/* Argument 2 for request	*/
	)
{
	struct gpio_csreg *csrptr;	/* Pointer ot the CSR for GPIO	*/	struct gpio_intreg *intptr;	/* Pointer ot GPIO Interrupt regs	*/
	struct	gpiocblk  *gpioptr;	/* Pointer to the control block	*/
	uint8 trigger;			/* Interrupt trigger */

	csrptr = (struct gpio_csreg *)(devptr->dvcsr);
	gpioptr = &gpiotab[devptr->dvminor];

	if(csrptr == GPIOA_BASE)	{
		intptr = (struct gpio_intreg *)(GPIOA_INT_BASE);
	}
	if(csrptr == GPIOG_BASE)	{
		intptr = (struct gpio_intreg *)(GPIOG_INT_BASE);
	}

	/* Process the request */

	switch ( func )	{

	/* Enable output capability for pin given by arg1 */

	case GPIO_OUTPUT_MODE:
		csrptr->config[arg1/8] &= ~(0x7<<(4*(arg1%8)));
		csrptr->config[arg1/8] |= 0x1<<(4*(arg1%8));
		return (devcall)OK;

	/* Disable output capability for pin given by arg1 */

	case GPIO_INPUT_MODE:
		csrptr->config[arg1/8] &= ~(0x7<<(4*(arg1%8)));
		return (devcall)OK;

	/* Set the multiplexed mode for pin given by arg1 to mode arg2 */

	case GPIO_SET_MODE:
		csrptr->config[arg1/8] &= ~(0x7<<(4*(arg1%8)));
		csrptr->config[arg1/8] |= arg2<<(4*(arg1%8));
		return (devcall)OK;

	/* Register arg1 function pointer to be called from interrupt */

	case GPIO_REG_INT_HANDLER:
		if((csrptr == GPIOA_BASE)||(csrptr == GPIOG_BASE))	{
			gpioptr->gphookfn = (gpiointhook)arg1;
			return (devcall)OK;
		}
		
#if 0
	/* Configure debounce capability for pins in arg2 */

	case GPIO_DEB_SET_TIME:
		if(arg1 > 0)	{
			csrptr->deb_time = (arg1-1) & 0xFF;
			csrptr->deb_ena |= arg2;
		}
		else	{
			csrptr->deb_ena &= (~arg2);
		}
		return (devcall)OK;
#endif
	/* Configure interrupt capability for pins in arg2 */

	case GPIO_INTERRUPT_CTL:
		
		/* Check if module has interrupt capability */
			
		if(intptr == NULL)	{
			return (devcall)SYSERR;
		}

		/* No lines affected */

		/* Disable lines and triggers */
		
		if((arg1&GPIO_INT_DISABLE) != 0)	{ 

			/* Disable interrupt fr pin */

			intptr->intctl &= (~PIN_MASK(arg2));

			return (devcall)OK;
		}
		
		if((arg1&GPIO_INT_ENABLE) != 0)	{ 
			
			/* Enable interupt */
			
			intptr->intctl |= PIN_MASK(arg2);

			/* Enable triggers */

			if((arg1&GPIO_INT_RISE_TRIG) != 0)	{
				trigger = GPIO_INT_POS_TRIG;
			}
			if((arg1&GPIO_INT_FALL_TRIG) != 0)	{
				trigger = GPIO_INT_NEG_TRIG;
			}
			if((arg1&GPIO_INT_LVL0_TRIG) != 0)	{
				trigger = GPIO_INT_LOW_TRIG ;
			}
			if((arg1&GPIO_INT_LVL1_TRIG) != 0)	{
				trigger = GPIO_INT_HIGH_TRIG;
			}
			if((arg1&(GPIO_INT_RISE_TRIG|GPIO_INT_FALL_TRIG)) != 0)	{
				trigger = GPIO_INT_DOUBLE_TRIG;
			}
			
			/* Write to corresponding bit */
			
			intptr->intconfig[arg2/8] |= trigger<<(4*(arg2%8));

			kprintf("int ctl GPIO A status 0x%x \n", (intptr->intctl));
			kprintf("int config GPIO A status 0x%x \n", intptr->intconfig[arg2/8]);
		}

		return (devcall)OK;

	/* Read input from a single pin arg1 */

	case GPIO_READ_PIN:
		arg2 = csrptr->data & PIN_MASK(arg1);
		return (arg2>0);

	/* Write value arg2 to a single output pin arg1 */

	case GPIO_WRITE_PIN:
		if(arg2){
			csrptr->data |= PIN_MASK(arg1);
		}
		else	{
			csrptr->data &= (~PIN_MASK(arg1));
		}
		return (devcall)OK;

	/* Set the pull settings for pin given by arg1 */

	case GPIO_PULL_SELECT:
		csrptr->pull[arg1/16] &= ~(arg2<<(2*(arg1%16)));				csrptr->pull[arg1/16] |= arg2<<(2*(arg1%16));
		return (devcall)OK;

	default:
		return (devcall)SYSERR;
	}

}
