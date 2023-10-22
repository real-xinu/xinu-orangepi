/*
 * Copyright (C) 2016  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 *
 * emac.c for the Orange Pi PC and PC Plus
 *
 * Tom Trebisky  3/20/2017  8/2/2020
 *
 * emac.c
 * Ethernet driver for the Allwinner H3 emac
 *
 * The Orange PI PC and the PC Plus both use the PHY internal to the
 *   H3 chip and thus support only 10/100 ethernet.
 *
 * The Orange Pi Plus (not the PC Plus) also uses the H3 chip,
 *   but has an external PHY and thus can do gigabit.
 * I don't have a plus (and probably never will), so this driver
 *   does not support the external gigabit PHY on that board.
 *
 * The PC Plus board is easy to tell apart from the PC
 *  -- it has a Wifi antenna (next to the micro-USB connector).
 *  -- it has the RTL8189 Wifi chip (near the Wifi antenna).
 *  -- it has one RAM chip on each side of the board
 *   (the PC has both RAM chips neatly on the top side.)
 */

#include <xinu.h>

// #include "net.h"

struct	ethcblk ethertab[1];
void emac_show ( void );
void tx_start ( void );
void rx_start ( void );

// extern void et_snd( void );

/* ------------------------------------------------------------ */
/* Descriptors */
/* ------------------------------------------------------------ */

#ifdef notdef
/* From the sunxi linux driver - bits in the status word of a DMA descriptor */
/* There looks like a lot of undocumented bits here */
typedef union {
        struct {
                /* TDES0 */
                unsigned int deferred:1;        /* Deferred bit (only half-duplex) */
                unsigned int under_err:1;       /* Underflow error */
                unsigned int ex_deferral:1;     /* Excessive deferral */
                unsigned int coll_cnt:4;        /* Collision count */
                unsigned int vlan_tag:1;        /* VLAN Frame */
                unsigned int ex_coll:1;         /* Excessive collision */
                unsigned int late_coll:1;       /* Late collision */
                unsigned int no_carr:1;         /* No carrier */
                unsigned int loss_carr:1;       /* Loss of collision */
                unsigned int ipdat_err:1;       /* IP payload error */
                unsigned int frm_flu:1;         /* Frame flushed */
                unsigned int jab_timeout:1;     /* Jabber timeout */
                unsigned int err_sum:1;         /* Error summary */
                unsigned int iphead_err:1;      /* IP header error */
                unsigned int ttss:1;            /* Transmit time stamp status */
                unsigned int reserved0:13;
                unsigned int own:1;             /* Own bit. CPU:0, DMA:1 */
        } tx;

        /* bits 5 7 0 | Frame status
         * ----------------------------------------------------------
         *      0 0 0 | IEEE 802.3 Type frame (length < 1536 octects)
         *      1 0 0 | IPv4/6 No CSUM errorS.
         *      1 0 1 | IPv4/6 CSUM PAYLOAD error
         *      1 1 0 | IPv4/6 CSUM IP HR error
         *      1 1 1 | IPv4/6 IP PAYLOAD AND HEADER errorS
         *      0 0 1 | IPv4/6 unsupported IP PAYLOAD
         *      0 1 1 | COE bypassed.. no IPv4/6 frame
         *      0 1 0 | Reserved.
         */
        struct {
                /* RDES0 */
                unsigned int chsum_err:1;       /* Payload checksum error */
                unsigned int crc_err:1;         /* CRC error */
                unsigned int dribbling:1;       /* Dribble bit error */
                unsigned int mii_err:1;         /* Received error (bit3) */
                unsigned int recv_wt:1;         /* Received watchdog timeout */
                unsigned int frm_type:1;        /* Frame type */
                unsigned int late_coll:1;       /* Late Collision */
                unsigned int ipch_err:1;        /* IPv header checksum error (bit7) */
                unsigned int last_desc:1;       /* Laset descriptor */
                unsigned int first_desc:1;      /* First descriptor */
                unsigned int vlan_tag:1;        /* VLAN Tag */
                unsigned int over_err:1;        /* Overflow error (bit11) */
                unsigned int len_err:1;         /* Length error */
                unsigned int sou_filter:1;      /* Source address filter fail */
                unsigned int desc_err:1;        /* Descriptor error */
                unsigned int err_sum:1;         /* Error summary (bit15) */
                unsigned int frm_len:14;        /* Frame length */
                unsigned int des_filter:1;      /* Destination address filter fail */
                unsigned int own:1;             /* Own bit. CPU:0, DMA:1 */
        #define RX_PKT_OK               0x7FFFB77C
        #define RX_LEN                  0x3FFF0000
        } rx;

        unsigned int all;
} desc0_u;
#endif


/* ------------------------------------------------------------ */
/* Descriptors */
/* ------------------------------------------------------------ */

/* The linux driver allocates 256 descriptors for each
 * When I try this I get a data abort
 */
// #define NUM_RX	256
// #define NUM_TX	256

/* For debug this is handy and fits on one screen */
// #define NUM_RX	16
// #define NUM_TX	16
// #define NUM_RX	16
// #define NUM_TX	4
#define NUM_RX	64
#define NUM_TX	64


/*
#define NUM_RX_UBOOT	32
#define UBOOT_PKTSIZE	2048
*/

#ifdef notdef
// char rx_buffers[NUM_RX * RX_SIZE];
char rx_buffers[NUM_RX * RX_SIZE] __aligned(ARM_DMA_ALIGN);

struct emac_desc rx_desc[NUM_RX];
#endif

struct emac_desc *rx_list;
struct emac_desc *tx_list;

struct emac_desc *cur_rx_dma;

struct emac_desc *cur_tx_dma;
struct emac_desc *clean_tx_dma;

int emac_wait_flag;
sid32 emac_sem;

void emac_debug ( void );

char *
ether2str ( unsigned char *bytes )
{
    	char buf[20];
	int i, len;
	char *p;

	len = ETH_ADDR_SIZE;
	p = buf;
	for ( i=0; i<len; i++ ) {
	    sprintf ( p, "%02x", bytes[i] );
	    p += 2;
	    if ( i < len-1 )
		*p++ = ':';
	}
	*p = '\0';

	return buf;
}


