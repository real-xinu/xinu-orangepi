/* clkhandler.c - clkhandler */

#include <xinu.h>

/*-----------------------------------------------------------------------
 * clkhandler - high level clock interrupt handler
 *-----------------------------------------------------------------------
 */
void	clkhandler()
{
	static uint32 count1000 = 1000;	/* variable to count 1000ms */
	struct timer_csreg* csrptr = (struct timer_csreg*)TIMER_BASE;
	struct cpuent* cpuptr;			/* pointer to cpu entry		*/
	uint32 thiscore;				/* cid of this core			*/

	thiscore = getcid();
	cpuptr = &cputab[thiscore];

	/* Acknowledge the interrupt */

	csrptr->irqstat &= 0x1;

	/* cpu 0 handles global timer state */
	if(thiscore == 0){
		/* Decrement 1000ms counter */
		count1000--;

		/* After 1 sec, increment clktime */
		if(count1000 == 0) {
			clktime++;
			asm volatile("dmb\n"); /* flush clktime update */
			count1000 = 1000;
		}

		/* check if sleep queue is empty */
		lock(sleepqlock); 
		if(!isempty(sleepq)) {
			/* sleepq nonempty, decrement the key of */
			/* topmost process on sleepq		 */

			if((--queuetab[firstid(sleepq)].qkey) == 0) {
				wakeup();
			}
		}
		unlock(sleepqlock);
	}

	/* Decrement the preemption counter */
	/* Reschedule if necessary	    */

	if((--(cpuptr->preempt)) == 0) {
		cpuptr->preempt = QUANTUM;
		resched();
	}
}
