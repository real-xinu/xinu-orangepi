/* clkinit.c - clkinit (BeagleBone Black) */

#include <xinu.h>

uint32	clktime;		/* Seconds since boot			*/
uint32	ctr1000 = 0;		/* Milliseconds since boot		*/
qid16	sleepq;			/* Queue of sleeping processes		*/
uint32	preempt;		/* Preemption counter			*/

/*------------------------------------------------------------------------
 * clkinit  -  Initialize the clock and sleep queue at startup
 *------------------------------------------------------------------------
 */
void	clkinit(void)
{
	/* pointer to timer registers in OrangePi */
	volatile struct timer_csreg* tmrptr = (struct timer_csreg*)TIMER_BASE;

	/* Set interrupt vector for clock to invoke clkint */

	set_evec(TMR0_IRQ, (uint32)clkhandler);

	sleepq = newqueue();	/* Allocate a queue to hold the delta	*/
				/*   list of sleeping processes		*/

	preempt = QUANTUM;	/* Set the preemption time		*/

	clktime = 0;		/* Start counting seconds		*/

	/* Reset the timer module */

	/* The following values are calculated for a	*/
	/*   timer that generates 1ms tick rate		*/
	tmrptr->t0intvl = 0x2EE0;		/* set interval value */
	tmrptr->t0ctl = 0x14;			/* select continuous mode, 24MHz clock source, 2 pre-scale */
	tmrptr->t0ctl |= 0x2;			/* set reload bit */
	while(tmrptr->t0ctl & 0x2){} 	/* wait until reload bit turns to 0 */
	tmrptr->t0ctl |= 0x1;			/* Enable Timer 0 */

	/* Kickstart the timer */
	tmrptr->irqen |= 0x1;			/* Enable Timer 0 Interrupts */

	return;
}
