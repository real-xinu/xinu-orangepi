#include <xinu.h>
#include <test.h>

/*------------------------------------------------------------------------
 *  xsec_test  -  verify basic xsec functionality
 *------------------------------------------------------------------------
 */
result xsec_test(void){
    lid32 tlids[7];
    int i;
    intmask mask;
    char tlname0[] = "tlids[i] != SYSERR";
    char tlname1[] = "locktab[tlids[i]].lock == LOCKED";
    char tlname2[] = "locktab[tlids[i]].lock == UNLOCKED";

    for (i = 0; i < 7; i++){
        tlids[i] = newlock();
        tlname0[6] = i + 0x30; // convert to int to char ascii
        EXPECT_EQ(tlname0, locktab[tlids[i]].lock != SYSERR, TRUE, BOOL); 
    }

    mask = xsec_beg(tlids[0]);
    tlname1[14] = 0 + 0x30;
    EXPECT_EQ(tlname1, locktab[tlids[0]].lock == LOCKED, TRUE, BOOL);
    xsec_end(mask, tlids[0]);
    tlname2[14] = 0 + 0x30;
    EXPECT_EQ(tlname2, locktab[tlids[0]].lock == UNLOCKED, TRUE, BOOL);

    mask = xsec_begn(4, tlids[1], tlids[5], tlids[2], tlids[6]);
    tlname1[14] = 1 + 0x30;
    EXPECT_EQ(tlname1, locktab[tlids[1]].lock == LOCKED, TRUE, BOOL);
    tlname1[14] = 5 + 0x30;
    EXPECT_EQ(tlname1, locktab[tlids[5]].lock == LOCKED, TRUE, BOOL);
    tlname1[14] = 2 + 0x30;
    EXPECT_EQ(tlname1, locktab[tlids[2]].lock == LOCKED, TRUE, BOOL);
    tlname1[14] = 6 + 0x30;
    EXPECT_EQ(tlname1, locktab[tlids[6]].lock == LOCKED, TRUE, BOOL);
    xsec_endn(mask, 4, tlids[1], tlids[5], tlids[2], tlids[6]);
    tlname2[14] = 1 + 0x30;
    EXPECT_EQ(tlname2, locktab[tlids[1]].lock == UNLOCKED, TRUE, BOOL);
    tlname2[14] = 5 + 0x30;
    EXPECT_EQ(tlname2, locktab[tlids[5]].lock == UNLOCKED, TRUE, BOOL);
    tlname2[14] = 2 + 0x30;
    EXPECT_EQ(tlname2, locktab[tlids[2]].lock == UNLOCKED, TRUE, BOOL);
    tlname2[14] = 6 + 0x30;
    EXPECT_EQ(tlname2, locktab[tlids[6]].lock == UNLOCKED, TRUE, BOOL);

    return PASS;
}