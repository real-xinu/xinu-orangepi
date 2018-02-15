/* allwinner_eth.h - Ethernet device definitions for Allwinner SoC */

	/*register name		 offset	description*/
struct eth_aw_csreg {
	reg32 basic_ctl_0;	/* 0x00	basic control 0 */
	reg32 basic_ctl_1;	/* 0x04	basic control 1 */
	reg32 int_sta;		/* 0x08	interrupt status */
	reg32 int_en;		/* 0x0C	interrupt enable */
	reg32 tx_ctl_0;		/* 0x10	transmit control 0 */
	reg32 tx_ctl_1;		/* 0x14	transmit control 1 */
	byte res0[8];
	reg32 tx_flow_ctl;	/* 0x1C	transmit flow control */
	reg32 tx_dma_desc_list;	/* 0x20	transmit descriptor list address */
	reg32 rx_ctl_0;		/* 0x24	receive control 0 */
	reg32 rx_ctl_1;		/* 0x28	receive control 1 */
	byte res1[12];
	reg32 rx_dma_desc_list;	/* 0x34	receive descriptor list address */
	reg32 rx_frm_flt;	/* 0x38	receive frame filter */
	byte res2[8];
	reg32 rx_hash_0;	/* 0x40	hash table 0 */
	reg32 rx_hash_1;	/* 0x44	hash table 1 */
	reg32 mii_cmd;		/* 0x48	management interface command */
	reg32 mii_data;		/* 0x4C	management interface data */
	reg32 addr_high_0;	/* 0x50	MAC address high 0 */
	reg32 addr_low_0;	/* 0x54	MAC address low 0 */
	reg32 addr_high_1;	/* 0x58	MAC address high 1 */
	reg32 addr_low_1;	/* 0x5C	MAC address low 1 */
	reg32 addr_high_2;	/* 0x60	MAC address high 2 */
	reg32 addr_low_2;	/* 0x64	MAC address low 2 */
	reg32 addr_high_3;	/* 0x68	MAC address high 3 */
	reg32 addr_low_3;	/* 0x6C	MAC address low 3 */
	reg32 addr_high_4;	/* 0x70	MAC address high 4 */
	reg32 addr_low_4;	/* 0x74	MAC address low 4 */
	reg32 addr_high_5;	/* 0x78	MAC address high 5 */
	reg32 addr_low_5;	/* 0x7C	MAC address low 5 */
	reg32 addr_high_6;	/* 0x80	MAC address high 6 */
	reg32 addr_low_6;	/* 0x84	MAC address low 6 */
	reg32 addr_high_7;	/* 0x88	MAC address high 7 */
	reg32 addr_low_7;	/* 0x8C	MAC address low 7 */
	byte res3[36];
	reg32 tx_dma_sta;	/* 0xB0	transmit dma status */
	reg32 tx_cur_desc;	/* 0xB4	current transmit descriptor */
	reg32 tx_cur_buf;	/* 0xB8	current transmit buffer address */
	byte res4[8];
	reg32 rx_dma_sta;	/* 0xC0	receive dma status */
	reg32 rx_cur_desc;	/* 0xC4	current receive descriptor */
	reg32 rx_cur_buf;	/* 0xC8	current receive buffer address */
	byte res5[8];
	reg32 rgmii_sta;	/* 0xD0	rgmii status register */
};

	/* name			value		description	set: '|' clear: '&~' */
/* basic_ctl_0 signals */

#define ETH_AW_CTL_100MB	0x0000000C	/* set for 100 Mbps speed, clear for Gbps speed */
#define ETH_AW_CTL_10MB		0x00000008	/* set for 10 Mbps speed */
#define ETH_AW_CTL_LOOPBACK	0x00000002	/* clear to disable, set to enable */
#define	ETH_AW_CTL_DUPLEX	0x00000001	/* clear for half duplex, set for full duplex */

/* basic_ctl_1 signals */

