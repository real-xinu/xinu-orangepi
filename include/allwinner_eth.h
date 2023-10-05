/* allwinner_eth.h - Ethernet device definitions for Allwinner SoC */

	/*register name			offset	description*/
struct __attribute__((packed)) eth_aw_csreg {
	volatile reg32 basic_ctl_0;	/* 0x00	Basic control 0 */
	volatile reg32 basic_ctl_1;	/* 0x04	Basic control 1 */
	volatile reg32 int_sta;			/* 0x08	Interrupt status */
	volatile reg32 int_en;				/* 0x0C	Interrupt enable */
	volatile reg32 tx_ctl_0;			/* 0x10	Transmit control 0 */
	volatile reg32 tx_ctl_1;			/* 0x14	Transmit control 1 */
	volatile byte res0[4];
	volatile reg32 tx_flow_ctl;	/* 0x1C	Transmit flow control */
	volatile reg32 tx_dma_desc_list;	/* 0x20	Transmit descriptor list address */
	volatile reg32 rx_ctl_0;			/* 0x24	Receive control 0 */
	volatile reg32 rx_ctl_1;			/* 0x28	Receive control 1 */
	volatile byte res1[8];
	volatile reg32 rx_dma_desc_list;	/* 0x34	Receive descriptor list address */
	volatile reg32 rx_frm_flt;		/* 0x38	Receive frame filter */
	volatile byte res2[4];
	volatile reg32 rx_hash_0;		/* 0x40	Hash table 0 */
	volatile reg32 rx_hash_1;		/* 0x44	Hash table 1 */
	volatile reg32 mii_cmd;			/* 0x48	Management interface command */
	volatile reg32 mii_data;			/* 0x4C	Management interface data */
	struct {
	    volatile unsigned int hi;		/* 50 */
	    volatile unsigned int lo;		/* 54 */
	} mac_addr[8];
	volatile byte res3[32];
	volatile reg32 tx_dma_sta;	/* 0xB0	Transmit dma status */
	volatile reg32 tx_cur_desc;	/* 0xB4	Current transmit descriptor */
	volatile reg32 tx_cur_buf;	/* 0xB8	Current transmit buffer address */
	volatile byte res4[4];
	volatile reg32 rx_dma_sta;	/* 0xC0	Receive dma status */
	volatile reg32 rx_cur_desc;	/* 0xC4	Current receive descriptor */
	volatile reg32 rx_cur_buf;	/* 0xC8	Current receive buffer address */
	volatile byte res5[4];
	volatile reg32 rgmii_sta;	/* 0xD0	RGMII status register */
};

/*	Name			Value		Description */
/* basic_ctl_0 signals */

#define ETH_AW_CTL_100MB	0x0000000C	/* Set for 100 Mbps */
#define ETH_AW_CTL_10MB		0x00000008	/* Set for 10 Mbps */
#define ETH_AW_CTL_LOOPBACK	0x00000002	/* Off by default */
#define	ETH_AW_CTL_DUPLEX	0x00000001	/* Set for full duplex */

/* basic_ctl_1 signals */

#define ETH_AW_BURST_LEN	0x0000003F	/* Mask for BURST_LEN */
#define ETH_AW_RX_TX_PRI	0x00000002	/* Set to prioritize RX */
#define ETH_AW_SOFT_RST		0x00000001	/* Soft reset */

/* int_sta signals (off or 0 by default) */

#define ETH_AW_RGMII_LINK_STA_INT	\
				0x00010000	/* Link status of RGMII changed */
#define ETH_AW_RX_EARLY_INT	0x00002000	/* RX DMA filled first buffer */
#define ETH_AW_RX_OVERFLOW_INT	0x00001000	/* RX FIFO overflowed */
#define ETH_AW_RX_TIMEOUT_INT	0x00000800	/* Received frame is too long */
#define ETH_AW_RX_DMA_STOPPED_INT	\
				0x00000400	/* RX DMA FSM is stopped */
#define ETH_AW_RX_BUF_UA_INT	0x00000200	/* RX DMA is suspended */
#define ETH_AW_RX_INT		0x00000100	/* Frame reception is completed */
#define ETH_AW_TX_EARLY_INT	0x00000020	/* Frame is transmitted to FIFO */
#define ETH_AW_TX_UNDERFLOW_INT	0x00000010	/* TX FIFO underflowed */
#define ETH_AW_TX_TIMEOUT_INT	0x00000008	/* TX has timed out */
#define ETH_AW_TX_BUF_UA_INT	0x00000004	/* TX DMA is suspended */
#define ETH_AW_TX_DMA_STOPPED_INT	\
				0x00000002	/* TX DMA FSM is stopped */
