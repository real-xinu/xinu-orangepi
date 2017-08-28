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
#define RDISK               11	/* type rds      */
#define RAM0                12	/* type ram      */
#define RFILESYS            13	/* type rfs      */
#define RFILE0              14	/* type rfl      */
#define RFILE1              15	/* type rfl      */
#define RFILE2              16	/* type rfl      */
#define RFILE3              17	/* type rfl      */
#define RFILE4              18	/* type rfl      */
#define RFILE5              19	/* type rfl      */
#define RFILE6              20	/* type rfl      */
#define RFILE7              21	/* type rfl      */
#define RFILE8              22	/* type rfl      */
#define RFILE9              23	/* type rfl      */
#define LFILESYS            24	/* type lfs      */
#define LFILE0              25	/* type lfl      */
#define LFILE1              26	/* type lfl      */
#define LFILE2              27	/* type lfl      */
#define LFILE3              28	/* type lfl      */
#define LFILE4              29	/* type lfl      */
#define LFILE5              30	/* type lfl      */
#define SPI0                31	/* type spi      */
#define SPI1                32	/* type spi      */

/* Control block sizes */

#define	Nnull	1
#define	Ngpio	7
#define	Ntty	1
#define	Neth	1
#define	Nrds	1
#define	Nram	1
#define	Nrfs	1
#define	Nrfl	10
#define	Nlfs	1
#define	Nlfl	6
#define	Nnam	1
#define	Nspi	2

#define NDEVS 33


/* Configuration and Size Constants */

#define	NPROC	     100	/* number of user processes		*/
#define	NSEM	     100	/* number of semaphores			*/
#define	IRQBASE	     32		/* base ivec for IRQ0			*/
#define	IRQ_TIMER    IRQ_HW5	/* timer IRQ is wired to hardware 5	*/
#define	IRQ_ATH_MISC IRQ_HW4	/* Misc. IRQ is wired to hardware 4	*/
#define CLKFREQ      200000000	/* 200 MHz clock			*/

#define	LF_DISK_DEV	RAM0
