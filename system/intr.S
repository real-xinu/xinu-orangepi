/* intr.S - enable, disable, restore, halt, pause, irq_except (ARM) */

#include <armv7a.h>

	.text
	.globl	disable
	.globl	restore
	.globl	enable
	.globl	pause
	.globl	halt
	.globl	irq_except
	.globl	irq_dispatch
	.globl	initevec
	.globl	expjmpinstr
	.globl  defexp_handler

/*------------------------------------------------------------------------
 * disable  -  Disable interrupts and return the previous state
 *------------------------------------------------------------------------
 */
disable:
	mrs	r0, cpsr	/* Copy the CPSR into r0		*/
	cpsid	i		/* Disable interrupts			*/
	mov	pc, lr		/* Return the CPSR			*/

/*------------------------------------------------------------------------
 * restore  -  Restore interrupts to value given by mask argument
 *------------------------------------------------------------------------
 */
restore:
	push	{r1, r2}	/* Save r1, r2 on stack			*/
	mrs	r1, cpsr	/* Copy CPSR into r1			*/
	ldr	r2, =0x01F00220
	and	r1, r1, r2	/* Extract flags and other important	*/
	bic	r0, r0, r2	/*    bits from the mask		*/
	orr	r1, r1, r0
	msr	cpsr_cfsx, r1	/* Restore the CPSR			*/
	pop	{r1, r2}	/* Restore r1, r2			*/
	mov	pc, lr		/* Return to caller			*/

/*------------------------------------------------------------------------
 * enable  -  Enable interrupts
 *------------------------------------------------------------------------
 */
enable:
	cpsie	i		/* Enable interrupts			*/
	mov	pc, lr		/* Return				*/

/*------------------------------------------------------------------------
 * pause or halt  -  Place the processor in a hard loop
 *------------------------------------------------------------------------
 */
halt:
pause:
	cpsid	i		/* Disable interrupts		*/
dloop:	b	dloop		/* Dead loop			*/

/*------------------------------------------------------------------------
 * irq_except  -  Dispatch an IRQ exception to higher level IRQ dispatcher
 *------------------------------------------------------------------------
 */
irq_except:
	sub	lr, lr, #4	/* Correct the return address		*/
	srsdb	sp!, #19	/* Save return state on the supervisor	*/
				/*   mode stack				*/
	cps	#19		/* Change to supervisor mode		*/
	push	{r0-r12, lr}	/* Save all registers			*/
	bl	irq_dispatch	/* Call IRQ dispatch			*/
	pop	{r0-r12, lr}	/* Restore all registers		*/
	rfeia	sp!		/* Return from the exception using info	*/
				/*   stored on the stack		*/

/*------------------------------------------------------------------------
 * expjmpinstr - A PC relative jump instruction, copied into exp. vector
 *------------------------------------------------------------------------
 */
expjmpinstr:
	ldr	pc, [pc, #ARMV7A_EH_OFFSET]
