/* ethread.c - ethread */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethread - read an incoming packet on TI AM335X Ethernet
 *------------------------------------------------------------------------
 */
int32	ethread	(
		struct	dentry *devptr,
		void	*buf,
		uint32	count
	)
{
	struct	ethcblk *ethptr;	/* Ethernet ctl blk ptr	*/
	struct	eth_aw_csreg *csrptr;	/* Ethernet CSR pointer	*/
	struct	eth_aw_rx_desc *rdescptr;/* Rx Desc. pointer	*/
	struct	eth_aw_rx_desc *prev;	/* Prev Rx desc pointer	*/
	uint32	retval = 0;			/* Num of bytes returned*/

	ethptr = &ethertab[devptr->dvminor];

	/* Get the pointer to Ethernet CSR */
	csrptr = (struct eth_aw_csreg *)ethptr->csr;

	/* Wait for a packet */
	wait(ethptr->isem);

	/* Get pointer to the descriptor */
	rdescptr = (struct eth_aw_rx_desc *)ethptr->rxRing +
						ethptr->rxHead;

	/* Read the packet length */
	retval = rdescptr->buf_len;
	if(retval > count) {
		retval = count;
	}

	/* Copy the packet into user provided buffer */
	memcpy((char *)buf, (char *)rdescptr->buf_addr, retval);

	/* Initialize the descriptor for next packet */
	rdescptr->status = DS_ACTIVE;
	rdescptr->buf_len = ETH_BUF_SIZE;
	rdescptr->next = NULL;

	/* Insert the descriptor into Rx queue */
	prev = (struct eth_aw_rx_desc *)csrptr->rx_dma_desc_list;
	if(prev == NULL) {
		kprintf("null rx_dma_desc_list, adding %x\n", rdescptr);
		csrptr->rx_dma_desc_list = (reg32)rdescptr;
	}
	else {
		while(prev->next != NULL) {
			prev = prev->next;
		}
		prev->next = rdescptr;
	}

	/* Increment the head index of rx ring */
	ethptr->rxHead++;
	if(ethptr->rxHead >= ethptr->rxRingSize) {
		ethptr->rxHead = 0;
	}

	return retval;
}
