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
// 	struct	eth_aw_csreg *csrptr;	/* Ethernet CSR pointer	(not currently needed here, so commented out) */
	struct	emac_desc *rdescptr;/* Rx Desc. pointer	*/
	uint32	retval = 0;			/* Num of bytes returned*/

	ethptr = &ethertab[devptr->dvminor];

	/* Get the pointer to Ethernet CSR (not currently needed here, so commented out) */
// 	csrptr = (struct eth_aw_csreg *)ethptr->csr;

	/* Wait for a packet */
// 	kprintf("ethptr->isem ct: %d\n", semcount(ethptr->isem));
	wait(ethptr->isem);

	/* Get pointer to the descriptor */
	rdescptr = (struct emac_desc *)ethptr->rxRing +
						ethptr->rxHead;

	/* Read the packet length */
	retval = rdescptr->size;
	if(retval > count) {
		retval = count;
	}

	/* Copy the packet into user provided buffer */
	memcpy((char *)buf, (char *)rdescptr->buf, retval);

	memset(rdescptr->buf, '\0', RX_ETH_SIZE);
	emac_cache_flush ( (unsigned long) rdescptr->buf, (unsigned long) rdescptr->buf + RX_ETH_SIZE);

	/* Initialize the descriptor for next packet */
	rdescptr->status = ETH_AW_RX_DESC_CTL;
	rdescptr->size = RX_ETH_SIZE;

	emac_cache_flush ( (unsigned long) rdescptr, (unsigned long) &rdescptr[1] );
	/* Increment the head index of rx ring */
	ethptr->rxHead++;
	if(ethptr->rxHead >= ethptr->rxRingSize) {
		ethptr->rxHead = 0;
	}

// 	kprintf("ethread done (%d bytes)\n", retval);
	return retval;
}
