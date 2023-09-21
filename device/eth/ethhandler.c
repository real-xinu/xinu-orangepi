/* ethhandler.c - ethhandler */

#include <xinu.h>

//TODO Design/Implement proper handling of rx/tx with rings
//TODO Convert netbuf to Xinu netbuf (netpacket?) format
static void rx_handler ( int stat, struct emac_desc *cur_rx_dma, struct ethcblk	*ethptr )
{
	struct netbuf *nbp;
	int len;
	int tag = ' ';

	printf ( "Rx interrupt, packet incoming (emac)\n" );
// 		et_rx ();

	// invalidate_dcache_range ( (void *) cur_rx_dma, &cur_rx_dma[1] );
// 		emac_cache_invalidate ( (void *) cur_rx_dma, &cur_rx_dma[1] );

	//TODO I think I'll do it like this: ethhandler just updates the pointers to the device's ring where the new info is stored, and ethread/write actually copy the packet data into a Xinu packet structure. This should eliminate the need for us to do allocation here.

	while ( ! (cur_rx_dma->status & DS_ACTIVE) ) {
// 		int i_dma = (cur_rx_dma - rx_list);

// 		len = (cur_rx_dma->status >> 16) & 0x3fff;
// 		// nbp = netbuf_alloc ();
// 		nbp = (struct netpacket *)getbuf(netbufpool);
// // 		nbp = netbuf_alloc_i ();
//
// 		if ( ! nbp )
// 			return;     /* drop packet */
//
// 		// pkt_arrive ();
//
// 		nbp->elen = len - 4;
// 		//Since the first 3 items of of struct netpacket should be identical to Tom's eth_hdr, this should be fine
// 		memcpy ( (char *) nbp, cur_rx_dma->buf, len - 4 );
//
// 		if ( last_capture ) {
// 			if ( last_len ) {
// 				prior_len = last_len;
// 				memcpy ( prior_buf, last_buf, last_len );
// 			}
// 			last_len = len - 4;
// 			memcpy ( last_buf, cur_rx_dma->buf, len - 4 );
// 		}

		// emac_show_packet ( tag, i_dma, nbp );
		// tag = '*';

		cur_rx_dma->status = DS_ACTIVE;

		// flush_dcache_range ( (void *) cur_rx_dma, &cur_rx_dma[1] );
// 		emac_cache_flush ( (void *) cur_rx_dma, &cur_rx_dma[1] );

		//TODO Update pointers to head/tail of ring so ethread/write know what to grab
// 		net_rcv ( nbp );
		ethptr->rxTail += sizeof(struct emac_desc);


		cur_rx_dma = cur_rx_dma->next;

		// invalidate_dcache_range ( (void *) cur_rx_dma, &cur_rx_dma[1] );
// 		emac_cache_invalidate ( (void *) cur_rx_dma, &cur_rx_dma[1] );
	}
}

/* Some notes on the cur and clean pointers ...
* cur always points to the next available slot
* clean always points after the last known clean slot.
* when cur == clean the list is empty, and that
*  is how we initialize things.
* when cur->next == clean the list is full.
*  (not really, there is still one slot, but we
*   cannot use it without making the list look empty).
*
* XXX - someday when we do transmits straight out of netbufs
*   this would be the place to release those netbufs.
*
* For some reason XXX at this time:
*  When I send a UDP packet, 1.08 milliseconds elapses
*  from the time the packet is queued and when we see the
*  interrupt here to indicate it is finished.
*/
// static void tx_cleaner ( struct emac_desc *cur_rx_dma )
// {
// 	if ( cur_tx_dma == clean_tx_dma )
// 		return;
//
// 	// tx_list_show ();
//
// 	while ( clean_tx_dma != cur_tx_dma ) {
// 		if ( clean_tx_dma->status & DS_ACTIVE)
// 		break;
// 		// printf ( "Tx clean: %08x %08x\n", clean_tx_dma->status, clean_tx_dma->size );
// 		clean_tx_dma = clean_tx_dma->next;
// 		// pkt_finish ();
// 	}
//
// 	// emac_busy = 0;
// }
//
// static void tx_handler ( int stat, struct emac_desc *cur_rx_dma )
// {
// 	// printf ( "Emac tx interrupt %08x\n", stat );
// 	// phy_show ();
// 	// emac_debug ();
// 	et_tx ();
// 	tx_cleaner ();
//
// 	if ( emac_wait_flag ) {
// 		sem_unblock ( emac_sem );
// 		emac_wait_flag = 0;
// 	}
// }
//

