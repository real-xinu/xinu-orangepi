/* clkhandler.c - clkhandler */

#include <xinu.h>

/*-----------------------------------------------------------------------
 * clkhandler - high level clock interrupt handler
 *-----------------------------------------------------------------------
 */
void	clkhandler()
{
	kprintf("clkhndlr()\n");

	static uint32 count1000 = 1000;	/* variable to count 1000ms */

	struct timer_csreg* csrptr = (struct timer_csreg*)TIMER_BASE;

	/* If there is no interrupt, return */

	if((csrptr->irqstat & 0x1) == 0) {
		return;
	}

	/* Acknowledge the interrupt */

	csrptr->irqstat &= 0x1;

	/* Decrement 1000ms counter */

	count1000--;

	/* After 1 sec, increment clktime */

	if(count1000 == 0) {
		clktime++;
		count1000 = 1000;
	}

	/* check if sleep queue is empty */

	if(!isempty(sleepq)) {
		/* sleepq nonempty, decrement the key of */
		/* topmost process on sleepq		 */

		if((--queuetab[firstid(sleepq)].qkey) == 0) {
			wakeup();
		}
	}

	/* Decrement the preemption counter */
	/* Reschedule if necessary	    */

	if((--preempt) == 0) {
		preempt = QUANTUM;
		resched();
	}
}
