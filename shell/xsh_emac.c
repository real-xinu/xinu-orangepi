/* xsh_help.c - xsh_help */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xhs_emac - dump emac information
 *------------------------------------------------------------------------
 */
shellcmd xsh_emac(int nargs, char *args[])
{
	/* For argument '--help', emit help about the 'help' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {

		printf("Use: emac\n");
		printf("Prints EMAC information to stdout\n");
		return 0;
	}

	/* Check for valid number of arguments */

	if (nargs > 1) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Usage: emac");
		return 1;
	}

	emac_debug();
	return 0;
}
