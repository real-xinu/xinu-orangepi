/* gpio.h - definintions for the gpio device */

/*
 * Control and Status Register (CSR) definintions for the GPIO.
 * The code maps the structure structure directly onto the base address
 * CSR address for the device.
 */

/* Number of GPIO devices in the hardware */
#define NGPIO	7

/* Gpio main register set */
struct gpio_csreg {
        volatile uint32 config[4];
        volatile uint32 data;
        volatile uint32 multidrv[2];
        volatile uint32 pull[2];
};

/* Gpio interrupt register set */
struct gpio_intreg {
        volatile uint32 intconfig[4];
        volatile uint32 intctl;
        volatile uint32 intsta;
        volatile uint32 intdeb;
};

typedef void (*gpiointhook)(uint32, uint32);

struct	gpiocblk	{	/* GPIO control block			*/
	gpiointhook gphookfn;	/* Interrupt hook function		*/
};

extern	struct	gpiocblk	gpiotab[];

/* Pin Masks */
#define PIN_MASK(pin)	 (1<<pin)
#define GPIO_PIN_ALL     0xFFFFFFFF
#define GPIO_PIN_00      0x00000001
#define GPIO_PIN_01      0x00000002
#define GPIO_PIN_02      0x00000004
#define GPIO_PIN_03      0x00000008
#define GPIO_PIN_04      0x00000010
#define GPIO_PIN_05      0x00000020
#define GPIO_PIN_06      0x00000040
#define GPIO_PIN_07      0x00000080
#define GPIO_PIN_08      0x00000100
#define GPIO_PIN_09      0x00000200
#define GPIO_PIN_10      0x00000400
#define GPIO_PIN_11      0x00000800
#define GPIO_PIN_12      0x00001000
#define GPIO_PIN_13      0x00002000
#define GPIO_PIN_14      0x00004000
#define GPIO_PIN_15      0x00008000
#define GPIO_PIN_16      0x00010000
#define GPIO_PIN_17      0x00020000
#define GPIO_PIN_18      0x00040000
#define GPIO_PIN_19      0x00080000
#define GPIO_PIN_20      0x00100000
#define GPIO_PIN_21      0x00200000
#define GPIO_PIN_22      0x00400000
#define GPIO_PIN_23      0x00800000
#define GPIO_PIN_24      0x01000000
#define GPIO_PIN_25      0x02000000
#define GPIO_PIN_26      0x04000000
#define GPIO_PIN_27      0x08000000
#define GPIO_PIN_28      0x10000000
#define GPIO_PIN_29      0x20000000
#define GPIO_PIN_30      0x40000000
#define GPIO_PIN_31      0x80000000

/* Pin values */
#define GPIO_VALUE_LOW		0x00
#define GPIO_VALUE_HIGH		0x01

/* Control features */
#define GPIO_INPUT_MODE    	0x00
#define GPIO_OUTPUT_MODE    	0x01
#define GPIO_REG_INT_HANDLER	0x02
#define GPIO_INTERRUPT_CTL	0x03
#define GPIO_DEB_SET_TIME	0x04
#define GPIO_READ_PIN		0x05
#define GPIO_WRITE_PIN		0x06
#define GPIO_SET_MODE		0x07
#define GPIO_PULL_SELECT	0x08

/* Control Flags */
#define GPIO_INT_ENABLE 	0x01
#define GPIO_INT_DISABLE	0x02
#define GPIO_INT_RISE_TRIG 	0x04
#define GPIO_INT_FALL_TRIG	0x08
#define GPIO_INT_LVL0_TRIG 	0x10
#define GPIO_INT_LVL1_TRIG	0x20
#define GPIO_INT_DEDGE_TRIG	(GPIO_INT_RISE_TRIG|GPIO_INT_FALL_TRIG)

/* Trigger values for intconf register */
#define GPIO_INT_POS_TRIG 	0x00
#define GPIO_INT_NEG_TRIG	0x01
#define GPIO_INT_LOW_TRIG 	0x02
#define GPIO_INT_HIGH_TRIG	0x03
#define GPIO_INT_DOUBLE_TRIG	0x04

/* Base values of the CSR addreses of the four GPIO devices used by	*/
/*	init and interrupt processing; read and write functions obtain	*/
/*	these CSR addresses from the csrptr field of the device switch	*/
/*	table.								*/

#define GPIOA_BASE      (struct gpio_csreg *)0x01C20800
#define GPIOC_BASE      (struct gpio_csreg *)0x01C20824
#define GPIOD_BASE      (struct gpio_csreg *)0x01C20848
#define GPIOE_BASE      (struct gpio_csreg *)0x01C2086C
#define GPIOF_BASE      (struct gpio_csreg *)0x01C20890
#define GPIOG_BASE      (struct gpio_csreg *)0x01C208B4
#define GPIOL_BASE      (struct gpio_csreg *)0x01C208D8

#define GPIOA_INT_BASE  (struct gpio_csreg *)0x01C20A00
#define GPIOG_INT_BASE  (struct gpio_csreg *)0x01C20A20

/* Interrupt vector assignments */

#define GPIOA_INT		43
#define GPIOG_INT		49

/* Pin Modes */

#define PIN_MODE_0			0x00
#define PIN_MODE_1			0x01
#define PIN_MODE_2			0x02
#define PIN_MODE_3			0x03
#define PIN_MODE_4			0x04
#define PIN_MODE_5			0x05
#define PIN_MODE_6			0x06
#define PIN_MODE_7			0x07

/* Pull Settings */

#define PIN_PULL_DISABLE		0x0
#define PIN_PULL_UP			0x1
#define PIN_PULL_DOWN			0x2

