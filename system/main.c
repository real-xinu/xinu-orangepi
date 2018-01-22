/*  main.c  - main */

#include <xinu.h>

#define PRCM_FCLK_GPIO	0x44E000B0

uint32 global_i = 0x7fffffff;

void hooksample(uint32 xnum, uint32 flag)	{
	if(GPIO_PIN_22&flag)
		kprintf("\t\t Hook Arguments: XNUM%d 0x%08x\n", xnum, flag);
}
void buttonPress(uint32 xnum, uint32 flag)	{
	kprintf("***BUTTON PRESSED***%d\n", global_i);
}

process	main(void)
{

	/* Run LED crawl */
	volatile uint32 i;
	recvclr();
	uint32 vwrite = 0;
	uint32 vread = 0;
#if 0

/* Debounce test */


	init(GPIO2);
	control(GPIO2, GPIO_REG_INT_HANDLER, (int32)buttonPress, 0);
	control(GPIO2, GPIO_OUTPUT_ENABLE,GPIO_PIN_06, 0);
	control(GPIO2, GPIO_INTERRUPT_CTL, GPIO_INT_LINE0_EN|GPIO_INT_RISE_TRIG, GPIO_PIN_07);
	control(GPIO2, GPIO_DEB_SET_TIME, 0x01, GPIO_PIN_07);


	while(global_i--)	{
		vwrite = 0xFFFFFFFF;
		write(GPIO2,(char*)&vwrite,GPIO_PIN_06);
		vwrite = 0;
		write(GPIO2,(char*)&vwrite,GPIO_PIN_06);
	}
	while(1);
/* Debounce test */




	pinmodeselect(PIN_CONF_GPMC_A5, PIN_MODE_7, PIN_PULL_DISABLE|PIN_INPUT_ENABLE);
	pinmodeselect(PIN_CONF_GPMC_A6, PIN_MODE_7, PIN_PULL_DISABLE|PIN_INPUT_ENABLE);
	pinmodeselect(PIN_CONF_GPMC_A7, PIN_MODE_7, PIN_PULL_DISABLE|PIN_INPUT_ENABLE);
	pinmodeselect(PIN_CONF_GPMC_A8, PIN_MODE_7, PIN_PULL_DISABLE|PIN_INPUT_ENABLE);


	control(GPIO1, GPIO_REG_INT_HANDLER, (int32)hooksample, 0);		
	control(GPIO1, GPIO_OUTPUT_ENABLE,(GPIO_PIN_21|GPIO_PIN_22|GPIO_PIN_23|GPIO_PIN_24), 0);
	control(GPIO1, GPIO_DEB_SET_TIME, 0x00, GPIO_PIN_21);
	control(GPIO1, GPIO_INTERRUPT_CTL, GPIO_INT_LINE0_EN|GPIO_INT_RISE_TRIG, GPIO_PIN_21|GPIO_PIN_22|GPIO_PIN_23|GPIO_PIN_24);
	control(GPIO1, GPIO_INTERRUPT_CTL, GPIO_INT_LINE1_EN|GPIO_INT_FALL_TRIG, GPIO_PIN_21|GPIO_PIN_22|GPIO_PIN_23|GPIO_PIN_24);

#endif

	/* Interrupt Test */
	control(GPIOA, GPIO_OUTPUT_MODE,10, PIN_MODE_1);
	control(GPIOA, GPIO_PULL_SELECT,10, PIN_PULL_DOWN);
	control(GPIOA, GPIO_INPUT_MODE,9, PIN_MODE_0);
	control(GPIOA, GPIO_PULL_SELECT,9, PIN_PULL_DOWN);
	control(GPIOA, GPIO_INTERRUPT_CTL,GPIO_INT_ENABLE|GPIO_INT_RISE_TRIG, 9);
	control(GPIOA, GPIO_REG_INT_HANDLER, (int32)hooksample, 0);

	while(1){
		vwrite = 0;
		write(GPIOA,(char*)&vwrite,GPIO_PIN_10);
		sleepms(500);
		vwrite = GPIO_PIN_10;
		write(GPIOA,(char*)&vwrite,GPIO_PIN_10);
		sleepms(500);
	}

	struct gic_distreg* gicdist = (struct gic_distreg*)GIC_DIST_BASE;
/*	//struct gic_cpuifreg* giccpuif = (struct gic_cpuifreg*)GIC_CPUIF_BASE;
	for(i=0;i<16;i++)	{
		kprintf("0x%08x GIC int %d seten\n", gicdist->seten[i],i);
		kprintf("0x%08x GIC int %d setpnd\n", gicdist->setpnd[i],i);
		kprintf("0x%08x GIC int %d setact\n", gicdist->setact[i],i);
		kprintf("0x%08x GIC int %d status\n", gicdist->status[i],i);
gicdist->clren[i] = 0xFFFFFFFF;
gicdist->clrpnd[i] = 0xFFFFFFFF;
gicdist->clract[i] = 0xFFFFFFFF;
gicdist->status[i] = 0x0;
	}
for(i=0;i<2000000;i++);

	for(i=0;i<16;i++)	{
		kprintf("0x%08x GIC int %d seten\n", gicdist->seten[i],i);
		kprintf("0x%08x GIC int %d setpnd\n", gicdist->setpnd[i],i);
		kprintf("0x%08x GIC int %d setact\n", gicdist->setact[i],i);
		kprintf("0x%08x GIC int %d status\n", gicdist->status[i],i);
}

*/
	kprintf("0x%08x GIC int %d status\n", gicdist->status[1],1);
	kprintf("0x%08x GIC int %d setpnd\n", gicdist->setpnd[1],1);
	kprintf("STARTING LED CRAWL\n");
	control(GPIOA, GPIO_INPUT_MODE,10, PIN_MODE_0);
	control(GPIOA, GPIO_PULL_SELECT,10, PIN_PULL_DOWN);
	kprintf("STARTING LED CRAWL\n");
	control(GPIOA, GPIO_OUTPUT_MODE,7, PIN_MODE_1);
	control(GPIOA, GPIO_INTERRUPT_CTL,GPIO_INT_ENABLE|GPIO_INT_RISE_TRIG, 9);
	kprintf("STARTING LED CRAWL\n");
	control(GPIOA, GPIO_OUTPUT_MODE,8, PIN_MODE_1);
	control(GPIOA, GPIO_OUTPUT_MODE,9, PIN_MODE_1);
	kprintf("STARTING LED CRAWL\n");//control(GPIOA, GPIO_OUTPUT_MODE,14, PIN_MODE_1);
	//control(GPIOA, GPIO_PULL_SELECT,7, PIN_PULL_DISABLE);
	//control(GPIOA, GPIO_PULL_SELECT,8, PIN_PULL_DISABLE);
	//control(GPIOA, GPIO_PULL_SELECT,9, PIN_PULL_DISABLE);
	//control(GPIOA, GPIO_PULL_SELECT,14, PIN_PULL_DISABLE);
	
	write(GPIOA,(char*)&vwrite,(GPIO_PIN_07|GPIO_PIN_08|GPIO_PIN_09));
	kprintf("STARTING LED CRAWL\n");
	while(1)	{
		vwrite = GPIO_PIN_08;
		write(GPIOA,(char*)&vwrite,GPIO_PIN_07|GPIO_PIN_08);
		kprintf ( "gpio1 write = 0x%08x\n", vwrite);
		//sleepms(100);
		for(i=0;i<100;i++);
		read(GPIOA,(char*)&vread,(GPIO_PIN_10|GPIO_PIN_07|GPIO_PIN_08|GPIO_PIN_09));
		//sleepms(500);
		for(i=0;i<2000000;i++);
		kprintf ( "gpio1 read = 0x%08x\n", vread);

		vwrite = GPIO_PIN_09;
		write(GPIOA,(char*)&vwrite,GPIO_PIN_09|GPIO_PIN_08);
		kprintf ( "gpio1 write = 0x%08x\n", vwrite);

		//sleepms(1);
		for(i=0;i<100;i++);
		read(GPIOA,(char*)&vread,(GPIO_PIN_10|GPIO_PIN_07|GPIO_PIN_08|GPIO_PIN_09));
		//sleepms(500);
		for(i=0;i<2000000;i++);
		kprintf ( "gpio1 read = 0x%08x\n", vread);

		vwrite = GPIO_PIN_07;
		write(GPIOA,(char*)&vwrite,GPIO_PIN_07|GPIO_PIN_09);
		kprintf ( "gpio1 write = 0x%08x\n", vwrite);

		//sleepms(1);
		for(i=0;i<100;i++);
		read(GPIOA,(char*)&vread,(GPIO_PIN_10|GPIO_PIN_07|GPIO_PIN_08|GPIO_PIN_09));
		//sleepms(500);
		for(i=0;i<2000000;i++);
		kprintf ( "gpio1 read = 0x%08x\n", vread);
		kprintf("0x%08x GIC int %d status\n", gicdist->status[1],1);
		kprintf("0x%08x GIC int %d setpnd\n", gicdist->setpnd[1],1);
	}


	kprintf ( "Reached Main Process\n");
	return OK;
    
}