/* BURST_LEN is acquired by right shifting BASIC_CTL_1 24 using & with this mask: */
#define ETH_AW_BURST_LEN	0x0000003F	/* mask for BURST_LEN */
#define ETH_AW_RX_TX_PRI	0x00000002	/* set to prioritize RX, clear for equal priority */
#define ETH_AW_SOFT_RST		0x00000001	/* set to soft reset all registers and logic */
						/* true = 'register & signal' */
/* int_sta signals (off or 0 by default) */

#define ETH_AW_RGMII_LINK_STA_INT
				0x00010000	/* true if link status of RGMII has changed */
#define ETH_AW_RX_EARLY_INT	0x00002000	/* true if RX DMA has filled the first buffer */
#define ETH_AW_RX_OVERFLOW_INT	0x00001000	/* true if RX FIFO overflowed */
#define ETH_AW_RX_TIMEOUT_INT	0x00000800	/* true if the received frame is too long */
#define ETH_AW_RX_DMA_STOPPED_INT
				0x00000400	/* true if RX DMA FSM is stopped */
#define ETH_AW_RX_BUF_UA_INT	0x00000200	/* true if RX DMA is suspended */
#define ETH_AW_RX_INT		0x00000100	/* true if frame reception is completed */
#define ETH_AW_TX_EARLY_INT	0x00000020	/* true if frame is transmitted to FIFO */
#define ETH_AW_TX_UNDERFLOW_INT	0x00000010	/* true if TX FIFO underflowed */
#define ETH_AW_TX_TIMEOUT_INT	0x00000008	/* true if TX has timed out */
#define ETH_AW_TX_BUF_UA_INT	0x00000004	/* true if TX DMA is suspended */
#define ETH_AW_TX_DMA_STOPPED_INT
				0x00000002	/* true if TX DMA FSM is stopped */
#define ETH_AW_TX_INT		0x00000001	/* true for frame completion */

/* int_en signals  (off or 0 by default) */

#define ETH_AW_RX_EARLY_INT_EN	0x00002000	/* toggle early receive interrupt enable */
#define ETH_AW_RX_OVERFLOW_INT_EN
				0x00001000	/* toggle overflow interrupts */
#define ETH_AW_RX_TIMEOUT_INT_EN
				0x00000800	/* toggle receive timeout interrupts */
#define ETH_AW_RX_DMA_STOPPED_INT_EN
				0x00000400	/* toggle receive DMA FSM stopped interrupts */
#define ETH_AW_RX_BUF_UA_INT_EN	0x00000200	/* toggle receive buffer unavailable interrupts */
#define ETH_AW_RX_INT_EN	0x00000100	/* toggle receive interrupts */
#define ETH_AW_TX_EARLY_INT_EN	0x00000020	/* toggle early transmit interrupts */
#define ETH_AW_TX_UNDERFLOW_INT_EN
				0x00000010	/* toggle underflow interrupts */
#define ETH_AW_TX_TIMEOUT_INT_EN
				0x00000008	/* toggle transmit timeout interrupts */
#define ETH_AW_TX_BUF_UA_INT_EN	0x00000004	/* toggle transmit buffer available interrupts */
#define ETH_AW_TX_DMA_STOPPED_INT_EN
				0x00000002	/* toggle transmit DMA FSM stopped interrupts */
#define ETH_AW_TX_INT_EN	0x00000001	/* toggle transmit interrupts */

/* tx_ctl_0 signals */

#define ETH_AW_TX_EN		0x80000000	/* toggle transmitter */
#define ETH_AW_TX_FRM_LEN_CTL	0X40000000	/* transmit frames of 2048 bytes or less */

/* tx_ctl_1 signals */

#define ETH_AW_TX_DMA_START	0x80000000	/* set to start the TX DMA FSM */
#define ETH_AW_TX_DMA_EN	0x40000000	/* set to start and run TX DMA, clear to stop TX DMA after the current frame is transmitted */

