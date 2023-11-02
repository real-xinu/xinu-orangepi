/* ethhandler.c - ethhandler */

#include <xinu.h>

extern struct emac_desc *cur_rx_dma;
extern struct emac_desc *cur_tx_dma;


/*------------------------------------------------------------------------
 * eth_rxPackets - handler for receiver interrupts
 *------------------------------------------------------------------------
 */
local 	void 	eth_rxPackets(
	struct 	ethcblk	*ethptr 	/* ptr to control block		*/
	)
{
	struct	emac_desc *descptr;/* ptr to ring descriptor 	*/
	uint32	tail;			/* pos to insert next packet	*/
	uint32	status;			/* status of ring descriptor 	*/
	int numdesc = 0; 			/* num. of descriptor reclaimed	*/


	tail = ethptr->rxTail;
	descptr = (struct emac_desc *)ethptr->rxRing + tail;

	emac_cache_invalidate ( (unsigned long) descptr, (unsigned long) &descptr[1] );
	descptr = (struct emac_desc *)ethptr->rxRing + tail;
	status = descptr->status;


	while ( ! (status & DS_ACTIVE) ) {
		/* Insert new arrived packet to the tail */

		if (descptr->status & DS_ACTIVE) {
// 			kprintf("ERROR: INACTIVE in rxPackets (ethptr->rxTail: %d)\n", ethptr->rxTail);
			return;
		}

		ethptr->rxTail
			= (ethptr->rxTail + 1) % ethptr->rxRingSize;

		tail = ethptr->rxTail;
		descptr = (struct emac_desc *)ethptr->rxRing + tail;


		emac_cache_invalidate ( (unsigned long) descptr, (unsigned long) &descptr[1] );
		descptr = (struct emac_desc *)ethptr->rxRing + tail;
		status = descptr->status;

		numdesc++;
		cur_rx_dma = cur_rx_dma->next;
	}

	signaln(ethptr->isem, numdesc);
// 	kprintf("ethhandler got %d packets\n", numdesc);

// 	kprintf("addr of last packet: %d\n", descptr);
// 	kprintf("rxTail: %d\n", ethptr->rxTail);
	return;
}


/*------------------------------------------------------------------------
 * eth_txPackets - handler for transmitter interrupts
 *------------------------------------------------------------------------
 */
local 	void 	eth_txPackets(
	struct	ethcblk	*ethptr		/* ptr to control block		*/
	)
{
// 	kprintf("eth_txPackets\n");
	struct	emac_desc *descptr;/* ptr to ring descriptor 	*/
	char 	*pktptr; 		/* ptr used during packet copy  */
	int 	numdesc = 0; 		/* num. of descriptor reclaimed	*/
	uint32	status;			/* status of ring descriptor 	*/


	descptr = (struct emac_desc *)ethptr->txRing + ethptr->txHead;
	emac_cache_invalidate ( (unsigned long) descptr, (unsigned long) &descptr[1] );
	descptr = (struct emac_desc *)ethptr->txRing + ethptr->txHead;
	status = descptr->status;

	while ( ! (status & DS_ACTIVE) ) {
		//If ETH_AW_TX_DESC_CTL is set, the buffer has not been fully transmitted, so we should not clear it out for future use.
		if ((status & ETH_AW_TX_DESC_CTL) || (status == 0 && descptr->size == 0)) {
// 			kprintf("Can't clear untransmitted packet, or no packet to transmit\n");
			break;
		}

// 		kprintf("Transmitted a packet\n");
		/* Clear the write-back descriptor and buffer */

		//Zero out the packet structure since it has been transmitted
		pktptr = (char *)((uint32)(descptr->buf));
		memset(pktptr, '\0', ETH_BUF_SIZE);
		emac_cache_flush ( (unsigned long ) descptr->buf, (unsigned long) descptr->buf + ETH_BUF_SIZE );
		descptr->size = 0;
		descptr->status = 0;
		emac_cache_flush ( (unsigned long) descptr, (unsigned long) &descptr[1] );

		//Increment the tx ring
		ethptr->txHead
			= (ethptr->txHead + 1) % ethptr->txRingSize;
		cur_tx_dma = cur_tx_dma->next;


		descptr = (struct emac_desc *)ethptr->txRing + ethptr->txHead;
		emac_cache_invalidate ( (unsigned long) descptr, (unsigned long) &descptr[1] );
		descptr = (struct emac_desc *)ethptr->txRing + ethptr->txHead;
		numdesc++;
	}

	signaln(ethptr->osem, numdesc);

	return;
}


/*------------------------------------------------------------------------
 * ethhandler - Orange Pi (Allwinner H3) Ethernet Interrupt Handler (WIP)
 *------------------------------------------------------------------------
 */
interrupt ethhandler (
		uint32	xnum	/* IRQ number	*/
	)
{
// 	kprintf("eh1 IRQ %d\n", xnum);
// 	uint32	status;
	struct  dentry  *devptr;        /* address of device control blk*/
	struct 	ethcblk	*ethptr;	/* ptr to control block		*/

	/* Initialize structure pointers */

	devptr = (struct dentry *) &devtab[ETHER0];

	/* Obtain a pointer to the tty control block */

	ethptr = &ethertab[devptr->dvminor];
// 	kprintf("eh2\n");

	struct eth_aw_csreg *csrptr = ethptr->csr;
	int stat;
	int statx;
	int stat2;

// 	int_count++;

	stat = csrptr->int_sta;
// 	last_stat = stat;

// 	kprintf("eh3\n");
	// printf ( "emac interrupt --   status:%08x\n", stat );

	/* For now, we run this on each interrupt,
	 * which at the present time is just Rx Ints
	 * XXX someday do this in Tx Ints.
	 */
	// tx_cleaner ();

	intmask mask = xsec_beg(ethptr->ethlock);	/* multicore protection */
	resched_cntl(DEFER_START);

	statx = stat & INT_RX_MASK;
// 	kprintf("EMAC interrupt (ethhandler()): %08x\n", stat);

	if ( statx && statx != INT_RX ) {
// 	    printf ( " *** unexpected emac Rx int status: %08x\n", stat );
	}

	// csrptr->int_ena = INT_RX | INT_TX | INT_TX_UNDERFLOW;
	if ( stat & INT_RX ) {
// 		kprintf("RX Interrupt (ethhandler.c)\n");
		eth_rxPackets(ethptr);
// 		rx_handler (stat);
// 	    rx_handler ( stat, csrptr->rx_dma_desc_list, ethptr );
	}

	if ( stat & INT_TX ) {
// 		kprintf("TX Interrupt (ethhandler.c)\n");
		//Print contents of tx ring
// 		kprintf("ethptr->txRing: %d\n", ethptr->txRing);
		eth_txPackets(ethptr);
// 	    tx_handler ( stat, csrptr->tx_dma_desc_list, ethptr );
	}

	if ( stat & INT_TX_UNDERFLOW ) {
// 	    printf ( " *** TX underflow interrupt !!\n" );
	}

	stat2 = csrptr->int_sta;
	if ( stat2 != stat ) {
// 	    printf ( "emac interrupt --  xstatus: %08x --> %08x\n", stat, stat2 );
	}

	/* Ack the IRQ in the emac */
	csrptr->int_sta = stat & 0x3fff;


	resched_cntl(DEFER_STOP);
	xsec_end(mask, ethptr->ethlock);
}
