#include <xinu.h>
#include <test.h>

void test_core_wakeup(void){
	cid32 cid = getcid();
	print_info("Hello from core %d!!!\n", cid);
}

result startcores_test(void){
	uint32 core;

	EXPECT_EQ("getcid()", getcid(), 0, DEC);

	for(core = 1; core < NCORE; core++){
		print_info("starting core %d...\n", core);
		cpu_set_entry(startcoretest);
		core_func[core] = test_core_wakeup;
		EXPECT_EQ("core_func[core]", (uint32)core_func[core],
				(uint32)test_core_wakeup, HEX);
		EXPECT_EQ("test_core_init(core)", test_core_init(core),
				OK, DEC);
		EXPECT_EQ("core_up[core]", core_up[core], TRUE, BOOL);
	}

	return PASS;
}

