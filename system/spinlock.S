/* spinlock.S - s_lock, s_unlock */

.text
.globl arm_lock
.globl arm_unlock

/*------------------------------------------------------------------------
 * arm_lock  -  Lock a spinlock using arm exclusive monitors
 *------------------------------------------------------------------------
 */
arm_lock:
		ldr r1, =1	/* SLK_LOCKED = 1 */
spin:	ldrex r2, [r0]
		cmp r2, r1 	/* Test if mutex is locked or unlocked */
		beq spin 	/* If locked - wait for it to be released */
		strexne r2, r1, [r0] /* Not locked, attempt to lock it */
		cmpne r2, #1 	/* Check if Store-Exclusive failed */
		beq spin		/* Failed - retry */
		/* Lock acquired */
		dmb	/* Required before accessing protected resource */
		bx lr

/*------------------------------------------------------------------------
 * arm_unlock  -  Unlock a spinlock using arm exclusive monitors
 *------------------------------------------------------------------------
 */
arm_unlock:
		ldr r1, =0		/* SLK_UNLOCKED = 0 */
		dmb				/* Required before releasing protected resource */
		str r1, [r0]	/* Unlock mutex */
		bx lr
