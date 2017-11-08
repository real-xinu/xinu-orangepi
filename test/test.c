// TODO: just for personal testing, remove at some point
/* test.c : basic testing framework */

#include <xinu.h>
#include <test.h>
#include <stdarg.h>

extern	void	_doprnt(char *, va_list, int (*)(int), int);
extern int strcmp(char*, char*);

struct test tests[] = {
		{"cache_test", cache_test},
//		{"startcores_test", startcores_test},
//		{"spinlock_test", spinlock_test},
		//{"armmutex_test", armmutex_test},
//		{"armmutexERG_test", armmutexERG_test},
		/* null terminate list of tests for easy iterating */
		{"null", NULL}
};

/******************** Test Driver ********************/
void run_all_tests(void){
	/* initialize */
	uint32 total = 0;
	uint32 passed = 0;
	uint32 failed = 0;

	//test_init_cores();

	kprintf("################ AUTOMATED TESTS ################\n");

	/* run */
	struct test* tstptr = &tests[0];
	kprintf("\n");
	while( tstptr->func != NULL ){
		total++;
		print_info("**************** %s ****************\n", tstptr->name);
		if(tstptr->func() == PASS){
			passed++;
			print_pass("**************** %s ****************\n", tstptr->name);
		} else {
			failed++;
			print_fail("**************** %s ****************\n", tstptr->name);
		}
		kprintf("\n");
		tstptr++;
	}

	/* report */
	print_info("**************** testing complete ****************\n");
	print_info("Ran %d Test(s)\n", total);
	if(passed){print_pass("%d Passed\n", passed);}
	if(failed){print_fail("%d Failed\n", failed);}
	kprintf("\n");
	kprintf("################# AUTOMATED TESTS #################\n");
}

/******************** Helper functions ********************/

void print_pass(char* msg, ...){
	kprintf(TERM_BGREEN);
	kprintf("[PASS] ");
	kprintf(TERM_RESET);

	va_list ap;

	va_start(ap, msg);
	_doprnt(msg, ap, (int (*)(int))kputc, (int)&devtab[CONSOLE]);
	va_end(ap);
}

void print_fail(char* msg, ...){
	kprintf(TERM_BRED);
	kprintf("[FAIL] ");
	kprintf(TERM_RESET);

	va_list ap;

	va_start(ap, msg);
	_doprnt(msg, ap, (int (*)(int))kputc, (int)&devtab[CONSOLE]);
	va_end(ap);
}

void print_info(char* msg, ...){
	kprintf(TERM_BWHITE);
	kprintf("[INFO] ");
	kprintf(TERM_RESET);

	va_list ap;

	va_start(ap, msg);
	_doprnt(msg, ap, (int (*)(int))kputc, (int)&devtab[CONSOLE]);
	va_end(ap);
}

result test_eq(char* name, uint32 actual, uint32 expected, enum type arg_type){
	switch (arg_type) {

	case CHAR:
		if ( actual == expected ){
			print_pass("%s: Expected %c, Actual %c\n", name,
					(char*)expected, (char*)actual);
			return TRUE;
		} else {
			print_fail("%s: Expected %c, Actual %c\n", name,
					(char*)expected, (char*)actual);
			return FALSE;
		}
		break;

	case BOOL:
		/* fall through */
	case DEC:
		if ( actual == expected ){
			print_pass("%s: Expected %d, Actual %d\n", name,
					expected, actual);
			return TRUE;
		} else {
			print_fail("%s: Expected %d, Actual %d\n", name,
					expected, actual);
			return FALSE;
		}
		break;

	case HEX:
		if ( actual == expected ){
			print_pass("%s: Expected 0x%X, Actual 0x%X\n", name,
					expected, actual);
			return TRUE;
		} else {
			print_fail("%s: Expected 0x%X, Actual 0x%X\n", name,
					expected, actual);
			return FALSE;
		}
		break;

	case STRING:
		if ( strcmp((char*)actual, (char*)expected) == 0 ){
			print_pass("%s: Expected %s, Actual %s\n", name,
					(char*)expected, (char*)actual);
			return TRUE;
		} else {
			print_fail("%s: Expected %s, Actual %s\n", name,
					(char*)expected, (char*)actual);
			return FALSE;
		}
		break;

	default:
		return FALSE;
	}

		return FALSE;
}