// rx_list_show ( rx_list, NUM_RX );
// rx_list_show ( struct emac_desc *desc, int num )
void
rx_list_show ( void )
{
	struct emac_desc *desc = rx_list;
	int num = NUM_RX;

	struct emac_desc *edp;
	int len;
	int i;

	// invalidate_dcache_range ( (void *) desc, &desc[num] );
	emac_cache_invalidate ( (void *) desc, &desc[num] );

	for ( i=0; i<num; i++ ) {
	    edp = &desc[i];
	    len = (edp->status >> 16) & 0x3fff;
	    if ( edp == cur_rx_dma )
		kprintf ( "* Rx Buf %2d (%08x) status: %08x  %d/%d %08x %08x\n", i, edp, edp->status, len, edp->size, edp->buf, edp->next );
	    else
		kprintf ( "  Rx Buf %2d (%08x) status: %08x  %d/%d %08x %08x\n", i, edp, edp->status, len, edp->size, edp->buf, edp->next );
	}
}

// tx_list_show ( tx_list, NUM_TX );
// tx_list_show ( struct emac_desc *desc, int num )
void
tx_list_show ( void )
{
	struct emac_desc *desc = tx_list;
	int num = NUM_TX;

	struct emac_desc *edp;
	int len;
	int i;

	// invalidate_dcache_range ( (void *) desc, &desc[num] );
	emac_cache_invalidate ( (void *) desc, &desc[num] );

	for ( i=0; i<num; i++ ) {
	    edp = &desc[i];
	    if ( edp == cur_tx_dma && cur_tx_dma == clean_tx_dma )
		kprintf ( "* Tx Buf %2d (%08x) status: %08x %08x  %08x %08x\n", i, edp, edp->status, edp->size, edp->buf, edp->next );
	    else if ( edp == clean_tx_dma )
		kprintf ( "> Tx Buf %2d (%08x) status: %08x %08x  %08x %08x\n", i, edp, edp->status, edp->size, edp->buf, edp->next );
	    else if ( edp == cur_tx_dma )
		kprintf ( "* Tx Buf %2d (%08x) status: %08x %08x  %08x %08x\n", i, edp, edp->status, edp->size, edp->buf, edp->next );
	    else
		kprintf ( "  Tx Buf %2d (%08x) status: %08x %08x  %08x %08x\n", i, edp, edp->status, edp->size, edp->buf, edp->next );
	}
}

/* We have 32 * 2k for Rx bufs (64K)
 * We have 32 * 2k for Tx bufs (64K)
 * and we have 64 * 64 bytes for descriptors (4K)
 * This is 128 + 4 = 132K, fits handily in 1M
 */
char *nocache;

struct emac_desc *
rx_list_init ( void )
{
	struct emac_desc *edp;
	char *buf;
	struct emac_desc *desc;

	// kprintf ( "Descriptor size: %d bytes\n", sizeof(struct emac_desc) );

	/*
	mem = getmem ( (NUM_RX+1) * sizeof(struct emac_desc) );
	mem = (mem + sizeof(struct emac_desc)) & ~ARM_DMA_ALIGN;
	desc = (struct emac_desc *) mem;

	mem = getmem ( NUM_RX * RX_SIZE + ARM_DMA_ALIGN );
	mem = (mem + ARM_DMA_ALIGN) & ~ARM_DMA_ALIGN;
	buf = (char *) mem;
	*/

	/* We can depend on getmem to give us dma aligned addresses */
	desc = (struct emac_desc *) getmem ( NUM_RX * sizeof(struct emac_desc) );
	buf = (char *) getmem ( NUM_RX * RX_SIZE );
	if (desc == SYSERR || buf == SYSERR) {
		panic("SYSERR in emac.c\n");
	}
	kprintf("desc: %08x, buf: %08x\n", desc, buf);
	kprintf("NUM_RX * sizeof(struct emac_desc): %d\n", (NUM_RX * sizeof(struct emac_desc)));
	kprintf("NUM_RX * RX_SIZE: %d\n", (NUM_RX * RX_SIZE));
	memset(desc, '\0', NUM_RX * sizeof(struct emac_desc));
	memset(buf, '\0', NUM_RX * RX_SIZE);

	for ( edp = desc; edp < &desc[NUM_RX]; edp ++ ) {
// 		kprintf("buf: %d\n", buf);
	    edp->status = DS_ACTIVE;
	    edp->size = RX_ETH_SIZE;
	    edp->buf = buf;
	    edp->next = &edp[1];
	    buf += RX_SIZE;
	}

	desc[NUM_RX-1].next = &desc[0];

	// emac_cache_flush ( (void *) desc, &desc[NUM_RX] );
	// rx_list_show ( desc, NUM_RX );

	return desc;
}

void
reset_rx_list ( struct emac_desc *list, int num )
{
	struct emac_desc *edp;

	list->status = DS_ACTIVE;
	for ( edp = list->next; edp != list; edp = edp->next )
	    edp->status = DS_ACTIVE;

/* Doing this will trigger a bug in the emac silicon.
 * with the size field set to 2048, it will run through all
 * of the available buffers putting nothing in any of them.
 */
#ifdef notdef
	/* XXX */
	list->size = 2048;
	for ( edp = list->next; edp != list; edp = edp->next )
	    edp->size = 2048;
#endif

	// flush_dcache_range ( (void *) list, &list[num] );
	emac_cache_flush ( (void *) list, &list[num] );
}

struct emac_desc *
tx_list_init ( void )
{
	int i;
	struct emac_desc *edp;
	struct emac_desc *desc;
	// unsigned long mem;
	char *buf;

	/* We can depend on getmem to give us dma aligned addresses */
	desc = (struct emac_desc *) getmem ( NUM_TX * sizeof(struct emac_desc) );
	buf = (char *) getmem ( NUM_TX * TX_SIZE );

	for ( edp = desc; edp < &desc[NUM_TX]; edp ++ ) {
	    edp->status = DS_ACTIVE;
	    edp->size = 0;
	    edp->buf = buf;
	    edp->next = &edp[1];
	    buf += TX_SIZE;
	}

	desc[NUM_TX-1].next = &desc[0];

	// flush_dcache_range ( (void *) desc, &desc[NUM_TX] );
	emac_cache_flush ( (void *) desc, &desc[NUM_TX] );

	return desc;
}

