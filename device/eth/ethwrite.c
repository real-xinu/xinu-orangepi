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
	// TODO look into allwinner_eth.h/tx status symbols (under comment) and rx status symbols. Also cross-reference with h3 datasheet to determine correct values here.
	struct	ethcblk *ethptr;	/* Ether entry pointer	*/
	struct	eth_aw_csreg *csrptr;	/* Ethernet CSR pointer	*/
	struct	eth_aw_tx_desc *tdescptr;/* Tx Desc. pointer	*/
	struct	eth_aw_tx_desc *prev;	/* Prev. Desc. pointer	*/

	ethptr = &ethertab[devptr->dvminor];

	/* Get the pointer to the Ethernet CSR */
	csrptr = (struct eth_aw_csreg *)ethptr->csr;

	/* Wait for an empty slot in the queue */
	wait(ethptr->osem);

	/* Get the pointer to the next descriptor */
	tdescptr = (struct eth_aw_tx_desc *)ethptr->txRing +
							ethptr->txTail;

	/* Adjust count if greater than max. possible packet size */
	if(count > PACKLEN) {
		count = PACKLEN;
	}

	/* Initialize the descriptor */
	tdescptr->next = NULL;
	tdescptr->buf_len = count;
	tdescptr->status = DS_ACTIVE;

	/* Copy the packet into the Tx buffer */
	memcpy((char *)tdescptr->buf_addr, buf, count);

	/* Insert the descriptor into Tx queue */

	if(csrptr->tx_dma_desc_list == NULL) {
		/* Tx queue is empty, this desc. will be the first */
		csrptr->tx_dma_desc_list = (uint32)tdescptr;
	}
	else {
		/* Tx queue not empty, insert at end */
		prev = (struct eth_aw_tx_desc *)
				csrptr->tx_dma_desc_list;
		while(prev->next != NULL) {
			prev = prev->next;
		}
		prev->next = tdescptr;
	}

	/* Increment the tail index of the Tx ring */
	ethptr->txTail++;
	if(ethptr->txTail >= ethptr->txRingSize) {
		ethptr->txTail = 0;
	}

	return count;
}
