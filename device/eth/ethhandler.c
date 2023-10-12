/* ethhandler.c - ethhandler */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethhandler - Orange Pi (Allwinner H3) Ethernet Interrupt Handler (WIP)
 *------------------------------------------------------------------------
 */
interrupt ethhandler (
		uint32	xnum	/* IRQ number	*/
	)
{
	kprintf("ethhandler\n");
	//TODO Comer says there may be an interrupt for every successful transmission/reception
	//Doesn't have to do anything until the end of the ring is reached
	//TODO Will need this to handle the case where the buffer is empty and a read is attempted, or if it is full and a write is attempted.
// 	kprintf("eh1\n");
// 	uint32	status;
// 	struct  dentry  *devptr;        /* address of device control blk*/
// 	struct 	ethcblk	*ethptr;	/* ptr to control block		*/
//
// 	/* Initialize structure pointers */
//
// 	devptr = (struct dentry *) &devtab[ETHER0];
//
// 	/* Obtain a pointer to the tty control block */
//
// 	ethptr = &ethertab[devptr->dvminor];
// // 	kprintf("eh2\n");
//
// 	//TODO Comer says there may be an interrupt for every successful transmission/reception
// 	//Doesn't have to do anything until the end of the ring is reached
// 	//TODO Will need this to handle the case where the buffer is empty and a read is attempted, or if it is full and a write is attempted.
// // 	kprintf("EMAC interrupt (ethhandler())\n");
// 	struct eth_aw_csreg *ep = EMAC_BASE;
// 	int stat;
// 	int statx;
// 	int stat2;
//
// // 	int_count++;
//
// 	stat = ep->int_sta;
// // 	last_stat = stat;
//
// // 	kprintf("eh3\n");
// 	// printf ( "emac interrupt --   status:%08x\n", stat );
//
// 	/* For now, we run this on each interrupt,
// 	 * which at the present time is just Rx Ints
// 	 * XXX someday do this in Tx Ints.
// 	 */
// 	// tx_cleaner ();
//
// 	statx = stat & INT_RX_MASK;
// 	kprintf("EMAC interrupt (ethhandler()): %d\n", stat);
//
// 	if ( statx && statx != INT_RX )
// 	    printf ( " *** unexpected emac Rx int status: %08x\n", stat );
//
// 	// ep->int_ena = INT_RX | INT_TX | INT_TX_UNDERFLOW;
// 	if ( stat & INT_RX ) {
// 		kprintf("RX Interrupt (ethhandler.c)\n");
// // 		eth_rxPackets(ethptr);
// // 	    rx_handler ( stat, ep->rx_dma_desc_list, ethptr );
// 	}
//
// 	if ( stat & INT_TX ) {
// 		kprintf("TX Interrupt (ethhandler.c)\n");
// 		//Print contents of tx ring
// 		kprintf("ethptr->txRing: %d\n", ethptr->txRing);
// // 		eth_txPackets(ethptr);
// // 	    tx_handler ( stat, ep->tx_dma_desc_list, ethptr );
// 	}
//
// 	if ( stat & INT_TX_UNDERFLOW ) {
// 	    printf ( " *** TX underflow interrupt !!\n" );
// 	}
//
// 	stat2 = ep->int_sta;
// 	// if ( stat2 != stat )
// 	//     printf ( "emac interrupt --  xstatus: %08x --> %08x\n", stat, stat2 );
//
// 	/* Ack the IRQ in the emac */
// 	/* experiment shows this to be necessary and correct */
// 	// ep->int_stat = ep->int_stat & 0xffff; (bad)
// 	// ep->int_stat &= stat;
// 	ep->int_sta = stat & 0x3fff;
	// TODO
//	struct	eth_a_csreg *csrptr;		/* Ethernet CSR pointer	*/
//	struct	eth_a_tx_desc *tdescptr;	/* Tx desc pointer	*/
//	struct	eth_a_rx_desc *rdescptr;	/* Rx desc pointer	*/
//	struct	ethcblk *ethptr = &ethertab[0];	/* Ethernet ctl blk ptr	*/
//
//	csrptr = (struct eth_a_csreg *)ethptr->csr;
//
//	if(xnum == ETH_AM335X_TXINT) {	/* Transmit interrupt */
//
//		/* Get pointer to first desc in queue	*/
//
//		tdescptr = (struct eth_a_tx_desc *)ethptr->txRing +
//							ethptr->txHead;
//
//		/* Defer scheduling until all descs are processed */
//
//		resched_cntl(DEFER_START);
//
//		while(semcount(ethptr->osem) < (int32)ethptr->txRingSize) {
//
//			/* If desc owned by DMA, check if we need to	*/
//			/* Restart the transmission			*/
//
//			if(tdescptr->stat & ETH_AM335X_TDS_OWN) {
//				if(csrptr->stateram->tx_hdp[0] == 0) {
//					csrptr->stateram->tx_hdp[0] =
//							(uint32)tdescptr;
//				}
//				break;
//			}
//
//			/* Acknowledge the interrupt	*/
//
//			csrptr->stateram->tx_cp[0] = (uint32)tdescptr;
//
//			/* Increment the head index of the queue	*/
//			/* And go to the next descriptor in queue	*/
//
//			ethptr->txHead++;
//			tdescptr++;
//			if(ethptr->txHead >= ethptr->txRingSize) {
//				ethptr->txHead = 0;
//				tdescptr = (struct eth_a_tx_desc *)
//		ethptr = &ethertab[devptr->dvminor];
//		//
//		//	switch (func) {
//		//
//		//		/* Get MAC address */
//		//
//		//		case ETH_CTRL_GET_MAC:
//		//			memcpy((byte *)arg1, ethptr->devAddress,
//		//					ETH_ADDR_LEN);
//		//			break;
//		//
//		//		default:
//		//			return SYSERR;
//		//	}					ethptr->txRing;
//			}
//
//			/* Signal the output semaphore */
//
//			signal(ethptr->osem);
//		}
//
//		/* Acknowledge the transmit interrupt */
//
//		csrptr->cpdma->eoi_vector = 0x2;
//
//		/* Resume rescheduling	*/
//
//		resched_cntl(DEFER_STOP);
//	}
//	else if(xnum == ETH_AM335X_RXINT) {	/* Receive interrupt */
//
//		/* Get the pointer to last desc in the queue	*/
//
//		rdescptr = (struct eth_a_rx_desc *)ethptr->rxRing +
//							ethptr->rxTail;
//
//		/* Defer scheduling until all descriptors are processed	*/
//
//		resched_cntl(DEFER_START);
//
//		while(semcount(ethptr->isem) < (int32)ethptr->rxRingSize) {
//
//			/* Check if we need to restart the DMA	*/
//
//			if(rdescptr->stat & ETH_AM335X_RDS_OWN) {
//				if(csrptr->stateram->rx_hdp[0] == 0) {
//					csrptr->stateram->rx_hdp[0] =
//							(uint32)rdescptr;
//				}
//				break;
//			}
//
//			/* Acknowledge the interrupt	*/
//
//			csrptr->stateram->rx_cp[0] = (uint32)rdescptr;
//
//			/* Increment the tail index of the queue	*/
//			/* And go to the next descriptor in the queue	*/
//
//			ethptr->rxTail++;
//			rdescptr++;
//			if(ethptr->rxTail >= ethptr->rxRingSize) {
//				ethptr->rxTail = 0;
//				rdescptr = (struct eth_a_rx_desc *)
//							ethptr->rxRing;
//			}
//
//			/* Signal the input semaphore	*/
//
//			signal(ethptr->isem);
//		}
//
//		/* Acknowledge the receive interrupt */
//
//		csrptr->cpdma->eoi_vector = 0x1;
//
//		/* Resume rescheduling	*/
//
//		resched_cntl(DEFER_STOP);
//	}
}
