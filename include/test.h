/*
 * test.h
 *
 *  Created on: Sep 23, 2017
 *      Author: pvanevery
 */

#ifndef INCLUDE_TEST_H_
#define INCLUDE_TEST_H_

#include <kernel.h>

#define TEST_NMLEN 32 /* test name length in bytes */

#define PASS 1
#define FAIL 0

#define TERM_RESET 	"\033[0m"
#define TERM_BWHITE "\033[1;37m"
#define TERM_BGREEN "\033[1;32m"
#define TERM_BRED 	"\033[1;31m"
#define TERM_GREEN 	"\033[0;32m"
#define TERM_RED 	"\033[0;31m"

#define TRUE_STR "true"
#define FALSE_STR "false"

#define EXPECT_EQ(name, actual, expected, arg_type)  \
if(!test_eq(name, actual, expected, arg_type)){return FAIL;}

#define ASSERT_EQ(name, actual, expected, arg_type)  \
if(!test_eq(name, actual, expected, arg_type)) \
{panic("Critical Test Failure");}

typedef bool8 result;

enum type {
	BOOL, 	/* boolean */
	CHAR, 	/* char */
	DEC,	/* 32 bit decimal value */
	HEX,	/* 32 bit hex  value */
	STRING  /* string */
};

void print_pass(char* msg, ...);
void print_fail(char* msg, ...);
void print_info(char* msg, ...);
result test_eq(char* name, uint32 actual, uint32 expected, enum type arg_type);
void run_all_tests(void);

struct test {
	char name[TEST_NMLEN];
	result (*func)(void);
};

/* core util stuff */
#define CORE_WAKEUP 0xC0FFEE
extern void* core_func[];
extern bool8 core_up[];
void test_init_cores(void);
status test_core_init(uint32 core);
extern void startcoretest(void);

/*************** Tests ******************/
result startcores_test(void);
result argstack_test(void);
result xsec_test(void);
result cache_test(void);
result sgi_test(void);

#endif /* INCLUDE_TEST_H_ */