//NOTE: WE CANNOT USE STATIC VARIABLES IN THIS FILE

/*------------------------------------------------------------------------
 * ethhandler - Orange Pi (Allwinner H3) Ethernet Interrupt Handler (WIP)
 *------------------------------------------------------------------------
 */
interrupt ethhandler (
		uint32	xnum	/* IRQ number	*/
	)
{

	//TODO Comer says there may be an interrupt for every successful transmission/reception
	//Doesn't have to do anything until the end of the ring is reached
	//TODO Will need this to handle the case where the buffer is empty and a read is attempted, or if it is full and a write is attempted.
// 	kprintf("EMAC interrupt (ethhandler())\n");
// 	kprintf("eh1\n");
	uint32	status;
	struct  dentry  *devptr;        /* address of device control blk*/
	struct 	ethcblk	*ethptr;	/* ptr to control block		*/

	/* Initialize structure pointers */

	devptr = (struct dentry *) &devtab[ETHER0];

	/* Obtain a pointer to the tty control block */

	ethptr = &ethertab[devptr->dvminor];
// 	kprintf("eh2\n");

	//TODO Comer says there may be an interrupt for every successful transmission/reception
	//Doesn't have to do anything until the end of the ring is reached
	//TODO Will need this to handle the case where the buffer is empty and a read is attempted, or if it is full and a write is attempted.
// 	kprintf("EMAC interrupt (ethhandler())\n");
	struct eth_aw_csreg *ep = EMAC_BASE;
	int stat;
	int statx;
	int stat2;

// 	int_count++;

	stat = ep->int_sta;
// 	last_stat = stat;

// 	kprintf("eh3\n");
	// printf ( "emac interrupt --   status:%08x\n", stat );

	/* For now, we run this on each interrupt,
	 * which at the present time is just Rx Ints
	 * XXX someday do this in Tx Ints.
	 */
	// tx_cleaner ();

	statx = stat & INT_RX_MASK;

	if ( statx && statx != INT_RX )
	    printf ( " *** unexpected emac Rx int status: %08x\n", stat );

	// ep->int_ena = INT_RX | INT_TX | INT_TX_UNDERFLOW;
	if ( stat & INT_RX ) {
		kprintf("RX Interrupt (ethhandler.c)\n");
	    rx_handler ( stat, ep->rx_dma_desc_list, ethptr );
	}

	if ( stat & INT_TX ) {
		kprintf("TX Interrupt (ethhandler.c)\n");
		//Print contents of tx ring
		kprintf("ethptr->txRing: %d\n", ethptr->txRing);
// 	    tx_handler ( stat, ep->tx_dma_desc_list, ethptr );
	}

	if ( stat & INT_TX_UNDERFLOW ) {
	    printf ( " *** TX underflow interrupt !!\n" );
	}

	stat2 = ep->int_sta;
	// if ( stat2 != stat )
	//     printf ( "emac interrupt --  xstatus: %08x --> %08x\n", stat, stat2 );

	/* Ack the IRQ in the emac */
	/* experiment shows this to be necessary and correct */
	// ep->int_stat = ep->int_stat & 0xffff; (bad)
	// ep->int_stat &= stat;
	ep->int_sta = stat & 0x3fff;
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