/*Each of the following is a mask for ETH_AW_TX_TH to set the threshold value. Do not set 64. */
#define ETH_AW_TX_TH_64		0x00000700	/* clear for 64, clear before setting others */
#define ETH_AW_TX_TH_128	0x00000100	/* set for 128 */
#define ETH_AW_TX_TH_192	0x00000200	/* set for 192 */
#define ETH_AW_TX_TH_256	0x00000300	/* set for 256 */
/* End threshold values. */
#define ETH_AW_TX_MD		0x00000002	/* set for transmission to start when a full frame is read in TX DMA FIFO */
						/* clear for transmission to start after the data in TX DMA FIFO goes over the threshold. */
#define ETH_AW_FLUSH_TX_FIFO	0x00000001	/* set to disable flushing the TX FIFO, clear to enable */

/* tx_flow_ctl signals */

#define ETH_AW_TX_FLOW_CTL_STA	0x80000000	/* true for a pause frame transmission is in progress */
/* set to transmit a pause frame in full-duplex mode or activate the backpressure function */
/* must be false before writing to this register again */

/* PAUSE_FRM_SLOT is acquired by using & with this mask then right shifting 20: */
#define ETH_AW_TX_PAUSE_FRM_SLOT
				0x00300000	/* threshold of pause timer, should be less than PAUSE_TIME */
#define ETH_AW_PAUSE_TIME	0x000FFFF0	/* mask used to read the pause time in the control frame */
#define ETH_AW_ZQP_FRM_EN	0x00000002	/* toggle functionality to generate Zero-Quanta Pause control frame */
#define ETH_AW_TX_FLOW_CTL_EN	0x00000001	/* toggle flow control operations */

/* TX_DMA_LIST is a register that accepts a 32 bit address of the transmit descriptor list. */

/* rx_ctl_0 signals */

#define ETH_AW_RX_EN		0x80000000	/* toggle receiver */
#define ETH_AW_RX_FRM_LEN_CTL	0x40000000	/* set to receive frames up to 16384 bytes, clear to limit to 2048 or 10240 if JUMBO_FRM_EN is set */
#define ETH_AW_JUMBO_FRM_EN	0x20000000	/* set to allow jumbo frames up to 9018 bytes */
#define ETH_AW_STRIP_FCS	0x10000000	/* set to strip the Pad/FCS field when length < 1500 */
#define ETH_AW_CHECK_CRC	0x08000000	/* set to calculate CRC and IPV4 Header Checksum */
#define ETH_AW_RX_PAUSE_FRM_MD	0x00020000	/* see documentation */
#define ETH_AW_RX_FLOW_CTL_EN	0x00010000	/* see documentation */

/* rx_ctl_1 signals */

#define ETH_AW_RX_DMA_START	0x80000000	/* set to start the RX DMA FSM */
#define ETH_AW_RX_DMA_EN	0x40000000	/* set to start and run the RX DMA, clear to stop the RX DMA after the current frame is received */
#define ETH_AW_RX_FIFO_FLOW_CTL	0x01000000	/* toggle rx flow control */
#define ETH_AW_RX_FLOW_CTL_TH_DEACT_1
				0x00C00000	/* clear for 1 */
#define ETH_AW_RX_FLOW_CTL_TH_DEACT_2
				0x00400000	/* set for 2 */
#define ETH_AW_RX_FLOW_CTL_TH_DEACT_3
				0x00800000	/* set for 3 */
#define ETH_AW_RX_FLOW_CTL_TH_DEACT_4
				0x00C00000	/* set for 4 */
#define ETH_AW_RX_FLOW_CTL_TH_ACT_1
				0x00300000	/* clear for 1 */
#define ETH_AW_RX_FLOW_CTL_TH_ACT_2
				0x00100000	/* set for 2 */
#define ETH_AW_RX_FLOW_CTL_TH_ACT_3
				0x00200000	/* set for 3 */
#define ETH_AW_RX_FLOW_CTL_TH_ACT_4
				0x00300000	/* set for 4 */
