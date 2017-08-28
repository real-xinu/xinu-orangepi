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
//	volatile struct am335x_timer1ms *csrptr =
//			(struct am335x_timer1ms *)0x44E31000;
			/* Set csrptr to address of timer CSR	    */
	struct timer_csreg* csrptr = (struct timer_csreg*)TIMER_BASE;

	/* If there is no interrupt, return */

//	if((csrptr->tisr & AM335X_TIMER1MS_TISR_OVF_IT_FLAG) == 0) {
//		return;
//	}
	if((csrptr->irqstat & 0x1) == 0) {
//		kprintf("no interrupt, returning\n");
		return;
	}
//
//	/* Acknowledge the interrupt */
//
////	csrptr->tisr = AM335X_TIMER1MS_TISR_OVF_IT_FLAG;
//	kprintf("processing interrupt\n");
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
//		kprintf("sleepq not empty\n");
		/* sleepq nonempty, decrement the key of */
		/* topmost process on sleepq		 */

		if((--queuetab[firstid(sleepq)].qkey) == 0) {
//			kprintf("calling wakeup()\n");
			wakeup();
//			kprintf("returned from wakeup()\n");
		}
	}

	/* Decrement the preemption counter */
	/* Reschedule if necessary	    */

	if((--preempt) == 0) {
		preempt = QUANTUM;
		kprintf("resched()\n");
		resched();
//		kprintf("returned from resched()\n");
	}
}