#define ETH_AW_TX_INT		0x00000001	/* Frame completion */

/* int_en signals  (off or 0 by default) */

#define ETH_AW_RX_EARLY_INT_EN	0x00002000	/* Early rx interrupt enable */
#define ETH_AW_RX_OVERFLOW_INT_EN	\
				0x00001000	/* Overflow interrupts */
#define ETH_AW_RX_TIMEOUT_INT_EN	\
				0x00000800	/* Rx timeout interrupts */
#define ETH_AW_RX_DMA_STOPPED_INT_EN	\
				0x00000400	/* Rx DMA FSM stopped interrupts */
#define ETH_AW_RX_BUF_UA_INT_EN	0x00000200	/* Rx buffer unavailable interrupt */
#define ETH_AW_RX_INT_EN	0x00000100	/* Rx interrupts */
#define ETH_AW_TX_EARLY_INT_EN	0x00000020	/* Early tx interrupts */
#define ETH_AW_TX_UNDERFLOW_INT_EN	\
				0x00000010	/* Underflow interrupts */
#define ETH_AW_TX_TIMEOUT_INT_EN	\
				0x00000008	/* Tx timeout interrupts */
#define ETH_AW_TX_BUF_UA_INT_EN	0x00000004	/* Tx buffer available interrupt */
#define ETH_AW_TX_DMA_STOPPED_INT_EN	\
				0x00000002	/* Tx DMA FSM stopped interrupts */
#define ETH_AW_TX_INT_EN	0x00000001	/* Tx interrupts */

/* tx_ctl_0 signals */

#define ETH_AW_TX_EN		0x80000000	/* Toggle transmitter */
#define ETH_AW_TX_FRM_LEN_CTL	0X40000000	/* Tx frames <= 2048 bytes */

/* tx_ctl_1 signals */

#define ETH_AW_TX_DMA_START	0x80000000	/* Start the TX DMA FSM */
#define ETH_AW_TX_DMA_EN	0x40000000	/* Start TX DMA, clear to stop TX DMA after the current frame is transmitted */
#define ETH_AW_TX_TH_64		0x00000700	/* Clear for 64 */
#define ETH_AW_TX_TH_128	0x00000100	/* Set for 128 */
#define ETH_AW_TX_TH_192	0x00000200	/* Set for 192 */
#define ETH_AW_TX_TH_256	0x00000300	/* Set for 256 */
#define ETH_AW_TX_MD		0x00000002	/* Start tx when a full frame is read in TX DMA FIFO, otherwise tx starts after the data in TX DMA FIFO goes over the threshold. */
#define ETH_AW_FLUSH_TX_FIFO	0x00000001	/* Disable flushing the TX FIFO */

/* tx_flow_ctl signals */

#define ETH_AW_TX_FLOW_CTL_STA	0x80000000	/* Trasmit a pause frame */
#define ETH_AW_TX_PAUSE_FRM_SLOT	\
				0x00300000	/* Threshold of pause timer */
#define ETH_AW_PAUSE_TIME	0x000FFFF0	/* Pause time for control frame */
#define ETH_AW_ZQP_FRM_EN	0x00000002	/* Generate Zero-Quanta Pause control frame */
#define ETH_AW_TX_FLOW_CTL_EN	0x00000001	/* Toggle flow control operations */

/* TX_DMA_LIST accepts a 32 bit address of the transmit descriptor list. */

/* rx_ctl_0 signals */

#define ETH_AW_RX_EN		0x80000000	/* Toggle receiver */
#define ETH_AW_RX_FRM_LEN_CTL	0x40000000	/* Receive frames <= 16384 bytes */
#define ETH_AW_JUMBO_FRM_EN	0x20000000	/* Allow frames <= 9018 bytes */
#define ETH_AW_STRIP_FCS	0x10000000	/* Strip the Pad/FCS when length < 1500 */
#define ETH_AW_CHECK_CRC	0x08000000	/* Calculate CRC and IPV4 Header Checksum */
#define ETH_AW_RX_PAUSE_FRM_MD	0x00020000	/* See documentation */
#define ETH_AW_RX_FLOW_CTL_EN	0x00010000	/* See documentation */