void
init_rings ( struct dentry *devptr )
{
	kprintf("Starting ring init\n");

	struct	ethcblk *ethptr;		/* Ethernet control blk pointer	*/
	ethptr = &ethertab[devptr->dvminor];

	struct emac *ep = EMAC_BASE;
	void *desc;

#ifdef EMAC_NOCACHE
	/* This gives us a full megabyte of memory with
	 * caching disabled.
	 */
	nocache = (char *) getmem ( 1024 );
#endif

#ifdef USE_UBOOT_RX
	/* Find the list that U-Boot left for us */
	desc = (void *) ep->rx_desc;
	reset_rx_list ( desc, NUM_RX_UBOOT );
#else
	desc = rx_list_init ();
#endif
	rx_list = desc;
	ethptr->rxRing = rx_list;
	ethptr->rxHead = 0;
	ethptr->rxTail = 0;
	ethptr->rxRingSize = NUM_RX;
	/* Reload the dma pointer register.
	 * This causes the dma list pointer to get reset.
	 */
	ep->rx_desc = desc;
	kprintf("ep->rx_desc: %d\n", ep->rx_desc);
	cur_rx_dma = desc;

	// rx_list_show ( (struct emac_desc *) ep->rx_desc, NUM_RX_UBOOT );

	/* Now set up the Tx list */
	desc = tx_list_init ();
	tx_list = desc;
	ethptr->txRing = tx_list;
	ethptr->txHead = 0;
	ethptr->txTail = 0;
	ethptr->txRingSize = NUM_TX;

	clean_tx_dma = cur_tx_dma = desc;
	ep->tx_desc = desc;
	kprintf("Completed ring init\n");
}

/* ------------------------------------------------------------ */
/* Interrupts */
/* ------------------------------------------------------------ */

int rx_count;
int tx_count;


#define ONE_LINE

void
emac_show_packet ( int tag, int i_dma, struct netbuf *nbp )
{
#ifdef ONE_LINE
        kprintf ( "%cRx packet %d (i_dma= %d), %d bytes", tag, rx_count, i_dma, nbp->elen );
        kprintf ( " ether src: %s", ether2str(nbp->eptr->src) );
        kprintf ( " ## dst: %s", ether2str(nbp->eptr->dst) );

        if ( nbp->eptr->type == ETH_ARP_SWAP )
            kprintf ( " (ARP)" );
        else if ( nbp->eptr->type == ETH_IP_SWAP ) {
	    if ( nbp->iptr->proto == IPPROTO_ICMP )
		kprintf ( " (IP: ICMP)" );
	    else if ( nbp->iptr->proto == IPPROTO_UDP )
		kprintf ( " (IP: UDP)" );
	    else if ( nbp->iptr->proto == IPPROTO_TCP )
		kprintf ( " (IP: TCP)" );
	    else
		kprintf ( " (IP: %d)", nbp->iptr->proto );
	} else
	    kprintf ( " ether type: %04x", nbp->eptr->type );
	kprintf ( "\n" );
#else
        kprintf ( "emac packet %d, %d bytes, ether type: %04x", rx_count, nbp->elen, nbp->eptr->type );
        kprintf ( " ether src: %s", ether2str(nbp->eptr->src) );
        kprintf ( " ## dst: %s\n", ether2str(nbp->eptr->dst) );

        if ( nbp->eptr->type == ETH_ARP_SWAP ) {
            kprintf ( "ARP packet\n" );
        } else if ( nbp->eptr->type == ETH_IP_SWAP ) {
            kprintf ( "ip src: %s (%08x)\n", ip2strl(nbp->iptr->src), nbp->iptr->src );
            kprintf ( "ip dst: %s (%08x)\n", ip2strl(nbp->iptr->dst), nbp->iptr->dst );
            kprintf ( "ip proto: %d\n", nbp->iptr->proto );
        } else
            kprintf ( "unknown packet type\n" );

        // dump_buf ( nbp->eptr, nbp->elen );
#endif
}

#ifdef notdef
/* Wrap a received packet in a netbuf, just for purposes of
 * display
 */
struct netbuf dump_netbuf;

void
kyu_receive ( char *buf, int len )
{
	struct netbuf *nbp;

	// invalidate_dcache_range ( (unsigned long) buf, (unsigned long) &buf[UBOOT_PKTSIZE] );
	emac_cache_invalidate ( (unsigned long) buf, (unsigned long) &buf[UBOOT_PKTSIZE] );

        // nbp = netbuf_alloc ();
        // nbp = netbuf_alloc_i ();
        // if ( ! nbp )
        //    return;     /* drop packet */

	nbp = &dump_netbuf;
	nbp->bptr = nbp->data;
        nbp->eptr = (struct eth_hdr *) (nbp->bptr + NETBUF_PREPAD * sizeof(struct netbuf *));
        nbp->iptr = (struct ip_hdr *) ((char *) nbp->eptr + sizeof ( struct eth_hdr ));

        // nbp->elen = len - 4;
        // memcpy ( (char *) nbp->eptr, buf, len - 4 );

        nbp->elen = len;
        memcpy ( (char *) nbp->eptr, buf, len );

        // net_rcv ( nbp );

	emac_show_packet ( ' ',0, nbp );
	// netbuf_free ( nbp );
}
#endif

// #ifdef notdef
// struct emac_desc {
// 	unsigned long status;	/* status */
// 	long size;		/* st */
// 	char * buf;		/* buf_addr */
// 	struct emac_desc *next;	/* next */
// }	__aligned(ARM_DMA_ALIGN);
// #endif

int last_capture;

void
capture_last ( int val )
{
	last_capture = val;
}

int last_stat;		/* last interrupt status */
int last_desc_stat;	/* last descriptor status */
int last_len;
char last_buf[2048];
int prior_len;
char prior_buf[2048];

