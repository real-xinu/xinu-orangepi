/* ethread.c - ethread */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethread - read an incoming packet on TI AM335X Ethernet
 *------------------------------------------------------------------------
 */
	// TODO look into allwinner_eth.h/tx status symbols (under comment) and rx status symbols. Also cross-reference with h3 datasheet to determine correct values here.
int32	ethread	(
		struct	dentry *devptr,
		void	*buf,
		uint32	count
	)
{
	struct	ethcblk *ethptr;	/* Ethernet ctl blk ptr	*/
	struct	eth_aw_csreg *csrptr;	/* Ethernet CSR pointer	*/
	struct	emac_desc *rdescptr;/* Rx Desc. pointer	*/
	struct	eth_aw_rx_desc *prev;	/* Prev Rx desc pointer	*/
	uint32	retval = 0;			/* Num of bytes returned*/

	ethptr = &ethertab[devptr->dvminor];

	/* Get the pointer to Ethernet CSR */
	csrptr = (struct eth_aw_csreg *)ethptr->csr;

// 	kprintf("ethread before wait()\n");
	/* Wait for a packet */
	kprintf("ethptr->isem ct: %d\n", semcount(ethptr->isem));
	wait(ethptr->isem);
// 	kprintf("ethread after wait()\n");

	/* Get pointer to the descriptor */
	rdescptr = (struct emac_desc *)ethptr->rxRing +
						ethptr->rxHead;

	/* Read the packet length */
	retval = rdescptr->size;
	if(retval > count) {
		retval = count;
	}

	/* Copy the packet into user provided buffer */
// 	kprintf("ethread before memcpy\n");
	memcpy((char *)buf, (char *)rdescptr->buf, retval);
// 	kprintf("ethread after memcpy\n");

	/* Initialize the descriptor for next packet */
	rdescptr->status = ETH_AW_RX_DESC_CTL;
	rdescptr->size = RX_ETH_SIZE;
// 	rdescptr->next = NULL;

// 	kprintf("ethread inserting descriptor\n");
	/* Insert the descriptor into Rx queue */
	//TODO I think the issue is that we are trying to add here when we have actually already created all descs in ethinit
// 	prev = (struct eth_aw_rx_desc *)csrptr->rx_dma_desc_list;
// 	if(prev == NULL) {
// 		kprintf("null rx_dma_desc_list, adding %x\n", rdescptr);
// 		csrptr->rx_dma_desc_list = (reg32)rdescptr;
// 	}
// 	else {
// 		while(prev->next != NULL) {
// 			kprintf("ethread while\n");
// 			prev = prev->next;
// 		}
// 		prev->next = rdescptr;
// 	}
// 	kprintf("ethread after descriptor insert\n");

	emac_cache_flush ( (void *) rdescptr, &rdescptr[1] );
	/* Increment the head index of rx ring */
	ethptr->rxHead++;
	if(ethptr->rxHead >= ethptr->rxRingSize) {
		ethptr->rxHead = 0;
	}

	kprintf("ethread done (%d bytes)\n", retval);
	return retval;
}
