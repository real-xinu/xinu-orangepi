/* clock.h */

extern	uint32	clktime;	/* current time in secs since boot	*/

extern	qid16	sleepq;		/* queue for sleeping processes		*/
extern	int32	slnonempty;	/* nonzero if sleepq is nonempty	*/
extern	int32	*sltop;		/* ptr to key in first item on sleepq	*/
extern	uint32	preempt;	/* preemption counter			*/

// TODO: this is leftover BBB stuff
struct am335x_timer1ms {
	uint32	tidr;		/* Identification register	*/
	uint32	res1[3];	/* Reserved			*/
	uint32	tiocp_cfg;	/* OCP Interface register	*/
	uint32	tistat;		/* Status register		*/
	uint32	tisr;		/* Interrupt status register	*/
	uint32	tier;		/* Interrupt enable register	*/
	uint32	twer;		/* Wakeup enable register	*/
	uint32	tclr;		/* Optional features		*/
	uint32	tcrr;		/* Internal counter value	*/
	uint32	tldr;		/* Timer load value		*/
	uint32	ttgr;		/* Trigger register		*/
	uint32	twps;		/* Write posting register	*/
	uint32	tmar;		/* Match register		*/
	uint32	tcar1;		/* Capture register 1		*/
	uint32	tsicr;		/* Synchronous interface control*/
	uint32	tcar2;		/* Capture register 2		*/
	uint32	tpir;		/* Positive increment register	*/
	uint32	tnir;		/* Negative increment register	*/
	uint32	tcvr;		/* 1ms control register		*/
	uint32	tocr;		/* Overflow mask register	*/
	uint32	towr;		/* no. of overflows		*/
};

#define AM335X_TIMER1MS_ADDR			0x44E31000
#define AM335X_TIMER1MS_IRQ			67

#define AM335X_TIMER1MS_TIOCP_CFG_SOFTRESET	0x00000002
#define AM335X_TIMER1MS_TISTAT_RESETDONE	0x00000001

#define AM335X_TIMER1MS_TISR_MAT_IT_FLAG	0x00000001
#define AM335X_TIMER1MS_TISR_OVF_IT_FLAG	0x00000002
#define AM335X_TIMER1MS_TISR_TCAR_IT_FLAG	0x00000004

#define AM335X_TIMER1MS_TIER_MAT_IT_ENA		0x00000001
#define AM335X_TIMER1MS_TIER_OVF_IT_ENA		0x00000002
#define AM335X_TIMER1MS_TIER_TCAR_IT_ENA	0x00000004

#define AM335X_TIMER1MS_TCLR_ST			0x00000001
#define AM335X_TIMER1MS_TCLR_AR			0x00000002

#define AM335X_TIMER1MS_CLKCTRL_ADDR		0x44E004C4
#define AM335X_TIMER1MS_CLKCTRL_EN		0x00000002


/*** TODO: Orange Pi stuff down here ***/
struct timer_csreg {
	reg32 irqen;		/* IRQ Enable Register */
	reg32 stat;			/* Status Register */
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