#define ETH_AW_RX_TH_64		0x00000030	/* clear for 64 */
#define ETH_AW_RX_TH_32		0x00000010	/* set for 32 */
#define ETH_AW_RX_TH_96		0x00000020	/* set for 96 */
#define ETH_AW_RX_TH_128	0x00000030	/* set for 128 */
#define ETH_AW_RX_ERR_FRM	0x00000008	/* set to forward frames with error */
#define ETH_AW_RUNT_FRM		0x00000004	/* set to forward undersized frames with no error */
#define ETH_AW_RX_MD		0x00000002	/* see documentation */
#define ETH_AW_FLUSH_RX_FRM	0x00000001	/* set to disable flushing frames when buffers are unavailable */

/* rx_dma_list is a register that accepts a 32 bit address of the receiver descriptor list. */

/* rx_frm_flt signals */

#define ETH_AW_DIS_ADDR_FILTER	0x80000000	/* toggle to disable address filter */
#define ETH_AW_DIS_BROADCAST	0x00020000	/* toggle to drop all broadcast frames */
#define ETH_AW_RX_ALL_MULTICAST	0x00010000	/* toggle to receive all multicast frames */
#define ETH_AW_CTL_FRM_FILTER_DROP
				0x00003000	/* clear to drop all control frames */
#define ETH_AW_CTL_FRM_FILTER_RECEIVE
				0x00002000	/* set to receive all control frames */
#define ETH_AW_CTL_FRM_FILTER_PASS
				0x00003000	/* set to receive control frames that pass the address filter*/
#define ETH_AW_HASH_MULTICAST	0x00000200	/* see documentation */
#define ETH_AW_HASH_UNICAST	0x00000100	/* '' */
#define ETH_AW_SA_FILTER_EN	0x00000080	/* '' */
#define ETH_AW_SA_INV_FILTER	0x00000040	/* '' */
#define ETH_AW_DA_INV_FILTER	0x00000020	/* '' */
#define ETH_AW_FLT_MD		0x00000002	/* '' */
#define ETH_AW_RX_ALL		0x00000001	/* '' */

/* rx_hash_0 is a register that holds the upper 32 bits of the hash table for the receive frame filter */

/* rx_hash_1 is a register that holds the lower 32 bits of the hash table for the receive frame filter */

/* mii_cmd signals */
#define ETH_AW_MDC_DIV_RATIO_M_16
				0x00700000	/* clear for 16 */
#define ETH_AW_MDC_DIV_RATIO_M_32
				0x00100000	/* set for 32 */
#define ETH_AW_MDC_DIV_RATIO_M_64
				0x00200000	/* set for 64 */
#define ETH_AW_MDC_DIV_RATIO_M_128
				0x00300000	/* set for 128 */
/* bits 12-16 are for selecting a PHY device */
/* bits 4-8 are for selecting a register in that PHY device */
#define ETH_AW_MII_WR		0x00000002	/* switch between reading and writing in PHY register */
#define ETH_AW_MII_BUSY		0x00000001	/* true for w/r in progress */

/* mii_data holds data in bits 0-15 that can be written or read from the selected PHY register */

/* addrX_high holds the upper 16 bits of the Xth mac address in bits 0-15 */
/* addrX_low holds the lower 32 bits of the Xth mac address */

/* tx_dma_sta signals */
#define ETH_AW_TX_DMA_STA	0x00000007	/* & to receive value */

/* tx_dma_cur_desc is a register that holds the address of the current descriptor */

/* tx_dma_cur_buf is a register that holds the address of the current buffer */

/* rx_dma_sta signals */
#define ETH_AW_RX_DMA_STA	0x00000007	/* & to receive value */

/* rx_dma_cur_desc is a register that holds the address of the current descriptor */

/* rx_dma_cur_buf is a register that holds the address of the current descriptor */

/* rgmii_sta signals */
#define ETH_AW_RGMII_LINK	0x00000008	/* indicates link status */
#define ETH_AW_RGMII_LINK_SP	0x00000006	/* & to receive link speed */
#define ETH_AW_RGMII_LINK_MD	0x00000001	/* indicates linke mode */

/* Receiving buffer description structure */
struct eth_aw_rx_desc {
	uint32 status;
	uint32 buf_len;
	uint32 buf_addr;
	eth_aw_rx_desc * next;
};

