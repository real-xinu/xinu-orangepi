/* spinlock.S - spin_lock, spin_unlock */

#include <cache.h>

.text
.globl spin_lock
.globl spin_unlock

/*------------------------------------------------------------------------
 * spin_lock  -  Lock a spinlock using arm exclusive monitors
 *------------------------------------------------------------------------
 */
spin_lock:
		ldr r1, =LOCKED
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
 * spin_unlock  -  Unlock a spinlock using arm exclusive monitors
 *------------------------------------------------------------------------
 */
spin_unlock:
		ldr r1, =UNLOCKED
		dmb				/* Required before releasing protected resource */
		str r1, [r0]	/* Unlock mutex */
		bx lr
