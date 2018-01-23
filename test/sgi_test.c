#include <xinu.h>
#include <test.h>

/* flags set by sgi test handlers */
volatile bool8 sgi13;
volatile bool8 sgi14;
volatile bool8 sgi15;

/* test handler declarations */
void thandler13(void);
void thandler14(void);
void thandler15(void);

/* test software generated interrupts */
result sgi_test(void){
    cid32 thiscore = getcid();
    
    /* save sgi handlers for restoration later */
    uint32 sgi13_handler = irq_vector[13];
    uint32 sgi14_handler = irq_vector[14];
    uint32 sgi15_handler = irq_vector[15];

    /* set sgi handlers and flags for testing */
    sgi13 = FALSE;
    set_irq_handler(13, (uint32)thandler13);
    sgi14 = FALSE;
    set_irq_handler(14, (uint32)thandler14);
    sgi15 = FALSE;
    set_irq_handler(15, (uint32)thandler15);

    /* generate sgis to run handlers */
    EXPECT_EQ("sendsgi(13, thiscore)", sendsgi(13, thiscore), OK, DEC);
    EXPECT_EQ("sendsgi(14, thiscore)", sendsgi(14, thiscore), OK, DEC);
    EXPECT_EQ("sendsgi(15, thiscore)", sendsgi(15, thiscore), OK, DEC);

    /* test bad input */
    EXPECT_EQ("sendsgi(16, thiscore)", sendsgi(16, thiscore), SYSERR, DEC);
    EXPECT_EQ("sendsgi(-1, thiscore)", sendsgi(-1, thiscore), SYSERR, DEC);
    EXPECT_EQ("sendsgi(3, (NCPU+1)", sendsgi(3, (NCPU+1)), SYSERR, DEC);
    EXPECT_EQ("sendsgi(3, -1)", sendsgi(3, -1), SYSERR, DEC);

    /* check flags */
    EXPECT_EQ("sgi13", sgi13, TRUE, BOOL);
    EXPECT_EQ("sgi14", sgi14, TRUE, BOOL);
    EXPECT_EQ("sgi15", sgi15, TRUE, BOOL);

    /* restore original sgis */
    set_irq_handler(13, (uint32)sgi13_handler);
    set_irq_handler(14, (uint32)sgi14_handler);
    set_irq_handler(15, (uint32)sgi15_handler);

    return PASS;
}

void thandler13(void){
    kprintf("Hello from sgi 13!\n");
    sgi13 = TRUE;
    return;
}

void thandler14(void){
    kprintf("Hello from sgi 14!\n");
    sgi14 = TRUE;
    return;
}

void thandler15(void){
    kprintf("Hello from sgi 15!\n");
    sgi15 = TRUE;
    return;
}