/* rx status signals */
#define ETH_AW_RX_DESC_CTL	0x80000000	/* true for usable by DMA */
#define ETH_AW_RX_DAF_FAIL	0x40000000	/* true if frame failed DA filter */
#define ETH_AW_RX_FRM_LEN	0x3FFF0000	/* & and right shift 16 for 14 bit frame length */
#define ETH_AW_RX_NO_ENOUGH_BUF_ERR
				0x00004000	/* true for not enough buffer for the frame */
#define ETH_AW_RX_SAF_FAIL	0x00002000	/* true for frame failed SA filter */
#define ETH_AW_RX_OVERFLOW_ERR	0x00000800	/* buffer overflow error occurred */
#define ETH_AW_RX_FIR_DESC		0x00000200	/* true for first descriptor */
#define ETH_AW_RX_LAST_DESC	0x00000100	/* true for last descriptor */
#define ETH_AW_RX_HEADER_ERR	0x00000080	/* true for wrong checksum */
#define ETH_AW_RX_COL_ERR	0x00000040	/* true for late collision */
#define ETH_AW_RX_LENGTH_ERR	0x00000010	/* wrong frame length */
#define ETH_AW_RX_PHY_ERR	0x00000008	/* PHY sent an error signal */
#define ETH_AW_RX_CRC_ERR	0x00000002	/* true for CRC file of received frame is wrong */
#define ETH_AW_RX_PAYLOAD_ERR	0x00000001	/* checksum or length of payload is wrong */

/* rx size description signals */
#define ETH_AW_RX_INT_CTL	0x80000000	/* see documentation */
#define ETH_AW_RX_BUF_SIZE	0x000007FF	/* 11 bit buffer size */

/* rx buf_addr holds the address of the current descriptor */

/* rx next holds the address of the next descriptor, 32 bit aligned */

/* Transmitting buffer description structure */
struct eth_aw_tx_desc {
	uint32 status;
	uint32 buf_len;
	uint32 buf_addr;
	eth_aw_tx_desc * next;
};

/* tx status signals */
#define ETH_AW_TX_DESC_CTL	0x80000000	/* true for usable by DMA */
#define ETH_AW_TX_HEADER_ERR	0x00010000	/* true for wrong checksum */
#define ETH_AW_TX_LENGTH_ERR	0x00004000	/* true for wrong frame length */
#define ETH_AW_TX_PAYLOAD_ERR	0x00001000	/* checksum or length of payload is wrong */
#define ETH_AW_TX_CRS_ERR	0x00000400	/* carrier was lost during transmission */
#define ETH_AW_TX_COL_ERR_0	0x00000200	/* see documentation */
#define ETH_AW_TX_COL_ERR_1	0x00000100	/* see documentation */
#define ETH_AW_TX_COL_CNT	0x00000078	/* number of collisions before transmission */
#define ETH_AW_TX_DEFER_ERR	0x00000004	/* set to abort frame due to deferal */
#define ETH_AW_TX_UNDERFLOW_ERR	0x00000002	/* set to abort frame due to TX FIFO underflow */
#define ETH_AW_TX_DEFER		0x00000001	/* if in half duplex, EMAC defers to transmission */

/* tx buf_len signals */
#define ETH_AW_TX_INT_CTL	0x80000000	/* see documentation */
#define ETH_AW_TX_LAST_DESC	0X40000000	/* true for last descriptor */
#define ETH_AW_TX_FIR_DESC	0x20000000	/* true for first descriptor */
#define ETH_AW_CHECKSUM_CTL	0x18000000	/* use to insert checksums in transmit frame */
#define ETH_AW_CRC_CTL		0x04000000	/* set to not transmit CRC field */
#define ETH_AW_TX_BUF_SIZE	0x000007FF	/* 11 bit buffer size */

/* tx buf_adr holds the address of the current descriptor */

/* rx next holds the address of the next descriptor, 32 bit aligned */

#define ETH_ALLWINNER_INIT_DELAY	1000000