// void
// rx_handler ( int stat )
// {
// 	struct netbuf *nbp;
// 	int len;
// 	int tag = ' ';
//
// 	kprintf ( "Rx interrupt, packet incoming (emac)\n" );
// // 	et_rx ();
//
// 	// invalidate_dcache_range ( (void *) cur_rx_dma, &cur_rx_dma[1] );
// 	emac_cache_invalidate ( (void *) cur_rx_dma, &cur_rx_dma[1] );
//
// 	while ( ! (cur_rx_dma->status & DS_ACTIVE) ) {
// 	    int i_dma = (cur_rx_dma - rx_list);
//
// 	    rx_count++;
// 	    len = (cur_rx_dma->status >> 16) & 0x3fff;
// 	    last_desc_stat = cur_rx_dma->status;
//
// 	    if ( last_desc_stat & ~0x3fff0000 != 0x00000320 )
// 		kprintf ( "Unusual desc status: %08x\n", cur_rx_dma->status );
//
// 		kprintf("RECEIVED PACKET (handling not implemented - discarding)\n");
// 	    // nbp = netbuf_alloc ();
// // 	    nbp = netbuf_alloc_i ();
// //
// // 	    if ( ! nbp )
// // 		return;     /* drop packet */
// //
// // 	    // pkt_arrive ();
// //
// // 	    nbp->elen = len - 4;
// // 	    memcpy ( (char *) nbp->eptr, cur_rx_dma->buf, len - 4 );
// //
// // 	    if ( last_capture ) {
// // 		if ( last_len ) {
// // 		    prior_len = last_len;
// // 		    memcpy ( prior_buf, last_buf, last_len );
// // 		}
// // 		last_len = len - 4;
// // 		memcpy ( last_buf, cur_rx_dma->buf, len - 4 );
// // 	    }
// //
// // 	    // emac_show_packet ( tag, i_dma, nbp );
// // 	    // tag = '*';
// //
// // 	    cur_rx_dma->status = DS_ACTIVE;
// //
// // 	    // flush_dcache_range ( (void *) cur_rx_dma, &cur_rx_dma[1] );
// // 	    emac_cache_flush ( (void *) cur_rx_dma, &cur_rx_dma[1] );
// //
// // 	    net_rcv ( nbp );
// //
// // 	    cur_rx_dma = cur_rx_dma->next;
// //
// // 	    // invalidate_dcache_range ( (void *) cur_rx_dma, &cur_rx_dma[1] );
// // 	    emac_cache_invalidate ( (void *) cur_rx_dma, &cur_rx_dma[1] );
// 	}
// }

#ifdef notdef
void
rx_handler ( int stat )
{
	int len;
	// int i_dma;

	// don't invalidate the whole thing ...
	// invalidate_dcache_range ( (void *) rx_list, &rx_list[NUM_RX_UBOOT] );

	/*
	if ( first_int ) {
	    rx_list_show ( (struct emac_desc *) ep->rx_desc, NUM_RX_UBOOT );
	    first_int = 0;
	}
	*/

	invalidate_dcache_range ( (void *) cur_rx_dma, &cur_rx_dma[1] );

	while ( ! (cur_rx_dma->status & DS_ACTIVE) ) {
	    rx_count++;
	    len = (cur_rx_dma->status >> 16) & 0x3fff;
	    /*
	    i_dma = (cur_rx_dma - rx_list);
	    kprintf ( " ---- buf %d (%08x) status: %08x  %d/%d %08x %08x\n",
		i_dma, cur_rx_dma, cur_rx_dma->status, len, cur_rx_dma->size, cur_rx_dma->buf, cur_rx_dma->next );
		*/

	    // kyu_receive ( cur_rx_dma->buf, len );

	    cur_rx_dma->status = DS_ACTIVE;
	    flush_dcache_range ( (void *) cur_rx_dma, &cur_rx_dma[1] );

	    cur_rx_dma = cur_rx_dma->next;
	    invalidate_dcache_range ( (void *) cur_rx_dma, &cur_rx_dma[1] );
	}

	// don't flush the whole thing ...
	// flush_dcache_range ( (void *) rx_list, &rx_list[NUM_RX_UBOOT] );
}
#endif

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
void
tx_cleaner ( void )
{
	if ( cur_tx_dma == clean_tx_dma )
	    return;

	// tx_list_show ();

	while ( clean_tx_dma != cur_tx_dma ) {
	    if ( clean_tx_dma->status & DS_ACTIVE)
		break;
	    // kprintf ( "Tx clean: %08x %08x\n", clean_tx_dma->status, clean_tx_dma->size );
	    clean_tx_dma = clean_tx_dma->next;
	    // pkt_finish ();
	}

	// emac_busy = 0;
}

void
tx_handler ( int stat )
{
	// kprintf ( "Emac tx interrupt %08x\n", stat );
	// phy_show ();
	// emac_debug ();
// 	et_tx ();
	tx_cleaner ();

	if ( emac_wait_flag ) {
	    signal ( emac_sem );
	    emac_wait_flag = 0;
	}
}

int int_count;
int rx_int_count;
int tx_int_count;
int first_int;

/* Interrupt handler.
 *
 * As soon as we enable interrupts, we get one and are here with
 * status 0x40000527, but nothing interesting in the rings.
 * This seems entirely harmless and is probably a relic that will
 * go away once we fully initialize things and reset the emac.
 *
 * When just receiving packets we see 0x40000100
 *  no telling what the 0x40000000 bit is.
 *  we expect the 0x100 (RX_INT).
 *  eventually we also expect 0x001 (TX_INT).
 *
 * With transmit interrupts enabled we often enter this routine
 * with a status like 0x25 (TX and TX_BUF), but by the time
 * we exit the RX interrupt is asserted.
 * We need to avoid clearing that RX interrupt if we have not
 * processed it.
 */
void
emac_handler ( int junk )
{
	struct emac *ep = EMAC_BASE;
	int stat;
	int statx;
	int stat2;

	int_count++;

	stat = ep->int_stat;
	last_stat = stat;

	// kprintf ( "emac interrupt --   status:%08x\n", stat );

	/* For now, we run this on each interrupt,
	 * which at the present time is just Rx Ints
	 * XXX someday do this in Tx Ints.
	 */
	// tx_cleaner ();

	statx = stat & INT_RX_MASK;

	if ( statx && statx != INT_RX )
	    kprintf ( " *** unexpected emac Rx int status: %08x\n", stat );

	// ep->int_ena = INT_RX | INT_TX | INT_TX_UNDERFLOW;
	if ( stat & INT_RX ) {
	    ++rx_int_count;
// 	    rx_handler ( stat );
	}

	if ( stat & INT_TX ) {
	    ++tx_int_count;
// 	    tx_handler ( stat );
	}

	if ( stat & INT_TX_UNDERFLOW ) {
	    kprintf ( " *** TX underflow interrupt !!\n" );
	}

	if ( first_int ) {
	    emac_show ();
	    first_int = 0;
	}

	stat2 = ep->int_stat;
	// if ( stat2 != stat )
	//     kprintf ( "emac interrupt --  xstatus: %08x --> %08x\n", stat, stat2 );

	/* Ack the IRQ in the emac */
	/* experiment shows this to be necessary and correct */
	// ep->int_stat = ep->int_stat & 0xffff; (bad)
	// ep->int_stat &= stat;
	ep->int_stat = stat & 0x3fff;
}

