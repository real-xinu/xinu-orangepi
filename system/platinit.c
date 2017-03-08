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
	kprintf("In platinit()\n");

	struct	uart_csreg *uptr;	/* Address of UART's CSRs	*/
//	struct	watchdog_csreg *wdtptr;	/* Watchdog registers		*/

	/* Disable the watchdog timer */
	// TODO: just have to turn off watchdog timer
//	kprintf("attempting to disable watchdog timer...");
//	wdtptr = (struct watchdog_csreg *)WDTADDR;
//	wdtptr->wspr = 0x0000aaaa;
//	while(wdtptr->wwps & 0x00000010);
//	wdtptr->wspr = 0x00005555;
//	while(wdtptr->wwps & 0x00000010);
//	kprintf(" disabled\n");
	/* Initialize the Interrupt Controller */

//	initintc(); TODO:
//	kprintf("platinit(): returned from initinc()\n");

	/* Initialize the Performance Counters */

//	counterinit(); TODO:
//	kprintf("platinit(): returned from coutnerinit()\n");
	/* Pad control for CONSOLE */

	// setting multiplexed pinout for uart on bbb
	// TODO: is this necessary for orange pi?
//	am335x_padctl(UART0_PADRX_ADDR,
//			AM335X_PADCTL_RXTX | UART0_PADRX_MODE);
//	am335x_padctl(UART0_PADTX_ADDR,
//			AM335X_PADCTL_TX | UART0_PADTX_MODE);

	/* Reset the UART device */

	uptr = (struct uart_csreg *)devtab[CONSOLE].dvcsr;
	uptr->sysc |= UART_SYSC_SOFTRESET;
	while((uptr->syss & UART_SYSS_RESETDONE) == 0);
}