/* rx_ctl_1 signals */

#define ETH_AW_RX_DMA_START	0x80000000	/* Start the RX DMA */
#define ETH_AW_RX_DMA_EN	0x40000000	/* Start the RX DMA, clear to stop the RX DMA after the current frame is received */
#define ETH_AW_RX_FIFO_FLOW_CTL	0x01000000	/* Rx flow control */
#define ETH_AW_RX_FLOW_CTL_TH_DEACT_1	\
				0x00C00000	/* Clear for 1 */
#define ETH_AW_RX_FLOW_CTL_TH_DEACT_2	\
				0x00400000	/* Set for 2 */
#define ETH_AW_RX_FLOW_CTL_TH_DEACT_3	\
				0x00800000	/* Set for 3 */
#define ETH_AW_RX_FLOW_CTL_TH_DEACT_4	\
				0x00C00000	/* Set for 4 */
#define ETH_AW_RX_FLOW_CTL_TH_ACT_1	\
				0x00300000	/* Clear for 1 */
#define ETH_AW_RX_FLOW_CTL_TH_ACT_2	\
				0x00100000	/* Set for 2 */
#define ETH_AW_RX_FLOW_CTL_TH_ACT_3	\
				0x00200000	/* Set for 3 */
#define ETH_AW_RX_FLOW_CTL_TH_ACT_4	\
				0x00300000	/* Set for 4 */
#define ETH_AW_RX_TH_64		0x00000030	/* Clear for 64 */
#define ETH_AW_RX_TH_32		0x00000010	/* Set for 32 */
#define ETH_AW_RX_TH_96		0x00000020	/* Set for 96 */
#define ETH_AW_RX_TH_128	0x00000030	/* Set for 128 */
#define ETH_AW_RX_ERR_FRM	0x00000008	/* Forward frames with errors */
#define ETH_AW_RUNT_FRM		0x00000004	/* Forward undersized frames */
#define ETH_AW_RX_MD		0x00000002	/* See documentation */
#define ETH_AW_FLUSH_RX_FRM	0x00000001	/* Disable flushing frames when buffers are unavailable */

/* rx_dma_list accepts a 32 bit address of the receiver descriptor list. */

/* rx_frm_flt signals */

#define ETH_AW_DIS_ADDR_FILTER	0x80000000	/* Disable address filter */
#define ETH_AW_DIS_BROADCAST	0x00020000	/* Drop all broadcast frames */
#define ETH_AW_RX_ALL_MULTICAST	0x00010000	/* Receive all multicast frames */
#define ETH_AW_CTL_FRM_FILTER_DROP	\
				0x00003000	/* Clear to drop control frames */
#define ETH_AW_CTL_FRM_FILTER_RECEIVE	\
				0x00002000	/* Receive all control frames */
#define ETH_AW_CTL_FRM_FILTER_PASS	\
				0x00003000	/* Receive control frames that pass the address filter*/
#define ETH_AW_HASH_MULTICAST	0x00000200	/* Filter by hash table  */
#define ETH_AW_HASH_UNICAST	0x00000100	/* Filter by hash table */
#define ETH_AW_SA_FILTER_EN	0x00000080	/* Update SA filter result */
#define ETH_AW_SA_INV_FILTER	0x00000040	/* Inverse filtering */
#define ETH_AW_DA_INV_FILTER	0x00000020	/* Inverse filtering */
#define ETH_AW_FLT_MD		0x00000002	/* Receive frame when it passes the filter */
#define ETH_AW_RX_ALL		0x00000001	/* Receive all frames */

/* rx_hash_0 holds the upper 32 bits of the hash table for the receive frame filter */

/* rx_hash_1 holds the lower 32 bits of the hash table for the receive frame filter */

/* mii_cmd signals */
#define ETH_AW_MDC_DIV_RATIO_M_16	\
				0x00700000	/* Clear for 16 */
#define ETH_AW_MDC_DIV_RATIO_M_32	\
				0x00100000	/* Set for 32 */
#define ETH_AW_MDC_DIV_RATIO_M_64	\
				0x00200000	/* Set for 64 */
#define ETH_AW_MDC_DIV_RATIO_M_128	\
				0x00300000	/* Set for 128 */