/* ------------------------------------------------------------ */
/* Real Emac stuff */
/* ------------------------------------------------------------ */

/* What is SID ?  Who is SID ?
 * Some mystery from the H3 manual that had me interested
 * at one poiht, perhaps when I was on a misguided search
 * looking for a MAC address.
 */

#define SID_BASE	((unsigned int *) 0x01c14200)

/* This yields:
 * SID 0 = 01c14200: 02004620
 * SID 1 = 01c14204: 7435c614
 * SID 2 = 01c14208: 504206ce
 * SID 3 = 01c1420c: 3800001a
 * SID 4 = 01c14210: 0f000000
 * SID 5 = 01c14214: 00000000
 * SID 6 = 01c14218: 00000000
 * SID 7 = 01c1421c: 00000000
 *
 * It very well may be that only the first 4 words are SID.
 */

void
show_sid ( void )
{
	volatile unsigned int *p = SID_BASE;
	int i;

	for ( i=0; i<8; i++ ) {
	    kprintf ( "SID %d = %08x: %08x\n", i, p, *p );
	    p++;
	}
}

#ifdef notdef
/* This doesn't give the same as we see Wireshark pick up */
void
get_mac ( char *mac_addr )
{
	mac_addr[0] = 0x02;
	mac_addr[1] = (sid[0] >>  0) & 0xff;
	mac_addr[2] = (sid[3] >> 24) & 0xff;
	mac_addr[3] = (sid[3] >> 16) & 0xff;
	mac_addr[4] = (sid[3] >>  8) & 0xff;
	mac_addr[5] = (sid[3] >>  0) & 0xff;
}
#endif

/* Interestingly the emac can accomodate 8 MAC addresses.
 * All but the first must have a bit set to indicate they
 *  are active.
 */
void
set_mac ( char *mac_id )
{
	struct emac *ep = EMAC_BASE;

	ep->mac_addr[0].hi = mac_id[4] + (mac_id[5] << 8);
	ep->mac_addr[0].lo = mac_id[0] + (mac_id[1] << 8) +
	    (mac_id[2] << 16) + (mac_id[3] << 24);
}

// seems to take 2 milliseconds
// actually takes 10 ticks
#define SOFT_RESET_TIMEOUT	500

void
emac_reset ( void )
{
	struct emac *ep = EMAC_BASE;
        int tmo = SOFT_RESET_TIMEOUT;

	ep->ctl1 |= CTL1_SOFT_RESET;

        while ( tmo-- && ep->ctl1 & CTL1_SOFT_RESET )
	    ;
            //thr_delay ( 1 );

        // kprintf ( "Emac Reset cleared in %d milliseconds\n", (SOFT_RESET_TIMEOUT-tmo) );
        kprintf ( "Emac Reset cleared in %d ticks\n", (SOFT_RESET_TIMEOUT-tmo) );
}

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */

/* Try to treat a register like ram */
/* Seems to work just fine */
void
verify_regs ( void )
{
	struct emac *ep = EMAC_BASE;

	kprintf ( "MAC test = %08x\n", ep->mac_addr[0].hi );
	ep->mac_addr[0].hi = 0;
	kprintf ( "MAC test = %08x\n", ep->mac_addr[0].hi );
	ep->mac_addr[0].hi = 0xff;
	kprintf ( "MAC test = %08x\n", ep->mac_addr[0].hi );
	ep->mac_addr[0].hi = 0;
	kprintf ( "MAC test = %08x\n", ep->mac_addr[0].hi );

	/* Verify structure */
	ep = (struct emac *) 0;
	kprintf ( "MII data = %08x\n", &ep->mii_data );
	kprintf ( "MII cmd  = %08x\n", &ep->mii_cmd );
}

#ifdef notdef
/* Never actually worked */
int
emac_init_ORIG ( void )
{
	struct emac *ep = EMAC_BASE;
	void *list;
	char mac_buf[6];

	emac_reset ();
	get_mac ( mac_buf );
	set_mac ( mac_buf );

	phy_init ();			/* sets ctl0 */

	ep->ctl1 = CTL1_BURST_8;

	ep->rx_ctl1 |= RX_MD;

	ep->rx_desc = rx_list_init ();
	// ep->tx_desc = tx_list_init ();

	ep->rx_ctl1 |= RX_DMA_ENA;
	ep->rx_ctl0 |= RX_EN;

	// ep->tx_ctl1 |= TX_DMA_ENA;
	// ep->tx_ctl0 = TX_EN;

	ep->rx_filt = RX_FILT_DIS;

        return 1;
}
#endif

/* Post this here so our genereic network stack can fetch it.
 * We use it locally also.
 */
char emac_mac[ETH_ADDR_SIZE];

/* For the outside world */
void
get_emac_addr ( char *addr )
{
	memcpy ( addr, emac_mac, ETH_ADDR_SIZE );
}

/*
 * our MAC address on the wire is: 02:20:7f:9b:26:8c with U-Boot
 * When running linux however:     c2:c2:9b:ae:f9:5e
 */
void
fetch_linux_mac ( char *addr )
{
	*addr++ = 0x02;
	*addr++ = 0x20;
	*addr++ = 0xcc;
	*addr++ = 0xa2;
	*addr++ = 0xd5;
	*addr++ = 0xff;
}

#ifdef notdef
/* Wireshark shows that my development board has the MAC address:
 * 02:20:7f:9B:26:8c
 * Just jamming these values will do for now ...
 */
void
get_mac ( char *x )
{
	x[0] = 0x02;
	x[1] = 0x20;
	x[2] = 0xcc;
	x[3] = 0xa2;
	x[4] = 0xd5;
	x[5] = 0xff;
}
#endif

