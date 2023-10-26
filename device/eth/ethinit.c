/* ethinit.c - ethinit, eth_phy_read, eth_phy_write, eth_phy_autonegotiate, eth_phy_reset, eth_syscon_setup */

#include <xinu.h>

struct	ethcblk ethertab[1];

/*-----------------------------------------------------------------------
 * eth_phy_read - read a PHY register
 *-----------------------------------------------------------------------
 */
int32	eth_phy_read (
		volatile struct	eth_aw_csreg *csr,/* CSR pointer	*/
		byte	regadr,	/* PHY Register number	*/
		byte	phyadr,	/* PHY address		*/
		uint32	*value	/* Pointer to value 	*/
	)
{

	/* Ethernet PHY has only 32 registers */
// 	kprintf("eth_phy_read 1\n");

	if(regadr > 31) {
// 		kprintf("eth_phy_read failure 1\n");
		return SYSERR;
	}

	/* Only 32 possible PHY addresses */

	if(phyadr > 31) {
// 		kprintf("eth_phy_read failure 2\n");
		return SYSERR;
	}

	/* Wait for the previous access to complete */

	while(csr->mii_cmd & ETH_AW_MII_BUSY);

	/* Start the access */

	csr->mii_cmd = (ETH_AW_MDC_DIV_RATIO_M_128) |
			    (phyadr << ETH_AW_MII_ADDR) |
			    (regadr << ETH_AW_MII_REG) |
					(ETH_AW_MII_BUSY);

	/* Wait until the access is complete */

	while(csr->mii_cmd & ETH_AW_MII_BUSY);

	/* Start the access */

	csr->mii_cmd = (ETH_AW_MDC_DIV_RATIO_M_128) |
			    (phyadr << ETH_AW_MII_ADDR) |
			    (regadr << ETH_AW_MII_REG) |
					(ETH_AW_MII_BUSY);

	/* Wait until the access is complete */

	while(csr->mii_cmd & ETH_AW_MII_BUSY);

	/* Check if the access was successful */
/*
	if( (csr->mii_data & ETH_AW_MII_DATA) == 0 ) {
		kprintf("eth_phy_read failure 3\n");
		return SYSERR;
	}*/

// 	kprintf("Here!\n");

	/* Copy the value read */

	(*value) = csr->mii_data & ETH_AW_MII_DATA;

	return OK;
}


/*-----------------------------------------------------------------------
 * eth_phy_write - write a PHY register
 *-----------------------------------------------------------------------
 */
int32	eth_phy_write (
		volatile struct	eth_aw_csreg *csr, /* CSR pointer	*/
		byte	regadr,	/* PHY register number	*/
		byte	phyadr,	/* PHY address		*/
		uint32	value	/* Value to be written	*/
	)
{
	/* There are only 32 PHY registers */

	if(regadr > 31) {
		return SYSERR;
	}

	/* There are only 32 possible PHY addresses */

	if(phyadr > 31) {
		return SYSERR;
	}

	/* Wait for the previous access to complete */

	while(csr->mii_cmd & ETH_AW_MII_BUSY);

	/* Start the access */

	csr->mii_data = value & ETH_AW_MII_DATA;
	csr->mii_cmd = ETH_AW_MDC_DIV_RATIO_M_128 |
			    ETH_AW_MII_WR |
			    (phyadr << ETH_AW_MII_ADDR) |
			    (regadr << ETH_AW_MII_REG) |
					(ETH_AW_MII_BUSY);

	/* Wait for the access to complete */

	while(csr->mii_cmd & ETH_AW_MII_BUSY);

	return OK;
}

/*-----------------------------------------------------------------------
 * eth_phy_autonegotiate - Auto-negotiate ethernet link
 * Based on Tom Trebisky's Kyu ethernet autonegotiation code
 * https://github.com/trebisky/Kyu/blob/master/orange_pi/emac_phy.c
 *-----------------------------------------------------------------------
 */