/* bits 12-16 are for selecting a PHY device */
#define ETH_AW_MII_ADDR		12
#define ETH_AW_MII_REG		4
#define ETH_AW_PHY_ADDR		1
/* bits 4-8 are for selecting a register in that PHY device */
#define ETH_AW_MII_WR			0x00000002	/* Reading or Write PHY register */
#define ETH_AW_MII_BUSY		0x00000001	/* R/W in progress */

/* mii_data holds data in bits 0-15 that can be written or read from the selected PHY register */
#define ETH_AW_MII_DATA		0x0000ffff	/* Use to read and write data */

/* addrX_high holds the upper 16 bits of the Xth mac address in bits 0-15 */
/* addrX_low holds the lower 32 bits of the Xth mac address */

/* tx_dma_sta signals */
#define ETH_AW_TX_DMA_STA	0x00000007	/* Receive value */

/* tx_dma_cur_desc holds the address of the current descriptor */

/* tx_dma_cur_buf holds the address of the current buffer */

/* rx_dma_sta signals */
#define ETH_AW_RX_DMA_STA	0x00000007	/* Receive value */

/* rx_dma_cur_desc holds the address of the current descriptor */

/* rx_dma_cur_buf holds the address of the current buffer */

/* rgmii_sta signals */
#define ETH_AW_RGMII_LINK	0x00000008	/* Indicates link status */
#define ETH_AW_RGMII_LINK_SP	0x00000006	/* Receive link speed */
#define ETH_AW_RGMII_LINK_MD	0x00000001	/* Indicates link mode */

/* Receiving buffer description structure */
struct eth_aw_rx_desc {
	volatile uint32 status;
	volatile uint32 buf_len;
	volatile uint32 buf_addr;
	struct eth_aw_rx_desc * next;
};

/* rx status signals */
#define ETH_AW_RX_DESC_CTL	0x80000000	/* Usable by DMA */
#define ETH_AW_RX_DAF_FAIL	0x40000000	/* Frame failed DA filter */
#define ETH_AW_RX_FRM_LEN	0x3FFF0000	/* Frame length */
#define ETH_AW_RX_NO_ENOUGH_BUF_ERR	\
				0x00004000	/* Not enough buffer for the frame */
#define ETH_AW_RX_SAF_FAIL	0x00002000	/* Frame failed SA filter */
#define ETH_AW_RX_OVERFLOW_ERR	0x00000800	/* Buffer overflow occurred */
#define ETH_AW_RX_FIR_DESC	0x00000200	/* First descriptor */
#define ETH_AW_RX_LAST_DESC	0x00000100	/* Last descriptor */
#define ETH_AW_RX_HEADER_ERR	0x00000080	/* Wrong checksum */
#define ETH_AW_RX_COL_ERR	0x00000040	/* Late collision */
#define ETH_AW_RX_LENGTH_ERR	0x00000010	/* Wrong frame length */
#define ETH_AW_RX_PHY_ERR	0x00000008	/* PHY sent an error signal */
#define ETH_AW_RX_CRC_ERR	0x00000002	/* CRC of received frame is wrong */
#define ETH_AW_RX_PAYLOAD_ERR	0x00000001	/* Checksum or length of payload is wrong */

/* rx size description signals */
#define ETH_AW_RX_INT_CTL	0x80000000	/* See documentation */
#define ETH_AW_RX_BUF_SIZE	0x000007FF	/* Buffer size */

/* rx buf_addr holds the address of the current descriptor */

/* rx next holds the address of the next descriptor, 32 bit aligned */

/* Transmitting buffer description structure */
struct eth_aw_tx_desc {
	volatile uint32 status;
	volatile uint32 buf_len;
	volatile uint32 buf_addr;
	struct eth_aw_tx_desc * next;
};

