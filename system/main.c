/*  main.c  - main */

#include <xinu.h>
#include <test.h>

process	main(void)
{
	run_all_tests();
//	int i;
//	uint32* ptable = (uint32*)0x7DFF0000;
//	for(i = 0; i < 4096; i++){
//		kprintf("ptable[%d] = 0x%X\n", i, ptable[i]);
//	}
//	kprintf("ptable[%d] = 0x%X\n", i, ptable[i]);

	/* Run the Xinu shell */

	recvclr();
	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */

	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	return OK;
    
}
