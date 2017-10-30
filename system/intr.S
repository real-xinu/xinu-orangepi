/* intr.S - enable, disable, restore, halt, pause, irq_except (ARM) */

#include <armv7a.h>

	.text
	.globl	disable
	.globl	restore
	.globl	enable
	.globl	pause
	.globl	halt
	.globl	rst_except
	.globl	udi_except
	.globl	swi_except
	.globl	pfa_except
	.globl	dab_except
	.globl	rsv_except
	.globl	irq_except
	.globl	fiq_except
	.globl	initevec
	.globl	expjmpinstr

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

/*------------------------------------------------------------------------
 * rst_except  -  Dispatch reset exception to higher level handler
 *------------------------------------------------------------------------
 */
rst_except:
	sub	lr, lr, #4	/* Correct the return address		*/
	srsdb	sp!, #19	/* Save return state on the supervisor	*/
				/*   mode stack				*/
	cps	#19		/* Change to supervisor mode		*/
	push	{r0-r12, lr}	/* Save all registers			*/
	bl	rst_handler	/* Call rst_handler			*/
	pop	{r0-r12, lr}	/* Restore all registers		*/
	rfeia	sp!		/* Return from the exception using info	*/
				/*   stored on the stack		*/

/*------------------------------------------------------------------------
 * udi_except  -  Dispatch undefined instruction exception to
 *				  higher level handler
 *------------------------------------------------------------------------
 */
udi_except:
	sub	lr, lr, #4	/* Correct the return address		*/
	srsdb	sp!, #19	/* Save return state on the supervisor	*/
				/*   mode stack				*/
	cps	#19		/* Change to supervisor mode		*/
	push	{r0-r12, lr}	/* Save all registers			*/
	bl	udi_handler	/* Call udi_handler			*/
	pop	{r0-r12, lr}	/* Restore all registers		*/
	rfeia	sp!		/* Return from the exception using info	*/
				/*   stored on the stack		*/

/*------------------------------------------------------------------------
 * swi_except  -  Dispatch software interrupt exception to higher level
 *				  handler
 *------------------------------------------------------------------------
 */
swi_except:
	sub	lr, lr, #4	/* Correct the return address		*/
	srsdb	sp!, #19	/* Save return state on the supervisor	*/
				/*   mode stack				*/
	cps	#19		/* Change to supervisor mode		*/
	push	{r0-r12, lr}	/* Save all registers			*/
	bl	swi_handler	/* Call swi_handler			*/
	pop	{r0-r12, lr}	/* Restore all registers		*/
	rfeia	sp!		/* Return from the exception using info	*/
				/*   stored on the stack		*/

/*------------------------------------------------------------------------
 * pfa_except  -  Dispatch prefetch abort exception to higher level
 *				  handler
 *------------------------------------------------------------------------
 */
pfa_except:
	sub	lr, lr, #4	/* Correct the return address		*/
	srsdb	sp!, #19	/* Save return state on the supervisor	*/
				/*   mode stack				*/
	cps	#19		/* Change to supervisor mode		*/
	push	{r0-r12, lr}	/* Save all registers			*/
	bl	pfa_handler	/* Call pfa_handler			*/
	pop	{r0-r12, lr}	/* Restore all registers		*/
	rfeia	sp!		/* Return from the exception using info	*/
				/*   stored on the stack		*/

/*------------------------------------------------------------------------
 * dab_except  -  Dispatch data abort exception to higher level
 *				  handler
 *------------------------------------------------------------------------
 */
dab_except:
	sub	lr, lr, #4	/* Correct the return address		*/
	srsdb	sp!, #19	/* Save return state on the supervisor	*/
				/*   mode stack				*/
	cps	#19		/* Change to supervisor mode		*/
	push	{r0-r12, lr}	/* Save all registers			*/
	bl	dab_handler	/* Call dab_handler			*/
	pop	{r0-r12, lr}	/* Restore all registers		*/
	rfeia	sp!		/* Return from the exception using info	*/
				/*   stored on the stack		*/

/*------------------------------------------------------------------------
 * rsv_except  -  Dispatch reserved exception to higher level
 *				  handler
 *------------------------------------------------------------------------
 */
rsv_except:
	sub	lr, lr, #4	/* Correct the return address		*/
	srsdb	sp!, #19	/* Save return state on the supervisor	*/
				/*   mode stack				*/
	cps	#19		/* Change to supervisor mode		*/
	push	{r0-r12, lr}	/* Save all registers			*/
	bl	rsv_handler	/* Call rsv_handler			*/
	pop	{r0-r12, lr}	/* Restore all registers		*/
	rfeia	sp!		/* Return from the exception using info	*/
				/*   stored on the stack		*/

/*------------------------------------------------------------------------
 * fiq_except  -  Dispatch FIQ exception to higher level
 *				  handler
 *------------------------------------------------------------------------
 */
fiq_except:
	sub	lr, lr, #4	/* Correct the return address		*/
	srsdb	sp!, #19	/* Save return state on the supervisor	*/
				/*   mode stack				*/
	cps	#19		/* Change to supervisor mode		*/
	push	{r0-r12, lr}	/* Save all registers			*/
	bl	fiq_handler	/* Call fiq_handler			*/
	pop	{r0-r12, lr}	/* Restore all registers		*/
	rfeia	sp!		/* Return from the exception using info	*/
				/*   stored on the stack		*/

