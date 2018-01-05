#include <xinu.h>
#include <test.h>

uint32 varargfunc(uint32 retarg, uint32 nargs, ...);

/*------------------------------------------------------------------------
 *  argstack_test  -  verify order of arguments in variadic functions
 *------------------------------------------------------------------------
 */
result argstack_test(void){
    EXPECT_EQ("varargfunc(0, 2, 1,2)", varargfunc(0, 2, 1,2), 1,DEC);
    EXPECT_EQ("varargfunc(1, 2, 1,2)", varargfunc(1, 2, 1,2), 2,DEC);
    int i;
    char tname[] = "varargfunc(i, 7, 0,1,2,3,4,5,6)";
    for(i = 0; i < 6; i++){
        tname[11] = i + 0x30;
        EXPECT_EQ(tname, varargfunc(i, 7, 0,1,2,3,4,5,6), i, DEC);
    }

    return PASS;
}

/*------------------------------------------------------------------------
 *  varargfunc  -  generic variadic function for testing:
 *                 returns requested argument
 *------------------------------------------------------------------------
 */
uint32 varargfunc(uint32 retarg, uint32 nargs, ...){
    uint32 *args = (uint32*)(&nargs + 1);	/* points to list of args	*/
    int i;
    for(i = 0; i < nargs; i++){
        kprintf("%d", args[i]);
    }
    return args[retarg];
}