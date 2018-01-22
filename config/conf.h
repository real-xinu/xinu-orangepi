/* conf.h (GENERATED FILE; DO NOT EDIT) */

/* Device switch table declarations */

/* Device table entry */
struct	dentry	{
	int32   dvnum;
	int32   dvminor;
	char    *dvname;
	devcall (*dvinit) (struct dentry *);
	devcall (*dvopen) (struct dentry *, char *, char *);
	devcall (*dvclose)(struct dentry *);
	devcall (*dvread) (struct dentry *, void *, uint32);
	devcall (*dvwrite)(struct dentry *, void *, uint32);
	devcall (*dvseek) (struct dentry *, int32);
	devcall (*dvgetc) (struct dentry *);
	devcall (*dvputc) (struct dentry *, char);
	devcall (*dvcntl) (struct dentry *, int32, int32, int32);
	void    *dvcsr;
	void    (*dvintr)(void);
	byte    dvirq;
};

extern	struct	dentry	devtab[]; /* one entry per device */

/* Device name definitions */

#define CONSOLE              0	/* type tty      */
#define GPIOA                1	/* type gpio     */
#define GPIOC                2	/* type gpio     */
#define GPIOD                3	/* type gpio     */
#define GPIOE                4	/* type gpio     */
#define GPIOF                5	/* type gpio     */
#define GPIOG                6	/* type gpio     */
#define GPIOL                7	/* type gpio     */
#define NULLDEV              8	/* type null     */
#define ETHER0               9	/* type eth      */
#define NAMESPACE           10	/* type nam      */
#define RAM0                11	/* type ram      */
#define LFILESYS            12	/* type lfs      */
#define LFILE0              13	/* type lfl      */
#define LFILE1              14	/* type lfl      */
#define LFILE2              15	/* type lfl      */
#define LFILE3              16	/* type lfl      */
#define LFILE4              17	/* type lfl      */
#define LFILE5              18	/* type lfl      */

/* Control block sizes */

#define	Nnull	1
#define	Ngpio	7
#define	Ntty	1
#define	Neth	1
#define	Nram	1
#define	Nlfs	1
#define	Nlfl	6
#define	Nnam	1

#define NDEVS 19


/* Configuration and Size Constants */

#define	NPROC	     100	/* number of user processes		*/
#define	NSEM	     100	/* number of semaphores			*/
#define	IRQBASE	     32		/* base ivec for IRQ0			*/
#define	IRQ_TIMER    IRQ_HW5	/* timer IRQ is wired to hardware 5	*/
#define	IRQ_ATH_MISC IRQ_HW4	/* Misc. IRQ is wired to hardware 4	*/
#define CLKFREQ      200000000	/* 200 MHz clock			*/

#define	LF_DISK_DEV	RAM0
