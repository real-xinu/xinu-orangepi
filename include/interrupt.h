/* interrupt.h  -   Macro definitions for interrupts */

#define RESCHED 1

extern uint32 exp_vector[];
extern uint32 irq_vector[GIC_NIRQ];	/* Interrupt vector	*/