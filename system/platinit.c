/* platinit.c - platinit */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  counterinit  -  Initialize the ARM performance counters
 *------------------------------------------------------------------------
 */
void		counterinit()
{
	/* Program the performance-counter control-register:		*/
	/*	Enable all counters:					*/
	asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"
							(0x00000011));

	/* Program the count enable set control-register:		*/
	/*	Enable all counters:					*/
	asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"
							(0x80000003));

	/* Program the overflow flag status-register:			*/
	/*	Clear overflows:					*/
	asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"
							(0x80000003));
}

/*------------------------------------------------------------------------
 * platinit - platform specific initialization for Orange Pi
 *------------------------------------------------------------------------
 */
void	platinit(void)
{
	struct	watchdog_csreg *wdtptr;	/* Watchdog registers		*/
	struct 	timer_csreg* tmrptr;	/* Timer registers */

	/* Disable the watchdog timer */

	wdtptr = (struct watchdog_csreg *)WDTADDR;
	wdtptr->ctrl = 0;
	tmrptr = (struct timer_csreg*)TIMER_BASE;
	tmrptr->wd0irqen = 0;

	/* Initialize the Interrupt Controller */

	gicinit();

	/* Initialize the Performance Counters */

	counterinit();

	/* Initialize the CPUs */

	cpuinit();
}
