/*
 * Copyright (C) 2023 Mitchell Augustin <augustm@purdue.edu>,  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 *
 * allwinner_eth.c for the Orange Pi PC and PC Plus
 * Adapted from Tom Trebisky's Kyu - https://github.com/trebisky/Kyu/blob/master/orange_pi/emac.c
 *
 * Mitchell Augustin	9/18/2023	10/25/2023
 * Tom Trebisky  3/20/2017  8/2/2020
 *
 * allwinner_eth.c
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
#include <stdio.h>
#include <stdlib.h>

struct	ethcblk ethertab[1];
struct	ethcblk *ethptr;		/* Ethernet control blk pointer	*/
struct	eth_aw_csreg *csrptr;	/* Ethernet CSR pointer		*/
void emac_show ( void );
void tx_start ( void );
void rx_start ( void );

byte global_mac_addr[6];
int global_mac_addr_set;

/* ------------------------------------------------------------ */
/* Descriptors */
/* ------------------------------------------------------------ */

#define NUM_RX	64
#define NUM_TX	64


struct emac_desc *rx_list;
struct emac_desc *tx_list;

struct emac_desc *cur_rx_dma;

struct emac_desc *cur_tx_dma;
struct emac_desc *clean_tx_dma;

int emac_wait_flag;
sid32 emac_sem;

void allwinner_eth_debug ( void );

/*------------------------------------------------------------------------
 * rx_list_show - prints the RX ring
 *------------------------------------------------------------------------
 */
void rx_list_show ( void )
{
	struct emac_desc *desc = rx_list;
	int num = NUM_RX;

	struct emac_desc *edp;
	int len;
	int i;

	emac_cache_invalidate ( (unsigned long) desc, (unsigned long) &desc[num] );

	for ( i=0; i<num; i++ ) {
	    edp = &desc[i];
	    len = (edp->status >> 16) & 0x3fff;
	    if ( edp == cur_rx_dma )
		kprintf ( "* Rx Buf %2d (%08x) status: %08x  %d/%d %08x %08x\n", i, edp, edp->status, len, edp->size, edp->buf, edp->next );
	    else
		kprintf ( "  Rx Buf %2d (%08x) status: %08x  %d/%d %08x %08x\n", i, edp, edp->status, len, edp->size, edp->buf, edp->next );
	}
}

/*------------------------------------------------------------------------
 * tx_list_show - prints the TX ring
 *------------------------------------------------------------------------
 */
