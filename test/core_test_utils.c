#include <xinu.h>
#include <test.h>

byte core_stacks[4096*4];
char rpfmt[] = "\nregprint: 0x%x\n";

void* core_func[4];
bool8 core_up[4];

void test_init_cores(void){
	uint32 core;
	for (core = 0; core < NCORE; core++){
		core_func[core] = NULL;
		core_up[core] = FALSE;
	}
	core_up[0] = TRUE;
}

status test_core_init(uint32 core){
	if(core < 1 || core >= NCORE){
		return SYSERR;
	}

	if(!core_up[core]){
		cpu_set_entry(startcoretest);
		cpu_enable(core);
		sleep(1);
	}

	return OK;
}

void core_startup(void){
	bputc();
	cid32 cid = getcid();
	core_up[cid] = TRUE;
	kprintf("Hello from core %d!\n", cid);
	void (*func)(void);
	while(1){
		while( (func = core_func[cid]) == NULL);
		func();
		core_func[cid] = NULL;
	}
}
