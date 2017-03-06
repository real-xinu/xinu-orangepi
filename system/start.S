/* start.S - start, bzero */

#include <armv7a.h>
#define UART0LSR 0x01C28014
#define UART0RBR 0x01C28000
#define TX_READY 0x40

/*------------------------------------------------------------------------
 * start  -  Initial entry point for a Xinu image (ARM)
 *------------------------------------------------------------------------
 */
	.text
	.globl	start			/* Declare entry point global	*/

start:

	bl bputc
	/* Load the stack pointer with end of memory */

	ldr	sp, =MAXADDR
	bl bputc
	/* Enable the Instruction Cache	*/

	//mrc	p15, 0, r0, c1, c0, 0
	//bl bputc
	//orr	r0, r0, #ARMV7A_C1CTL_I
	//bl bputc
	//mcr	p15, 0, r0, c1, c0, 0
	//bl bputc
	/* Use bzero (below) to zero out the BSS area */

	//ldr	r0, =edata
	//bl bputc
	//ldr	r1, =end
//	bl bputc
	//bl	bzero
	//bl bputc
	/* Call nulluser to initialize the Xinu system  */
	/*    (Note: the call never returns)		*/

	bl	nulluser

	/* Function to zero memory (r0 is lowest addr; r1 is highest)	*/

bzero:
	mov	r2, #0			/* Round address to multiple	*/
	add	r0, r0, #3		/*   of four by adding 3 and	*/
	and	r0, r0, #0xFFFFFFFC	/*   taking the result module 4	*/
bloop:	cmp	r0, r1			/* Loop until last address	*/
	bhs	bexit			/*   has been reached		*/
	str	r2, [r0]		/* Zero four-byte word of memory*/
	add	r0, r0, #4		/* Move to next word		*/
	b	bloop			/* Continue to iterate		*/
bexit:	mov	pc, lr			/* Return to caller		*/

bputc:
	ldr r3, =UART0LSR		/* point to uart line status reg */
	ldr r4, [r3]			/* get value of line status reg */
	and r4, r4, #TX_READY	/* mask tx ready bit bit */
    cmp r4, #TX_READY		/* check for bit set */
	bne bputc				/* keep poling until bit is set */
	ldr r3, =UART0RBR		/* point to TX register */
	mov r4, #67				/* decimal 67 is capitol C */
	str r4, [r3]
	mov pc, lr