void
fetch_uboot_mac ( char *addr )
{
	struct emac *ep = EMAC_BASE;
	// char *addr = emac_mac;

	unsigned long mac_hi = ep->mac_addr[0].hi;
	unsigned long mac_lo = ep->mac_addr[0].lo;

	/* This is just from good old U-Boot
	kprintf ( "MAC addr %d: %08x %08x\n", ep->mac_addr[0].hi, ep->mac_addr[0].lo );
	MAC addr 0: 00008c26 9b7f2002
	*/
	*addr++ = mac_lo & 0xff;
	mac_lo >>= 8;
	*addr++ = mac_lo & 0xff;
	mac_lo >>= 8;
	*addr++ = mac_lo & 0xff;
	mac_lo >>= 8;
	*addr++ = mac_lo & 0xff;

	*addr++ = mac_hi & 0xff;
	mac_hi >>= 8;
	*addr++ = mac_hi & 0xff;
}

/* We have been pulling our hair out getting the emac properly
 * initialized from scratch, so as an "end run" approach, we
 * just try using much of the initialization we inherit from U-Boot.
 */
int
emac_init_new ( struct dentry *devptr )
{
	emac_wait_flag = 0;
	rx_count = 0;
	tx_count = 0;
	last_capture = 0;
	last_len = 0;
	int_count = 0;
	rx_int_count = 0;
	tx_int_count = 0;
	first_int = 1;




	struct	ethcblk *ethptr;		/* Ethernet control blk pointer	*/
	ethptr = &ethertab[devptr->dvminor];
	struct emac *ep = EMAC_BASE;
	int i;
	int reg;

	// validate my structure layout
	kprintf ( "Shoud be 0xd0 == 0x%x\n", &ep->rgmii_status );
	verify_regs ();

	/* We inherit clocks and such from U-boot, thus far anyway ... */
	// ccu_emac ();

	// phy_uboot ();
	// verify_regs ();
	//syscon_setup ();
	// show_sid ();

	kprintf ( "Emac init\n" );
	kprintf ( " *************************** Hello from the Emac driver\n" );

	emac_sem = semcreate(1);

	// phy_init ();	// 1-10-2023

// #define INHERIT_UBOOT
#ifndef INHERIT_UBOOT
	// This used to just leave the emac useless, but is working now
	// 1-16-2023
	// kprintf ( "Reset the emac\n" );
// 	emac_reset ();
#endif

	// show_sid ();

	// This does no harm
	// phy_update ();

	// NEW from old raw init.
	ep->ctl1 = CTL1_BURST_8;

	kprintf ( "emac rx_desc = %08x\n", ep->rx_desc );
	kprintf ( "emac tx_desc = %08x\n", ep->tx_desc );

	// rx_list_show ( (struct emac_desc *) ep->rx_desc, NUM_RX_UBOOT );

	kprintf ( "emac CTL0 = %08x\n", ep->ctl0 );
	kprintf ( "emac CTL1 = %08x\n", ep->ctl1 );
	kprintf ( "emac RX CTL0 = %08x\n", ep->rx_ctl0 );
	kprintf ( "emac RX CTL1 = %08x\n", ep->rx_ctl1 );
	kprintf ( "emac TX CTL0 = %08x\n", ep->tx_ctl0 );
	kprintf ( "emac TX CTL1 = %08x\n", ep->tx_ctl1 );
	// kprintf ( "emac rx_filt = %08x\n", ep->rx_filt );

	// for ( i=0; i<8; i++ ) {
	for ( i=0; i<1; i++ ) {
	    kprintf ( "MAC addr from U-Boot %d: %08x %08x\n",
		i, ep->mac_addr[i].hi, ep->mac_addr[i].lo );
	}

	/* The mac address registers read as all ones, except the first.
	 * MAC addr 0: 00008c26 9b7f2002
	 * MAC addr 0: 00 00 8c 26 9b 7f 20 02
	 * our MAC address on the wire is: 02:20:7f:9b:26:8c with U-Boot
	 * When running linux however:     c2:c2:9b:ae:f9:5e
	 *
	 * XXX - using this so called "linux" MAC address works but is
	 * totally bogus.  The end result of using this wired in address
	 * will be that every Orange Pi on my network will have the same
	 * MAC address.
	 */

#ifdef USE_UBOOT_MAC
	fetch_uboot_mac ( emac_mac );
#else
	// get_mac ( emac_mac );
	fetch_linux_mac ( emac_mac );
	set_mac ( emac_mac );
	kprintf ( "*** Using BOGUS linux MAC address ***\n" );
#endif

	kprintf ( "MAC addr in use: %08x %08x\n", ep->mac_addr[0].hi, ep->mac_addr[0].lo );

	// this would yield promiscuous mode ?? */
	// ep->rx_filt = RX_FILT_DIS;

	ep->rx_ctl1 |= TX_MD;
	ep->tx_ctl1 |= RX_MD;

	/* Shut down receiver and receive DMA */
	ep->rx_ctl0 &= ~RX_EN;
	ep->rx_ctl1 &= ~RX_DMA_ENA;

	/* Shut down transmitter and transmit DMA */
	ep->tx_ctl0 &= ~TX_EN;
	ep->tx_ctl1 &= ~TX_DMA_ENA;

	/* Set up interrupts */
	set_irq_handler(IRQ_EMAC, (uint32)devptr->dvintr);
	set_irq_handler(IRQ_DMA, (uint32)devptr->dvintr);
// 	irq_hookup ( IRQ_EMAC, emac_handler, 0 );
	kprintf("Completed IRQ hookup\n");

	init_rings (devptr);
	kprintf("Completed init_rings\n");
	emac_debug();

	/* the "emac_activate" entry point really kicks things off */

#ifndef INHERIT_UBOOT
// 	phy_init ();	// 1-10-2023
	// phy_init_10 ();	// FAILS 1-12-2023
#endif

	// never hurts
// 	phy_update ();

	// This seems to be 0 after reset
	kprintf("before addr read\n");
	kprintf ( "emac CTL0 (orig) = %08x\n", ep->ctl0 );
	kprintf("after addr read\n");

#ifndef INHERIT_UBOOT
	/* interestingly when inheriting 100 Mbit from U-boot 2022.10
	 * I see this register 0xc -- I usually set it to 0xd
	 * when I leave it 0xc I get RxEARLY interrupts, which
	 * is the interesting part
	 */
#endif
	reg = 0;
	if ( phy_get_speed () )
	    reg |= CTL_SPEED_100;
	if ( phy_get_duplex () )
	    reg |= CTL_FULL_DUPLEX;
	ep->ctl0 = reg;
	ep->ctl0 = CTL_SPEED_100 | CTL_FULL_DUPLEX;
	kprintf ( "emac CTL0 (new) = %08x\n", ep->ctl0 );

	return 1;
}