static void eth_phy_autonegotiate (
		volatile struct	eth_aw_csreg *csr, /* CSR pointer	*/
		byte	phyadr			  /* PHY Address	*/
								   ) {
	uint32 reg;
	int tmo = ANEG_TIMEOUT;

	eth_phy_read(csr, ETH_PHY_CTLREG, phyadr, &reg);
// 	kprintf ( "Ethernet PHY autonegotiation starting, BMCR = %04x\n", reg );
	reg |= BMCR_ANEG_ENA;
	eth_phy_write(csr, ETH_PHY_CTLREG, phyadr, reg);

	reg |= BMCR_ANEG;
	eth_phy_write(csr, ETH_PHY_CTLREG, phyadr, reg);
	eth_phy_read(csr, ETH_PHY_STATREG, phyadr, &reg);
// 	kprintf ( "autonegotiation started: BMSR  = %04x\n", reg );

	while (tmo-- && ! (reg & BMSR_ANEGCOMPLETE)) {
		DELAY(ETH_AW_INIT_DELAY);
		eth_phy_read(csr, ETH_PHY_STATREG, phyadr, &reg);
	}

	if (!(reg & BMSR_ANEGCOMPLETE)) {
		kprintf("Ethernet PHY Autonegotiation Failed\n");
	}
	else {
// 	    kprintf ( "autonegotion done: BMSR  = %04x\n", reg);
	    kprintf ( "Ethernet PHY autonegotion done in %d milliseconds\n", 2*(ANEG_TIMEOUT-tmo) );
	}
}

/*-----------------------------------------------------------------------
 * eth_phy_reset - Reset an Ethernet PHY
 *-----------------------------------------------------------------------
 */
int32	eth_phy_reset (
		volatile struct	eth_aw_csreg *csr, /* CSR pointer	*/
		byte	phyadr			  /* PHY Address	*/
	)
{
	uint32	phyreg;	/* Variable to hold ETH PHY register value	*/
	int32	retries;/* Number of retries				*/
	int32	retval;	/* Return value of functions called here	*/

	/* Read the PHY Control Register */

	retval = eth_phy_read(csr, ETH_PHY_CTLREG, phyadr, &phyreg);
	if(retval == SYSERR) {
		return SYSERR;
	}

	/* Set the Reset bit and write the register */

	phyreg |= ETH_PHY_CTLREG_RESET;
	eth_phy_write(csr, ETH_PHY_CTLREG, phyadr, phyreg);

	/* Check if Reset operation is complete */

	for(retries = 0; retries < 10; retries++) {
		if(eth_phy_read(csr, ETH_PHY_CTLREG, phyadr, &phyreg) == SYSERR) {
// 			kprintf("eth_phy_reset failure 1\n");
			return SYSERR;
		}
		if((phyreg & ETH_PHY_CTLREG_RESET) == 0) {
			break;
		}
		else {
			retries++;
			DELAY(ETH_AW_INIT_DELAY);
			continue;
		}
	}
	if(retries >= 3) {
// 		kprintf("eth_phy_reset failure 2\n");
		return SYSERR;
	}

	//Do link auto-negotiation
	eth_phy_autonegotiate(csr, phyadr);

	/* Check if the Link is established */

	for(retries = 0; retries < 10; retries++) {
		if(eth_phy_read(csr, ETH_PHY_STATREG, phyadr, &phyreg) == SYSERR) {
// 			kprintf("eth_phy_reset failure 3\n");
			return SYSERR;
		}

		if(phyreg & ETH_PHY_STATREG_LINK) {
// 			kprintf("breaking on phyreg: %d\n", phyreg);
			break;
		}
		else {
			retries++;
			DELAY(ETH_AW_INIT_DELAY);
			continue;
		}
	}
	if(retries >= 3) {
// 		kprintf("eth_phy_reset failure 4 - retries: %d\n", retries);
		return SYSERR;
	}

	return OK;
}

/* ------------------------------------------------------------ */
/* Syscon stuff - from Kyu by Tom Trebisky (https://github.com/trebisky/Kyu/blob/master/orange_pi/emac_phy.c#L534) */
/* ------------------------------------------------------------ */


/* We probably inherit all of this from U-Boot, but we
 * certainly aren't going to rely on that, are we?
 * I see: Emac Syscon =     00148000 before and after
 * the datasheet default is 00058000
 * The "1" is the PHY ADDR
 * The "5" going to "4" is clearing the shutdown bit.
 *
 * So even calling or having this is pointless, given what U-Boot
 * passes to us, but it is just "clean living" to perform all
 * of our own initialization.
 */
