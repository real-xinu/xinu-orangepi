#include <xinu.h>
#include <test.h>

result cache_test(void){
	/* read the L2 control register */
	uint32 l2ctlr;
	asm volatile ("mrc p15, 1, %0, c9, c0, 2\t\n": "=r"(l2ctlr) );
	/* expect that L2 cache is configured for 4 cpus */
	print_info("Checking that L2 cache is configured for 4 cpus.\n");
	EXPECT_EQ("l2ctlr & 0x03000000", l2ctlr & 0x03000000, 0x03000000, HEX);

	/* read the system control register */
	uint32 sctlr;
	asm volatile ("mrc p15, 0, %0, c1, c0, 0\t\n": "=r"(sctlr) );
	/* expect that address translation is enabled */
	print_info("Expecting normal exception base register.\n");
	EXPECT_EQ("sctlr & ARMV7A_SCTLR_V", sctlr & ARMV7A_SCTLR_V, 0x0, HEX);
	print_info("Expecting I-cache enabled.\n");
	EXPECT_EQ("sctlr & ARMV7A_SCTLR_I", sctlr & ARMV7A_SCTLR_I, ARMV7A_SCTLR_I, HEX);
	print_info("Expecting D-cache enabled.\n");
	EXPECT_EQ("sctlr & ARMV7A_SCTLR_C", sctlr & ARMV7A_SCTLR_C, ARMV7A_SCTLR_C, HEX);
	print_info("Expecting strict alignment enabled.\n");
	EXPECT_EQ("sctlr & ARMV7A_SCTLR_A", sctlr & ARMV7A_SCTLR_A, ARMV7A_SCTLR_A, HEX);
	print_info("Expecting MMU enabled.\n");
	EXPECT_EQ("sctlr & ARMV7A_SCTLR_M", sctlr & ARMV7A_SCTLR_M, ARMV7A_SCTLR_M, HEX);
//	print_info("Expecting Branch Prediction enabled.\n");
//	EXPECT_EQ("sctlr & ARMV7A_SCTLR_Z", sctlr & ARMV7A_SCTLR_Z, ARMV7A_SCTLR_Z, HEX);
	print_info("Expecting Access Flag disabled.\n");
	EXPECT_EQ("sctlr & ARMV7A_SCTLR_AFE", sctlr & ARMV7A_SCTLR_AFE, 0, HEX);
	print_info("Expecting TEX Remap Flag disabled.\n");
	EXPECT_EQ("sctlr & ARMV7A_SCTLR_TRE", sctlr & ARMV7A_SCTLR_TRE, 0, HEX);

	/* read the auxiliary control register */
	uint32 actlr;
	asm volatile ("mrc p15, 0, %0, c1, c0, 1\t\n": "=r"(actlr) );
	/* expect that actlr bit is on */
	print_info("Checking that smp bit is set for core 0.\n");
	EXPECT_EQ("actlr & 0x40", actlr & 0x40, 0x40, HEX);

	/* check tlb base */
	uint32 ttb = 0;
	//TODO: print_info("&page_dir = 0x%X.\n", &page_dir);
	asm volatile ("mrc p15, 0, %0, c2, c0, 0\t\n": "=r"(ttb) );
	print_info("TT base address 0  is 0x%X.\n", ttb);
	asm volatile ("mrc p15, 0, %0, c2, c0, 1\t\n": "=r"(ttb) );
	print_info("TT base address 1  is 0x%X.\n", ttb);
	asm volatile ("mrc p15, 0, %0, c2, c0, 2\t\n": "=r"(ttb) );
	print_info("TT base control is 0x%X.\n", ttb);
	asm volatile ("mrc p15, 0, %0, c3, c0, 0\t\n": "=r"(ttb) );
	print_info("domain access control is 0x%X.\n", ttb);

	/* check processor mode */
	uint32 pmode;
	asm (" mrs %0, cpsr\n" : "=r" (pmode));
	print_info("Processor mode is 0x%X\n", pmode & 0x1F);

	/* check reg 13 PID */
	uint32 dacr;
	asm volatile ("mrc p15, 0, %0, c13, c0, 1\t\n": "=r"(dacr) );
	print_info("reg 13 dacr is is 0x%X\n", dacr);

	/* check SCU base */
	uint32* scuptr;
	asm volatile ("mrc p15, 4, %0, c15, c0, 0\t\n": "=r"(scuptr) );
	print_info("checking PERIPHBASE...\n");
	EXPECT_EQ("(uint32)scuptr", (uint32)scuptr, GIC_BASE, HEX);
	print_info("Checking that SCU is enabled.\n");
	print_info("*scuptr & 0x1 = 0x%x.\n", *scuptr & 0x1);
	//EXPECT_EQ("*scuptr & 0x1", *scuptr & 0x1, 0x1, HEX);

	return PASS;
}
