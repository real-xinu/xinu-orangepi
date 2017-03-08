/* am335x_watchdog.h */ // TODO: Is this right on orangepi?

/* Control and Status registers for watchdog timer */

//struct	watchdog_csreg {
//	volatile uint32 widr;	/* Identification register	*/
//	volatile uint32 res1[3];/* Reserved space		*/
//	volatile uint32	wdsc;	/* System control register	*/
//	volatile uint32	wdst;	/* Status register		*/
//	volatile uint32	wisr;	/* Interrupt status register	*/
//	volatile uint32	wier;	/* Interrupt enable register	*/
//	volatile uint32	res2;	/* Reserved space		*/
//	volatile uint32	wclr;	/* Control register		*/
//	volatile uint32	wcrr;	/* Counter register		*/
//	volatile uint32	wldr;	/* Load register		*/
//	volatile uint32	wtgr;	/* Trigger register		*/
//	volatile uint32	wwps;	/* Write posting bits register	*/
//	volatile uint32	res3[3];/* Reserved space		*/
//	volatile uint32	wdly;	/* Delay configuration register	*/
//	volatile uint32	wspr;	/* Start/stop register		*/
//	volatile uint32	res4[2];/* Reserved space		*/
//	volatile uint32	wirqstatraw;
//				/* Raw interrupt status register*/
//	volatile uint32	wirqstat;/* Interrupt status register	*/
//	volatile uint32	wirqenset;
//				/* Interrupt enable set register*/
//	volatile uint32	wirqenclr;
//				/* Interrupt enable clr register*/
//};

struct watchdog_csreg {
	reg32 stat;		/* status register */
	reg32 pad1[3];	/* unused space */
	reg32 ctrl;		/* control register */
	reg32 rest;		/* restart register */
	reg32 pad2[2];	/* unused space */
	reg32 lwct;		/* low counter register */
	reg32 hict;		/* high counter register */
	reg32 pad3[2];	/* unused spaced */
	reg32 intv;		/* interval value register */
	reg32 lwcp;		/* low counter compare register */
	reg32 hicp;		/* high counter compare register */
	reg32 pad4[22];	/* unused space */
	reg32 nvct;		/* secure storage NV-counter register */
	reg32 pad5[3];	/* unused space */
	reg32 dtct[4];	/* synchronize data counter registers 0-3 */
};


#define	WDTADDR	0x01F01800
