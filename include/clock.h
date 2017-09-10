/* clock.h */

extern	uint32	clktime;	/* current time in secs since boot	*/

extern	qid16	sleepq;		/* queue for sleeping processes		*/
extern	int32	slnonempty;	/* nonzero if sleepq is nonempty	*/
extern	int32	*sltop;		/* ptr to key in first item on sleepq	*/
extern	uint32	preempt;	/* preemption counter			*/

struct timer_csreg {
	reg32 irqen;		/* IRQ Enable Register */
	reg32 irqstat;		/* IRQ Status Register */
	reg32 res1[2];		/* Reserved */
	reg32 t0ctl;		/* Timer 0 Control Register */
	reg32 t0intvl;		/* Timer 0 Interval Value Register */
	reg32 t0curvl;		/* Timer 0 Current Value Register */
	reg32 res2;			/* Reserved */
	reg32 t1ctl;		/* Timer 1 Control Register */
	reg32 t1intvl;		/* Timer 1 Interval Value Register */
	reg32 t1curvl;		/* Timer 1 Current Value Register */
	reg32 res3[21];		/* Reserved */
	reg32 avsctl;		/* AVS Control Register */
	reg32 avscnt0;		/* AVS Counter 0 Register */
	reg32 avscnt1;		/* AVS Counter 1 Register */
	reg32 avsdiv;		/* AVS Divisor Register */
	reg32 res4[4];		/* Reserved */
	reg32 wd0irqen;		/* Watchdog 0 IRQ Enable Register */
	reg32 wd0stat;		/* Watchdog 0 Status Register */
	reg32 res5[2];		/* Reserved */
	reg32 wd0ctl;		/* Watchdog 0 Control Register */
	reg32 wd0cfg;		/* Watchdog 0 Configuration Register */
	reg32 wd0mode;		/* Watchdog 0 Mode Register */
};

#define TIMER_BASE	0x01C20C00 /* Timer Base Address */
#define TMR0_CLK_SRC_OSC24M 0x00000002 /* 24M OSC bit for control register*/

#define TMR0_IRQ 50	/* Interrupt number */