void tx_list_show ( void )
{
	struct emac_desc *desc = tx_list;
	int num = NUM_TX;

	struct emac_desc *edp;
	int i;

	emac_cache_invalidate ( (unsigned long) desc, (unsigned long) &desc[num] );

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
/*------------------------------------------------------------------------
 * rx_list_init - Initializes the RX ring
 *------------------------------------------------------------------------
 */
struct emac_desc *rx_list_init ( void )
{
	struct emac_desc *edp;
	char *buf;
	struct emac_desc *desc;

	// kprintf ( "Descriptor size: %d bytes\n", sizeof(struct emac_desc) );

	/* We can depend on getmem to give us dma aligned addresses */
	desc = (struct emac_desc *) getmem ( NUM_RX * sizeof(struct emac_desc) );
	buf = (char *) getmem ( NUM_RX * RX_SIZE );
	if ((int)desc == SYSERR || (int)buf == SYSERR) {
		kprintf("ERORR: rx_list_init failed in device/eth/allwinner_eth.c\n");
		return NULL;
	}
// 	kprintf("desc: %08x, buf: %08x\n", desc, buf);
// 	kprintf("NUM_RX * sizeof(struct emac_desc): %d\n", (NUM_RX * sizeof(struct emac_desc)));
// 	kprintf("NUM_RX * RX_SIZE: %d\n", (NUM_RX * RX_SIZE));
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

	emac_cache_flush ( (unsigned long) desc, (unsigned long) &desc[NUM_RX] );
	// rx_list_show ( desc, NUM_RX );

	return desc;
}

/*------------------------------------------------------------------------
 * reset_rx_list - Empties the RX ring and makes it available for DMA
 *------------------------------------------------------------------------
 */
void reset_rx_list ( struct emac_desc *list, int num )
{
	struct emac_desc *edp;

	list->status = DS_ACTIVE;
	for ( edp = list->next; edp != list; edp = edp->next )
	    edp->status = DS_ACTIVE;

	emac_cache_flush ( (unsigned long) list, (unsigned long) &list[num] );
}

/*------------------------------------------------------------------------
 * tx_list_init - Initializes the TX ring
 *------------------------------------------------------------------------
 */
struct emac_desc *tx_list_init ( void )
{
	struct emac_desc *edp;
	struct emac_desc *desc;
	char *buf;

	/* We can depend on getmem to give us dma aligned addresses */
	desc = (struct emac_desc *) getmem ( NUM_TX * sizeof(struct emac_desc) );
	buf = (char *) getmem ( NUM_TX * TX_SIZE );
	if ((int)desc == SYSERR || (int)buf == SYSERR) {
		kprintf("ERORR: rx_list_init failed in device/eth/allwinner_eth.c\n");
		return NULL;
	}

	for ( edp = desc; edp < &desc[NUM_TX]; edp ++ ) {
	    edp->status = DS_ACTIVE;
	    edp->size = 0;
	    edp->buf = buf;
	    edp->next = &edp[1];
	    buf += TX_SIZE;
	}

	desc[NUM_TX-1].next = &desc[0];

	emac_cache_flush ( (unsigned long) desc, (unsigned long) &desc[NUM_TX] );

	return desc;
}

/*------------------------------------------------------------------------
 * init_rings - Configures RX/TX rings for DMA
 *------------------------------------------------------------------------
 */
void init_rings ( struct dentry *devptr )
{
// 	kprintf("Starting ring init\n");

	void *desc;

	desc = rx_list_init ();
	rx_list = desc;
	ethptr->rxRing = rx_list;
	ethptr->rxHead = 0;
	ethptr->rxTail = 0;
	ethptr->rxRingSize = NUM_RX;
	/* Reload the dma pointer register.
	 * This causes the dma list pointer to get reset.
	 */
	csrptr->rx_dma_desc_list = (reg32) desc;
// 	kprintf("csrptr->rx_dma_desc_list: %d\n", csrptr->rx_dma_desc_list);
	cur_rx_dma = desc;

	/* Now set up the Tx list */
	desc = tx_list_init ();
	tx_list = desc;
	ethptr->txRing = tx_list;
	ethptr->txHead = 0;
	ethptr->txTail = 0;
	ethptr->txRingSize = NUM_TX;

	clean_tx_dma = cur_tx_dma = desc;
	csrptr->tx_dma_desc_list = (reg32) desc;
// 	kprintf("Completed ring init\n");
}

/* Interestingly the emac can accomodate 8 MAC addresses.
 * All but the first must have a bit set to indicate they
 *  are active.
 */
/*------------------------------------------------------------------------
 * set_mac - Sets the MAC address stored in the H3 EMAC CSR to mac_id
 *------------------------------------------------------------------------
 */
void set_mac ( char *mac_id )
{
	csrptr->mac_addr[0].hi = mac_id[4] + (mac_id[5] << 8);
	csrptr->mac_addr[0].lo = mac_id[0] + (mac_id[1] << 8) +
	    (mac_id[2] << 16) + (mac_id[3] << 24);
}

// seems to take 2 milliseconds
// actually takes 10 ticks
#define SOFT_RESET_TIMEOUT	500

/*------------------------------------------------------------------------
 * emac_reset - Soft-resets the EMAC
 *------------------------------------------------------------------------
 */
void emac_reset ( void )
{
	int tmo = SOFT_RESET_TIMEOUT;

	csrptr->basic_ctl_1 |= CTL1_SOFT_RESET;

	while ( tmo-- && csrptr->basic_ctl_1 & CTL1_SOFT_RESET )
		;
		//thr_delay ( 1 );

	// kprintf ( "Emac Reset cleared in %d milliseconds\n", (SOFT_RESET_TIMEOUT-tmo) );
// 	kprintf ( "Emac Reset cleared in %d ticks\n", (SOFT_RESET_TIMEOUT-tmo) );
}


/* Post this here so our genereic network stack can fetch it.
 * We use it locally also.
 */
char emac_mac[ETH_ADDR_SIZE];

/* For the outside world */
void get_emac_addr ( char *addr )
{
	memcpy ( addr, emac_mac, ETH_ADDR_SIZE );
}

/*------------------------------------------------------------------------
 * fetch_random_mac - Generates a pseudorandom MAC address based on your default hardware seed
 * Note: The Orange Pi does not have a "true" MAC address embedded in the hardware - instead, it is up to the OS or bootloader
 * to generate one at runtime. If you are using uboot to boot Xinu, uboot will generate its own MAC address, then Xinu will generate
 * its own. If you have connection issues due to the randomized MAC, you can change occurrences of fetch_random_mac to fetch_linux_mac
 * and change the bytes of the MAC to that of your uboot MAC address.
 *------------------------------------------------------------------------
 */
void fetch_random_mac ( char *addr )
{
	if (!global_mac_addr_set) {
		*addr++ = 0x02; //MUST have 2 bit set and 1 bit clear
		global_mac_addr[0] = *(addr - 1);
		*addr++ = rand() % 256;
		global_mac_addr[1] = *(addr - 1);
		*addr++ = rand() % 256;
		global_mac_addr[2] = *(addr - 1);
		*addr++ = rand() % 256;
		global_mac_addr[3] = *(addr - 1);
		*addr++ = rand() % 256;
		global_mac_addr[4] = *(addr - 1);
		*addr++ = rand() % 256;
		global_mac_addr[5] = *(addr - 1);
		global_mac_addr_set = 1;
	}
	else {
		*addr++ = global_mac_addr[0]; //MUST have 2 bit set and 1 bit clear
		*addr++ = global_mac_addr[1];
		*addr++ = global_mac_addr[2];
		*addr++ = global_mac_addr[3];
		*addr++ = global_mac_addr[4];
		*addr++ = global_mac_addr[5];
	}
}

/*------------------------------------------------------------------------
 * fetch_linux_mac - Sets addr to the hardcoded static MAC address below
 *------------------------------------------------------------------------
 */
void fetch_linux_mac ( char *addr )
{
	*addr++ = 0x02;
	*addr++ = 0x20;
	*addr++ = 0xcc;
	*addr++ = 0xa2;
	*addr++ = 0xd5;
	*addr++ = 0xff;
}

/*------------------------------------------------------------------------
 * fetch_uboot_mac - Sets addr to the MAC address left in CSR by UBoot (Note: networking did not work properly when I tried using this)
 *------------------------------------------------------------------------
 */
void fetch_uboot_mac ( char *addr )
{
	// char *addr = emac_mac;
	unsigned long mac_hi = csrptr->mac_addr[0].hi;
	unsigned long mac_lo = csrptr->mac_addr[0].lo;

	int i;
	for (i = 0; i < 5; ++i) {
		unsigned long mac_hii = csrptr->mac_addr[i].hi;
		unsigned long mac_loi = csrptr->mac_addr[i].lo;
		kprintf("%d:\n", i);
		kprintf("uboot mac_hi: %08x\n", mac_hii);
		kprintf("uboot mac_lo: %04x\n", mac_loi);
	}


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

/*------------------------------------------------------------------------
 * allwinner_eth_init - Initializes allwinner EMAC. Called from ethinit() after Phy is initialized.
 *------------------------------------------------------------------------
 */
int allwinner_eth_init ( struct dentry *devptr )
{
	ethptr = &ethertab[devptr->dvminor];
	csrptr = ethptr->csr;

	// validate my structure layout
// 	kprintf ( "Shoud be 0xd0 == 0x%x\n", &csrptr->rgmii_stat );
// 	verify_regs ();

	/* We inherit clocks and such from U-boot, thus far anyway ... */
	// ccu_emac ();

	// phy_uboot ();
	// verify_regs ();
	//syscon_setup ();
	// show_sid ();

// 	kprintf ( "Emac init\n" );
// 	kprintf ( " *************************** Hello from the Emac driver\n" );

	emac_sem = semcreate(1);

	// phy_init ();	// 1-10-2023


	// show_sid ();

	// This does no harm
	// phy_update ();

	// NEW from old raw init.
	csrptr->basic_ctl_1 = CTL1_BURST_8;

// 	kprintf ( "emac rx_desc = %08x\n", csrptr->rx_dma_desc_list );
// 	kprintf ( "emac tx_desc = %08x\n", csrptr->tx_dma_desc_list );

	// rx_list_show ( (struct emac_desc *) csrptr->rx_dma_desc_list, NUM_RX_UBOOT );

// 	kprintf ( "emac CTL0 = %08x\n", csrptr->basic_ctl_0 );
// 	kprintf ( "emac CTL1 = %08x\n", csrptr->basic_ctl_1 );
// 	kprintf ( "emac RX CTL0 = %08x\n", csrptr->rx_ctl_0 );
// 	kprintf ( "emac RX CTL1 = %08x\n", csrptr->rx_ctl_1 );
// 	kprintf ( "emac TX CTL0 = %08x\n", csrptr->tx_ctl_0 );
// 	kprintf ( "emac TX CTL1 = %08x\n", csrptr->tx_ctl_1 );
	// kprintf ( "emac rx_filt = %08x\n", ep->rx_filt );

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

	fetch_random_mac ( emac_mac );
	set_mac ( emac_mac );

// 	kprintf ( "MAC addr in use: %08x %08x\n", csrptr->mac_addr[0].hi, csrptr->mac_addr[0].lo );

	// this would yield promiscuous mode ?? */
	// ep->rx_filt = RX_FILT_DIS;

	csrptr->rx_ctl_1 |= TX_MD;
	csrptr->tx_ctl_1 |= RX_MD;

	/* Shut down receiver and receive DMA */
	csrptr->rx_ctl_0 &= ~RX_EN;
	csrptr->rx_ctl_1 &= ~RX_DMA_ENA;

	/* Shut down transmitter and transmit DMA */
	csrptr->tx_ctl_0 &= ~TX_EN;
	csrptr->tx_ctl_1 &= ~TX_DMA_ENA;

	/* Set up interrupts */
	set_irq_handler(IRQ_EMAC, (uint32)devptr->dvintr);
	set_irq_handler(IRQ_DMA, (uint32)devptr->dvintr);

	/* Route interrupts to core 0 */
	set_irq_target(IRQ_EMAC, 0);
// 	irq_hookup ( IRQ_EMAC, emac_handler, 0 );
// 	kprintf("Completed IRQ hookup\n");

	init_rings (devptr);
// 	kprintf("Completed init_rings\n");
// 	allwinner_eth_debug();

	/* the "emac_activate" entry point really kicks things off */

	// This seems to be 0 after reset
// 	kprintf("before addr read\n");
// 	kprintf ( "emac CTL0 (orig) = %08x\n", csrptr->basic_ctl_0 );
// 	kprintf("after addr read\n");


	return 0;
}

/*------------------------------------------------------------------------
 * allwinner_eth_enable - Enables allwinner EMAC interrupts and starts receiver and transmitter
 *------------------------------------------------------------------------
 */
void allwinner_eth_enable ( void )
{
	/* Linux driver enables these three */
	csrptr->int_en = INT_RX | INT_TX | INT_TX_UNDERFLOW;
// 	csrptr->int_en = INT_RX_ALL | INT_TX_ALL | INT_TX_UNDERFLOW;
	// csrptr->int_en = INT_RX_ALL;

	rx_start ();
	tx_start ();
}

/*------------------------------------------------------------------------
 * tx_dma_start - Enables TX DMA
 *------------------------------------------------------------------------
 */
void tx_dma_start ( void )
{
	/* Poke the Tx DMA */
	csrptr->tx_ctl_1 |= TX_DMA_START;
}

/*------------------------------------------------------------------------
 * tx_start - Enables transmitter
 *------------------------------------------------------------------------
 */
void tx_start ( void )
{
	/* Restart Tx DMA */
	csrptr->tx_ctl_1 |= TX_DMA_ENA;

	/* Enable the transmitter */
	csrptr->tx_ctl_0 |= TX_EN;
}

/*------------------------------------------------------------------------
 * rx_start - Enables receiver
 *------------------------------------------------------------------------
 */
void rx_start ( void )
{
	/* Restart Rx DMA */
	csrptr->rx_ctl_1 |= RX_DMA_ENA | RX_DMA_START;

	/* Enable the receiver */
	csrptr->rx_ctl_0 |= RX_EN;
}

/* These are the "official" production entry points to this driver.
 */

/*------------------------------------------------------------------------
 * allwinner_eth_activate - Enables EMAC
 *------------------------------------------------------------------------
 */
void allwinner_eth_activate ( void )
{
// 	kprintf ( "Emac activated\n" );

	allwinner_eth_enable ();
}


/* Displayed as "emac" command output.
 *  more details than the above.
 */
/*------------------------------------------------------------------------
 * allwinner_eth_debug - Prints EMAC status and tx/rx rings
 *------------------------------------------------------------------------
 */
void allwinner_eth_debug ( void )
{
	/* Get current link status */
// 	phy_update ();

	kprintf ( " Tx list\n" );
	// tx_list_show ( tx_list, NUM_TX );
	tx_list_show ();

	kprintf ( " Rx list\n" );
	// rx_list_show ( rx_list, NUM_RX );
	rx_list_show ();

	kprintf ( "emac RX CTL0 = %08x\n", csrptr->rx_ctl_0 );
	kprintf ( "emac RX CTL1 = %08x\n", csrptr->rx_ctl_1 );
	kprintf ( "emac TX CTL0 = %08x\n", csrptr->tx_ctl_0 );
	kprintf ( "emac TX CTL1 = %08x\n", csrptr->tx_ctl_1 );

	kprintf ( "emac Tx DMA status: %08x\n", csrptr-> tx_dma_stat );
	kprintf ( "emac Tx DMA cur desc: %08x\n", csrptr-> tx_dma_cur_desc );
	kprintf ( "emac Tx DMA cur buf: %08x\n", csrptr-> tx_dma_cur_buf );

	kprintf ( "emac Rx DMA status: %08x\n", csrptr-> rx_dma_stat );
	kprintf ( "emac Rx DMA cur desc: %08x\n", csrptr-> rx_dma_cur_desc );
	kprintf ( "emac Rx DMA cur buf: %08x\n", csrptr-> rx_dma_cur_buf );

	kprintf ( "emac Rx DMA dec list addr: %08x\n", csrptr-> rx_dma_desc_list);

// 	emac_show_last ( 1 );

	/* Again, since it gets lost in the above */
// 	phy_update ();
// 	struct emac_desc* desc = (struct emac_desc*) csrptr->rx_dma_desc_list;
// 	desc[13].status = ETH_AW_RX_DESC_CTL;
// 	allwinner_eth_enable ();
}
