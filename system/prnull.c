/* prnull.c	- prnull */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  prnull  -  Null process: poll ready list until there is another
 *  		   process to run.
 *------------------------------------------------------------------------
 */
process	prnull(void){

	while(TRUE){
		if(!isempty(readylist)){
			resched();
		}
	}

	return SYSERR;
}
