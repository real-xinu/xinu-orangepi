/* prnull.c	- prnull */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  prnull  -  Null process: poll ready list until there is another
 *  		   process to run.
 *------------------------------------------------------------------------
 */
process	prnull(void){

	while(TRUE){
		// if(firstkey(readylist) > 0){
		// 	resched();
		// }
		// TODO: wfi?
	}

	return SYSERR;
}
