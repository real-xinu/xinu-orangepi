/* xsh_cpudump.c  - xsh_cpudump */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_cpudump - shell command to print info from the cpuice switch table
 *------------------------------------------------------------------------
 */
shellcmd xsh_cpudump (
	 int	nargs,			/* number of arguments		*/
	 char	*args[]			/* list of arguments		*/
 	)
{
	/* No arguments are expected */

	if (nargs > 1) {
		fprintf(stderr, "No arguments allowed\n");
		return 1;
	}

	cpu_dump();

    return 0;
}