/* tx status signals */
#define ETH_AW_TX_DESC_CTL	0x80000000	/* Usable by DMA */
#define ETH_AW_TX_HEADER_ERR	0x00010000	/* Wrong checksum */
#define ETH_AW_TX_LENGTH_ERR	0x00004000	/* Wrong frame length */
#define ETH_AW_TX_PAYLOAD_ERR	0x00001000	/* Checksum or length of payload is wrong */
#define ETH_AW_TX_CRS_ERR	0x00000400	/* Carrier is lost during transmission */
#define ETH_AW_TX_COL_ERR_0	0x00000200	/* See documentation */
#define ETH_AW_TX_COL_ERR_1	0x00000100	/* See documentation */
#define ETH_AW_TX_COL_CNT	0x00000078	/* Number of collisions before transmission */
#define ETH_AW_TX_DEFER_ERR	0x00000004	/* Abort frame due to deferal */
#define ETH_AW_TX_UNDERFLOW_ERR	0x00000002	/* Abort frame due to TX FIFO underflow */
#define ETH_AW_TX_DEFER		0x00000001	/* In half duplex, EMAC defers to transmission */

/* tx buf_len signals */
#define ETH_AW_TX_INT_CTL	0x80000000	/* See documentation */
#define ETH_AW_TX_LAST_DESC	0X40000000	/* Last descriptor */
#define ETH_AW_TX_FIR_DESC	0x20000000	/* First descriptor */
#define ETH_AW_CHECKSUM_CTL	0x18000000	/* Insert checksums in transmit frame */
#define ETH_AW_CRC_CTL		0x04000000	/* Don't transmit CRC field */
#define ETH_AW_TX_BUF_SIZE	0x000007FF	/* Buffer size */

/* tx buf_adr holds the address of the current descriptor */

/* rx next holds the address of the next descriptor, 32 bit aligned */

#define ETH_AW_INIT_DELAY	1000000

#define ETH_AW_RXINT		41
#define ETH_AW_TXINT		42


/* System configuration for EMAC based on Tom Trebisky's Kyu driver - https://github.com/trebisky/Kyu/blob/master/orange_pi/emac_phy.c#L534 */

#define EMAC_SYSCON	((unsigned int *) 0x01c00030)

/* Syscon is peculiar in that it is essentially dedicated to the EMAC
 * It has this one register for controlling EMAC stuff,
 *   along with a chip version register.
 * This single 32 bit register controlls EMAC phy mux and such.
 */
/* These are only some of the bit definitions, but more than we need */

#define SYSCON_CLK24		0x40000		/* set for 24 Mhz clock (else 25) */
#define SYSCON_LEDPOL		0x20000		/* set for LED active low polarity */
#define SYSCON_SHUTDOWN		0x10000		/* set to power down PHY */
#define SYSCON_EPHY_INTERNAL	0x08000		/* set to use internal PHY */

#define SYSCON_RXINV		0x10		/* set to invert Rx Clock */
#define SYSCON_TXINV		0x8		/* set to invert Tx Clock */
#define SYSCON_PIT		0x4		/* PHYS type, set for RGMII, else MII */

/* TCS - Transmit Clock Source */
#define SYSCON_TCS_MII		0		/* for MII (what we want) */
#define SYSCON_TCS_EXT		1		/* External for GMII or RGMII */
#define SYSCON_TCS_INT		2		/* Internal for GMII or RGMII */
#define SYSCON_TCS_INVALID	3		/* invalid */

#define CTL1_BURST_8		0x08000000;	/* DMA burst len = 8 (29:24)  */


/* -- bits in the ctl0 register */

#define	CTL_FULL_DUPLEX	0x0001
#define	CTL_LOOPBACK	0x0002
#define	CTL_SPEED_1000	0x0000
#define	CTL_SPEED_100	0x000C
#define	CTL_SPEED_10	0x0008

/* -- bits in the ctl1 register */

#define CTL1_SOFT_RESET		0x01
#define CTL1_RX_TX		0x02		/* Rx DMA has prio over Tx when set */
#define CTL1_BURST_8		0x08000000;	/* DMA burst len = 8 (29:24)  */

/* bits in the int_ena and int_stat registers */
#define INT_TX			0x0001
#define INT_TX_DMA_STOP		0x0002
#define INT_TX_BUF_AVAIL	0x0004
#define INT_TX_TIMEOUT		0x0008
#define INT_TX_UNDERFLOW	0x0010
#define INT_TX_EARLY		0x0020
#define INT_TX_ALL	(INT_TX | INT_TX_BUF_AVAIL | INT_TX_DMA_STOP | INT_TX_TIMEOUT | INT_TX_UNDERFLOW )
#define INT_TX_MASK		0x00ff

