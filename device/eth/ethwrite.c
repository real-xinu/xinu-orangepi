/* ethwrite.c - ethwrite */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethwrite - enqueue a packet for transmission on TI AM335X Ethernet
 *------------------------------------------------------------------------
 */
int32	ethwrite (
		struct	dentry *devptr,
		void	*buf,
		uint32	count
	)
{

// 	kprintf("ethwrite\n");
	struct	ethcblk *ethptr;	/* Ether entry pointer	*/
	struct	eth_aw_csreg *csrptr;	/* Ethernet CSR pointer	*/
	struct	emac_desc *tdescptr;/* Tx Desc. pointer	*/

	ethptr = &ethertab[devptr->dvminor];

	/* Verify arguments are valid */

	if ((count < ETH_HDR_LEN) || (count > ETH_MAX_PKT_LEN) ) {
		return SYSERR;
	}

	/* Get the pointer to the Ethernet CSR */
	csrptr = (struct eth_aw_csreg *)ethptr->csr;


	// Verify Ethernet link is up. This check definitely works - when I try it with the ethernet unplugged, it reports link down, and it proceeds when the link is up otherwise.
	uint32 reg;
	eth_phy_read(csrptr, ETH_PHY_STATREG, ETH_PHY_CTLREG, &reg);
	if (!(reg & BMSR_LINK_UP)) {
// 		kprintf("Error: Link down\n");
		return SYSERR;
	}

// 	kprintf("before ethwrite wait (txTail: %d)\n", ethptr->txTail);
	/* Wait for an empty slot in the queue */
	wait(ethptr->osem);

	/* Get the pointer to the next descriptor */
	tdescptr = (struct emac_desc *)ethptr->txRing +
							ethptr->txTail;

	/* Adjust count if greater than max. possible packet size */
	if(count > PACKLEN) {
		count = PACKLEN;
	}

	/* Initialize the descriptor */
// 	tdescptr->next = NULL;
	tdescptr->size = count;

// 	kprintf("desc status before: %d\n", tdescptr->status);

// 	kprintf("Getting to ethwrite. Next step in comment\n");
// 	kprintf("buf->type: %d\n", ((struct	etherPkt *)buf)->type);
// 	kprintf("before ethwrite memcpy\n");
	/* Copy the packet into the Tx buffer */
	memcpy((char *)tdescptr->buf, (char*) buf, count);
// 	kprintf("after ethwrite memcpy\n");

	emac_cache_flush ( (unsigned long ) tdescptr->buf, (unsigned long) tdescptr->buf + count );

	//NOTE: I know for sure that a properly formatted ARP packet is making it here, because my packet is 42 bytes, which is the size of an ARP packet.
	/* ARP packets are small (42 bytes) and may get rejected unless padded.
	 */
	if ( count < ETH_MIN_SIZE ) count = ETH_MIN_SIZE;

	#define DS_TX_SEND	( DS_TX_INT | DS_TX_LAST | DS_TX_FIRST | DS_TX_ADR_CHAIN )
	tdescptr->size = (count & 0x7ff) | DS_TX_SEND;
	tdescptr->status = DS_ACTIVE;
	emac_cache_flush ( (unsigned long) tdescptr, (unsigned long) &tdescptr[1] );

	csrptr->tx_ctl_1 |= TX_DMA_START;

	//Start TX
	csrptr->tx_ctl_1 |= TX_DMA_ENA;
	csrptr->tx_ctl_0 |= TX_EN;

// 	kprintf ( "ethwrite: Tx Buf (%08x) status: %08x %08x  %08x %08x\n", tdescptr, tdescptr->status, tdescptr->size, tdescptr->buf, tdescptr->next );
	/* Insert the descriptor into Tx queue */

	/* Increment the tail index of the Tx ring */
	ethptr->txTail++;
	if(ethptr->txTail >= ethptr->txRingSize) {
		ethptr->txTail = 0;
	}

// 	kprintf("txTail is now %d, txHead is %d\n", ethptr->txTail, ethptr->txHead);

// 	kprintf("desc status after: %d\n", tdescptr->status);
	return count;
}