/* Get things going.
 * Called by emac_activate()
 */
void
emac_enable ( void )
{
	struct emac *ep = EMAC_BASE;

	/* Linux driver enables these three */
	ep->int_ena = INT_RX | INT_TX | INT_TX_UNDERFLOW;
// 	ep->int_ena = INT_RX_ALL | INT_TX_ALL | INT_TX_UNDERFLOW;
	// ep->int_ena = INT_RX_ALL;

	rx_start ();
	tx_start ();
}

void
tx_dma_start ( void )
{
	struct emac *ep = EMAC_BASE;

	/* Poke the Tx DMA */
	ep->tx_ctl1 |= TX_DMA_START;
}

#ifdef notdef
/* U-Boot uses this rather than tx_poll()
 */
void
tx_restart ( void )
{
	struct emac *ep = EMAC_BASE;

	/* Restart Tx DMA */
	ep->tx_ctl1 |= TX_DMA_START | TX_DMA_ENA;

	/* Enable the transmitter */
	ep->tx_ctl0 |= TX_EN;
}
#endif

void
tx_start ( void )
{
	struct emac *ep = EMAC_BASE;

	/* Restart Tx DMA */
	ep->tx_ctl1 |= TX_DMA_ENA;

	/* Enable the transmitter */
	ep->tx_ctl0 |= TX_EN;
}

void
rx_start ( void )
{
	struct emac *ep = EMAC_BASE;

	/* Restart Rx DMA */
	ep->rx_ctl1 |= RX_DMA_ENA | RX_DMA_START & ~RX_NO_FLUSH;

	/* Enable the receiver */
	ep->rx_ctl0 |= RX_EN;
}

#ifdef notdef
-- /* Stuff pertaining to U-Boot harvesting */
--
-- /* Figure out where list activity starts, if there is any.
--  * Note that DMA is active while this is running,
--  * so things can change out from under us.
--  * This is not an issue unless the list fills since
--  * DMA fiddles with the end while we look for the start.
--  */
-- int
-- scan_rcv_list ( struct emac_desc *list )
-- {
-- 	struct emac_desc *edp;
-- 	int index = -1;
-- 	int ready0 = 0;
-- 	int wait = 0;
-- 	int i;
-- 	int num;
--
-- 	if ( ! (list->status & DS_ACTIVE) ) {
-- 	    ready0 = 1;
-- 	    wait = 1;
-- 	}
--
-- 	i = 0;
-- 	for ( edp = list;; edp = edp->next ) {
-- 	    if ( edp->next == list )
-- 		break;
-- 	    if ( wait && ! (edp->status & DS_ACTIVE) )
-- 		continue;
-- 	    wait = 0;
-- 	    if ( ! (edp->status & DS_ACTIVE) ) {
-- 		index = i;
-- 		break;
-- 	    }
-- 	    i++;
-- 	}
--
-- 	/* detect idle list */
-- 	if ( index < 0 && ready0 == 0 ) {
-- 	    kprintf ( "Scan finds idle list\n" );
-- 	    return -1;
-- 	}
--
-- 	/* detect full list */
-- 	if ( index < 0 && ready0 == 1 && wait == 1 ) {
-- 	    num = i;
-- 	    kprintf ( "Rcv list found full with %d entries\n", num );
-- 	    for ( i=0; i<num; i++ )
-- 		list[i].status = DS_ACTIVE;
-- 	    return -1;
-- 	}
--
-- 	// kprintf ( "scan ends with %d %d\n", index, ready0 );
-- 	if ( index < 0 )
-- 	    return 0;
-- 	return index;
-- }
--
-- int first_poll = 1;
-- struct emac_desc *cur_rx_dma;
-- struct emac_desc *cur_tx_dma;
--
-- void
-- emac_rcv_poll ( void )
-- {
-- 	struct emac *ep = EMAC_BASE;
-- 	struct emac_desc *list;
-- 	struct emac_desc *edp;
-- 	int index;
-- 	int len;
-- 	int i_dma;
--
-- 	list = (struct emac_desc *) ep->rx_desc;
--
-- 	invalidate_dcache_range ( (void *) list, &list[NUM_RX_UBOOT] );
--
-- 	// rx_list_show ( (struct emac_desc *) ep->rx_desc, NUM_RX_UBOOT );
--
-- 	if ( first_poll ) {
-- 	    index = scan_rcv_list ( list );
-- 	    if ( index < 0 ) {
-- 		kprintf ( "Rx list empty at startup\n" );
-- 		return;
-- 	    }
-- 	    cur_rx_dma = &list[index];
-- 	    first_poll = 0;
-- 	}
--
-- 	if ( cur_rx_dma->status & DS_ACTIVE ) {
-- 	    // rx_list_show ( (struct emac_desc *) ep->rx_desc, NUM_RX_UBOOT );
-- 	    // kprintf ( "Rx list empty on poll\n" );
-- 	    return;
-- 	}
--
-- 	while ( ! (cur_rx_dma->status & DS_ACTIVE) ) {
-- 	    i_dma = (cur_rx_dma - list);
-- 	    len = (cur_rx_dma->status >> 16) & 0x3fff;
-- 	    kprintf ( " ---- buf %d (%08x) status: %08x  %d/%d %08x %08x\n",
-- 		i_dma, cur_rx_dma, cur_rx_dma->status, len, cur_rx_dma->size, cur_rx_dma->buf, cur_rx_dma->next );
-- 	    kyu_receive ( cur_rx_dma->buf, len );
-- 	    cur_rx_dma->status = DS_ACTIVE;
-- 	    cur_rx_dma = cur_rx_dma->next;
-- 	}
-- }
#endif

/* Not truly official -- called at 1 Hz from "net_slow"
 */
void
emac_poll ( void )
{
	// Don't need this is we are using interrupts
	// emac_rcv_poll ();
}

/* My orange Pi seems to be 196.168.0.61
 * with MAC address 02:20:7f:9b:26:8c
 */

/* These are the "official" production entry points to this driver.
 */


void
emac_activate ( void )
{
	kprintf ( "Emac activated\n" );

	emac_enable ();
}