#define INT_RX			0x0100
#define INT_RX_NOBUF		0x0200
#define INT_RX_DMA_STOP		0x0400
#define INT_RX_TIMEOUT		0x0800
#define INT_RX_OVERFLOW		0x1000
#define INT_RX_EARLY		0x2000
#define INT_RX_ALL	(INT_RX | INT_RX_NOBUF | INT_RX_DMA_STOP | INT_RX_TIMEOUT | INT_RX_OVERFLOW )
#define INT_RX_MASK		0xff00

#define	INT_MII			0x10000		/* only in status */

/* bits in the Rx ctrl0 register */
#define	RX_EN			0x80000000
#define	RX_FRAME_LEN_CTL	0x40000000
#define	RX_JUMBO		0x20000000
#define	RX_STRIP_FCS		0x10000000
#define	RX_CHECK_CRC		0x08000000

#define	RX_PAUSE_MD		0x00020000
#define	RX_FLOW_CTL_ENA		0x08010000

/* bits in the Rx ctrl1 register */
#define	RX_DMA_START		0x80000000
#define	RX_DMA_ENA		0x40000000
#define	RX_MD			0x00000002
#define	RX_NO_FLUSH		0x00000001

/* bits in the Tx ctrl0 register */
#define	TX_EN			0x80000000
#define	TX_FRAME_LEN_CTL	0x40000000

/* bits in the Tx ctrl1 register */
#define	TX_DMA_START		0x80000000
#define	TX_DMA_ENA		0x40000000
#define	TX_MD			0x00000002

/* Bits in the Rx filter register */
#define	RX_FILT_DIS		0x80000000
#define	RX_DROP_BROAD		0x00020000
#define	RX_ALL_MULTI		0x00010000

#define EMAC_BASE	((struct eth_aw_csreg *) 0x01c30000)
#define IRQ_EMAC	114	/* Ethernet */

struct emac_desc {
	volatile unsigned long status;
	volatile long size;
	volatile char * buf;
	struct emac_desc *next;
}	__aligned(ARM_DMA_ALIGN);

/* status bits */
#define DS_ACTIVE	0x80000000	/* set when available for DMA */
#define DS_DS_FAIL	0x40000000	/* Rx DAF fail */
#define DS_CLIP		0x00004000	/* Rx clipped (buffer too small) */
#define DS_SA_FAIL	0x00002000	/* Rx SAF fail */
#define DS_OVERFLOW	0x00000800	/* Rx overflow */
#define DS_FIRST	0x00000200	/* Rx first in list */
#define DS_LAST		0x00000100	/* Rx last in list */
#define DS_HEADER_ERR	0x00000080	/* Rx header error */
#define DS_COLLISION	0x00000040	/* Rx late collision in half duplex */
#define DS_LENGTH_ERR	0x00000010	/* Rx length is wrong */
#define DS_PHY_ERR	0x00000008	/* Rx error from Phy */
#define DS_CRC_ERR	0x00000002	/* Rx error CRC wrong */
#define DS_PAYLOAD_ERR	0x00000001	/* Rx error payload checksum or length wrong */

#define DS_TX_HEADER_ERR	0x00010000	/* Tx header error */
#define DS_TX_LENGTH_ERR	0x00004000	/* Tx length error */
#define DS_TX_PAYLOAD		0x00001000	/* Tx payload checksum wrong */
#define DS_CARRIER		0x00000400	/* Tx lost carrier */
#define DS_COL1			0x00000200	/* Tx collision */
#define DS_COL2			0x00000100	/* Tx too many collisions */
/* collision count in these bits */
#define DS_DEFER_ERR		0x00000004	/* Tx defer error (too many) */
#define DS_UNDERFLOW		0x00000002	/* Tx fifo underflow */
#define DS_DEFER		0x00000001	/* Tx defer this frame (half duplex) */

/* Bits in the Tx size descriptor */
#define DS_TX_INT		0x80000000	/* Set TX_INT when finished */
#define DS_TX_LAST		0x40000000	/* This is the last buffer in a packet */
#define DS_TX_FIRST		0x20000000	/* This is the first buffer in a packet */
#define	DS_TX_EOR		0x02000000	/* End of Ring */
#define	DS_TX_ADR_CHAIN		0x01000000	/* was magic for U-Boot */

#define emac_cache_flush(a,b )		flush_dcache_range ( a, b )
#define emac_cache_invalidate(a,b)	invalidate_dcache_range ( a, b )
