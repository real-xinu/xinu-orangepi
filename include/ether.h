/* ether.h */

/* Ethernet packet format:

 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |  Dest. MAC (6)  |  Src. MAC (6)   |Type (2)|      Data (46-1500)...   |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

#define	ETH_ADDR_LEN	6	/* Length of Ethernet (MAC) address	*/
typedef	unsigned char	Eaddr[ETH_ADDR_LEN];/* a physical Ethernet address*/

/* Ethernet packet header */

struct	etherPkt {
	byte	dst[ETH_ADDR_LEN];	/* Destination Mac address	*/
	byte	src[ETH_ADDR_LEN];	/* Source Mac address		*/
	uint16	type;			/* Ether type field		*/
	byte	data[1];		/* Packet payload		*/
};

#define	ETH_HDR_LEN		14	/* Length of Ethernet packet 	*/
					/*  header			*/

/* Ethernet DMA buffer sizes */

#define	ETH_MTU			1500	/* Maximum transmission unit	*/
#define	ETH_VLAN_LEN		4	/* Length of Ethernet vlan tag	*/
#define ETH_CRC_LEN		4	/* Length of CRC on Ethernet 	*/
					/*  frame			*/

#define	ETH_MAX_PKT_LEN	( ETH_HDR_LEN + ETH_VLAN_LEN + ETH_MTU )

#define	ETH_BUF_SIZE		1518	/* 1500 MTU + 14 ETH Header + 4 bytes optional VLAN Tagging */

/* State of the Ethernet interface */

#define	ETH_STATE_FREE		0	/* control block is unused 	*/
#define	ETH_STATE_DOWN		1	/* interface is currently  	*/
					/* 	inactive 		*/
#define	ETH_STATE_UP		2	/* interface is currently active*/

/* Ethernet device control functions */

#define	ETH_CTRL_GET_MAC     	1 	/* Get the MAC for this device	*/

/* Ethernet multicast */

#define ETH_NUM_MCAST		32     /* Max number of multicast addresses*/

/* Ehternet NIC type */

#define ETH_TYPE_3C905C 	1
#define ETH_TYPE_E1000E 	2

#define ETH_PHY_CTLREG	0
#define ETH_PHY_STATREG	1

#define ETH_PHY_CTLREG_RESET	0x8000
#define ETH_PHY_CTLREG_SM	0x2040	/* Speed Mask	*/
#define ETH_PHY_10M		0x0000
#define ETH_PHY_100M		0x2000
#define ETH_PHY_1000M		0x0040
#define ETH_PHY_CTLREG_FD	0x0100

#define ETH_PHY_STATREG_LINK	0x0004

/* Ethernet autonegotiation */
#define ANEG_TIMEOUT 5000
#define BMCR_ANEG_ENA 0x1000
#define BMCR_ANEG 0x0200
#define BMSR_ANEGCOMPLETE 0x0020

/* Here are the registers in the PHY device that we use.
 * The first PHY registers are standardized and device
 * independent
 */
#define PHY_BMCR        0
#define PHY_BMSR        1
#define PHY_ID1         2
#define PHY_ID2         3
#define PHY_ADVERT      4
#define PHY_PEER        5

/* Bits in the basic mode control register. */
#define BMCR_RESET              0x8000
#define BMCR_LOOPBACK           0x4000
#define BMCR_100                0x2000		/* Set for 100 Mbit, else 10 */
#define BMCR_ANEG_ENA           0x1000		/* enable autonegotiation */
#define BMCR_POWERDOWN          0x0800		/* set to power down */
#define BMCR_ISOLATE            0x0400
#define BMCR_ANEG               0x0200		/* restart autonegotiation */
#define BMCR_FULL               0x0100		/* Set for full, else half */
#define BMCR_CT_ENABLE          0x0080		/* enable collision test */

/* Bits in the basic mode status register. */

#define BMSR_100FULL            0x4000
#define BMSR_100HALF            0x2000
#define BMSR_10FULL             0x1000
#define BMSR_10HALF             0x0800
#define BMSR_ANEGCOMPLETE       0x0020
#define BMSR_ANEGCAPABLE        0x0008
#define BMSR_LINK_UP            0x0004

/* Bits in the link partner ability register. */
#define LPA_10HALF              0x0020
#define LPA_10FULL              0x0040
#define LPA_100HALF             0x0080
#define LPA_100FULL             0x0100

#define LPA_ADVERT              LPA_10HALF | LPA_10FULL | LPA_100HALF | LPA_100FULL

#define ETH_ADDR_SIZE	6
#define ETH_MIN_SIZE	64
#define ETH_MAX_SIZE	1514


struct	ethcblk	{
	byte	state; 		/* ETH_STATE_... as defined above 	*/
	struct	dentry	*phy;	/* physical eth device for Tx DMA 	*/
	byte 	type; 		/* NIC type_... as defined above 	*/

	/* Pointers to associated structures */

	struct	dentry	*dev;	/* address in device switch table	*/
	void	*csr;		/* addr.of control and status regs.	*/
	uint32	pcidev;		/* PCI device number			*/
	uint32	iobase;		/* I/O base from config			*/
	uint32  flashbase;      /* flash base from config	       	*/
    	uint32	membase; 	/* memory base for device from config	*/

	void    *rxRing;	/* ptr to array of recv ring descriptors*/
	void    *rxBufs; 	/* ptr to Rx packet buffers in memory	*/
	uint32	rxHead;		/* Index of current head of Rx ring	*/
	uint32	rxTail;		/* Index of current tail of Rx ring	*/
	uint32	rxRingSize;	/* size of Rx ring descriptor array	*/
	uint32	rxIrq;		/* Count of Rx interrupt requests       */

	void    *txRing; 	/* ptr to array of xmit ring descriptors*/
	void    *txBufs; 	/* ptr to Tx packet buffers in memory	*/
	uint32	txHead;		/* Index of current head of Tx ring	*/
	uint32	txTail;		/* Index of current tail of Tx ring	*/
	uint32	txRingSize;	/* size of Tx ring descriptor array	*/
	uint32	txIrq;		/* Count of Tx interrupt requests       */

	byte	devAddress[ETH_ADDR_LEN];/* MAC address 		*/

	uint8	addrLen;	/* Hardware address length	      	*/
	uint16	mtu;	    	/* Maximum transmission unit (payload)  */

	uint32	errors;		/* Number of Ethernet errors 		*/
	sid32	isem;		/* Semaphore for Ethernet input		*/
	sid32	osem; 		/* Semaphore for Ethernet output	*/
	uint16	istart;		/* Index of next packet in the ring     */

	int16	inPool;		/* Buffer pool ID for input buffers 	*/
	int16	outPool;	/* Buffer pool ID for output buffers	*/

	int16 	proms; 		/* nonzero => promiscuous mode 		*/

	int16 	ed_mcset;       /* nonzero => multicast reception set   */
	int16 	ed_mcc;	 	/* count of multicast addresses		*/
    	Eaddr   ed_mca[ETH_NUM_MCAST];/* array of multicast addrs 	*/
};

extern	struct	ethcblk	ethertab[];	/* array of control blocks      */