static void eth_syscon_setup ( void )
{
	volatile unsigned int *sc = EMAC_SYSCON;

	// printf ( "Emac Syscon = %08x\n", *sc );

	*sc = SYSCON_EPHY_INTERNAL | SYSCON_CLK24 | (ETH_AW_PHY_ADDR<<20);

	// I try using the 25 Mhz clock and autonegotion simply fails.
	// *sc = SYSCON_EPHY_INTERNAL | (ETH_AW_PHY_ADDR<<20);

	// printf ( "Emac Syscon = %08x\n", *sc );
}

/*-----------------------------------------------------------------------
 * ethinit - initialize the Allwinner H3 ethernet hardware
 *-----------------------------------------------------------------------
 */
int32	ethinit	(
		struct	dentry *devptr
	)
{
	// TODO
	struct	ethcblk *ethptr;		/* Ethernet control blk pointer	*/
	struct	eth_aw_csreg *csrptr;	/* Ethernet CSR pointer		*/
	uint32	phyreg;			/* Variable to store PHY reg val*/
	int32	retval;			/* Return value			*/
	int32	i;			/* Index variable		*/

	eth_syscon_setup();

	/* Get the Ethernet control block address	*/
	/* from the device table entry			*/

	ethptr = &ethertab[devptr->dvminor];

	/* Store the address of CSRs in the Ethernet control block	*/

	csrptr = (struct eth_aw_csreg *)devptr->dvcsr;
	ethptr->csr = csrptr;

	/* Reset internal ethernet registers */

	csrptr->basic_ctl_1 |= ETH_AW_SOFT_RST;
	while(csrptr->basic_ctl_1 & ETH_AW_SOFT_RST);

	/* Reset the PHY */

	retval = eth_phy_reset(csrptr, ETH_AW_PHY_ADDR);
	if(retval == SYSERR) {
		kprintf("Cannot reset Ethernet PHY\n");
		return SYSERR;
	}

	retval = eth_phy_read(csrptr, ETH_PHY_CTLREG, ETH_AW_PHY_ADDR, &phyreg);
	if(retval == SYSERR) {
		return SYSERR;
	}

	if( (phyreg & ETH_PHY_CTLREG_SM) == ETH_PHY_10M ) {
		kprintf("Ethernet Link is Up. Speed is 10Mbps\n");
	}
	else if( (phyreg & ETH_PHY_CTLREG_SM) == ETH_PHY_100M ) {
		kprintf("Ethernet Link is Up. Speed is 100Mbps\n");
	}
	else if( (phyreg & ETH_PHY_CTLREG_SM) == ETH_PHY_1000M ) {
		kprintf("Ethernet Link is Up. Speed is 1000Mbps\n");
	}
	else {
		return SYSERR;
	}

	if(phyreg & ETH_PHY_CTLREG_FD) {
		kprintf("Link is Full Duplex\n");
		csrptr->basic_ctl_0 |= ETH_AW_CTL_DUPLEX;
	}
	else {
		kprintf("Link is Half Duplex\n");
	}

	fetch_random_mac((char*) ethptr->devAddress);
//
	kprintf("MAC Address is: ");
	for(i = 0; i < 5; i++) {
		kprintf("%02X:", ethptr->devAddress[i]);
	}
	kprintf("%02X\n", ethptr->devAddress[5]);

// 	kprintf("Calling emac_init_new()");

	allwinner_eth_init(devptr);
// 	kprintf("Finished emac_init_new()");

	// Finish PHY setup
	csrptr->basic_ctl_0 = CTL_SPEED_100 | CTL_FULL_DUPLEX;
// 	kprintf ( "emac CTL0 (new) = %08x\n", csrptr->basic_ctl_0 );

	allwinner_eth_activate();

	ethptr->isem = semcreate(0);
	if((int32)ethptr->isem == SYSERR) {
		return SYSERR;
	}

	ethptr->osem = semcreate(ethptr->txRingSize);
	if((int32)ethptr->osem == SYSERR) {
		return SYSERR;
	}

// TODO Route interrupts to core 0
//	/* Route the interrupts to core 0 */
//	csrptr->wr->c0_tx_en = 0x1;
//	csrptr->wr->c0_rx_en = 0x1;

	return OK;
}