/* Displayed at start of "n 1" command output */
void
emac_show ( void )
{
	kprintf ( "Emac int count: %d, rx/tx = %d/%d\n", int_count, rx_int_count, tx_int_count );
	kprintf ( "Emac rx_count: %d\n", rx_count );
	kprintf ( "Emac tx_count: %d\n", tx_count );
}

// void
// emac_show_last ( int show_bufs )
// {
// 	kprintf ( "Prior Rx: len: %d\n", prior_len );
// 	if ( show_bufs )
// 	    dump_buf ( prior_buf, prior_len );
//
// 	kprintf ( "Last Rx: int, desc, len: %08x %08x %d\n", last_stat, last_desc_stat, last_len );
// 	if ( show_bufs )
// 	    dump_buf ( last_buf, last_len );
// }

int first_drop;

void
emac_send_int ( struct netbuf *nbp, int wait_int )
{
        int len;

	// kprintf ( "Emac sending packet\n" );

	if ( ! phy_get_link() ) {
	    if ( first_drop ) {
		first_drop = 0;
		kprintf ( "PHY link down --------------- **\n" );
		kprintf ( "Dropping packet, will not send\n" );
		phy_show ();
		// emac_debug ();
		phy_update ();
	    }
	    return;
	}

#ifdef notdef
	if ( emac_busy ) {
	    kprintf ( "Emac, waiting to send\n" );
	    while ( emac_busy )
		sleep(1);
	    kprintf ( "Emac, done waiting .. sending ----\n" );
	} else
	    kprintf ( "Emac, clear -- sending packet -----\n" );
#endif

        ++tx_count;

        /* Put our ethernet address in the packet */
        memcpy ( nbp->eptr->src, emac_mac, ETH_ADDR_SIZE );

        len = nbp->ilen + sizeof(struct eth_hdr);

	/* statistics */
	// pkt_send ();

	INT_lock;
	// tx_cleaner ();

	if ( cur_tx_dma->next == clean_tx_dma ) {
	    kprintf ( " !!! *** Tx ring full, packet not sent\n" );
	    INT_unlock;
	    return;
	}

	// kprintf ( "Sending %d bytes\n", len );
        // dump_buf ( nbp->eptr, len );

	memcpy ( cur_tx_dma->buf, (char *) nbp->eptr, len );

	// flush_dcache_range ( (unsigned long ) cur_tx_dma->buf, (unsigned long) cur_tx_dma->buf + len );
	emac_cache_flush ( (unsigned long ) cur_tx_dma->buf, (unsigned long) cur_tx_dma->buf + len );

	/* ARP packets are small (42 bytes) and may get rejected unless padded.
	 */
        // if ( len < ETH_MIN_SIZE ) len = ETH_MIN_SIZE;

	// this would only be necessary if we were clearing and setting
	// bits like the silly U-Boot driver does.
	// invalidate_dcache_range ( (void *) cur_tx_dma, &cur_tx_dma[1] );

// #define DS_TX_SEND	( DS_TX_INT | DS_TX_LAST | DS_TX_FIRST )
#define DS_TX_SEND	( DS_TX_INT | DS_TX_LAST | DS_TX_FIRST | DS_TX_ADR_CHAIN )

	/* Note that the address length is limited to 11 bits (2048 bytes).
	 *  XXX - we could check that here.
	 */
	// cur_tx_dma->size = len | DS_TX_SEND;
	cur_tx_dma->size = (len & 0x7ff) | DS_TX_SEND;
	cur_tx_dma->status = DS_ACTIVE;

	// flush_dcache_range ( (void *) cur_tx_dma, &cur_tx_dma[1] );
	emac_cache_flush ( (void *) cur_tx_dma, &cur_tx_dma[1] );

	cur_tx_dma = cur_tx_dma->next;

	// tx_cleaner ();
	// emac_busy = 1;

	INT_unlock;

	// tx_list_show ();

// 	et_snd ();
	tx_dma_start ();

	if ( wait_int ) {
	    emac_wait_flag = 1;
	    wait ( emac_sem );
	}
}

void
emac_send ( struct netbuf *nbp )
{
	emac_send_int ( nbp, 0 );
}

void
emac_send_wait ( struct netbuf *nbp )
{
	emac_send_int ( nbp, 1 );
}

/* Displayed as "n 11" command output.
 *  more details than the above.
 */
void
emac_debug ( void )
{
	struct emac *ep = EMAC_BASE;

	/* Get current link status */
	phy_update ();

	kprintf ( "Emac int count: %d, rx/tx = %d/%d\n", int_count, rx_int_count, tx_int_count );
	kprintf ( "Emac rx_count / tx_count: %d / %d\n", rx_count, tx_count );

	kprintf ( " Tx list\n" );
	// tx_list_show ( tx_list, NUM_TX );
	tx_list_show ();

	kprintf ( " Rx list\n" );
	// rx_list_show ( rx_list, NUM_RX );
	rx_list_show ();

	kprintf ( "emac RX CTL0 = %08x\n", ep->rx_ctl0 );
	kprintf ( "emac RX CTL1 = %08x\n", ep->rx_ctl1 );
	kprintf ( "emac TX CTL0 = %08x\n", ep->tx_ctl0 );
	kprintf ( "emac TX CTL1 = %08x\n", ep->tx_ctl1 );

	kprintf ( "emac Tx DMA status: %08x\n", ep-> tx_dma_stat );
	kprintf ( "emac Tx DMA cur desc: %08x\n", ep-> tx_dma_cur_desc );
	kprintf ( "emac Tx DMA cur buf: %08x\n", ep-> tx_dma_cur_buf );

	kprintf ( "emac Rx DMA status: %08x\n", ep-> rx_dma_stat );
	kprintf ( "emac Rx DMA cur desc: %08x\n", ep-> rx_dma_cur_desc );
	kprintf ( "emac Rx DMA cur buf: %08x\n", ep-> rx_dma_cur_buf );

	kprintf ( "emac Rx DMA dec list addr: %08x\n", ep-> rx_desc);

// 	emac_show_last ( 1 );

	/* Again, since it gets lost in the above */
	phy_update ();
// 	struct emac_desc* desc = (struct emac_desc*) ep->rx_desc;
// 	desc[13].status = ETH_AW_RX_DESC_CTL;
// 	emac_enable();
}

/* THE END */
