/*
 * Copyright (C) 2016  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 *
 * net.h -- Kyu
 *
 * Tom Trebisky  3-31-2005
 */

/*
#define DEBUG_IP
#define DEBUG_ARP
#define DEBUG_ARP_MUCHO
#define DEBUG_ICMP
#define DEBUG_UDP
#define DEBUG_TCP
*/

#include "netbuf.h"

#ifndef __KYU_NET_H__
#define __KYU_NET_H__

#define	PRI_NET_IN	20
#define	PRI_NET_OUT	21
#define	PRI_SLOW	22
#define	PRI_HW_INIT	23

#define DEBUG_TCP

#define ETH_ARP		0x0806
#define ETH_ARP_SWAP	0x0608

#define ETH_IP		0x0800
#define ETH_IP_SWAP	0x0008

#define ETH_ADDR_SIZE	6
#define ETH_MIN_SIZE	60
#define ETH_MAX_SIZE	1514

/* in netinet/in.h in bsd sources */
#define IPPROTO_ICMP	1
#define IPPROTO_TCP	6
#define IPPROTO_UDP	17

#define IP_BROADCAST	0xffffffff

typedef void (*ufptr) ( struct netbuf * );

char * ip2str ( unsigned char * );
char * ip2str32 ( uint32 );
char * ether2str ( unsigned char * );

void udp_hookup ( int, ufptr );
int get_ephem_port ( void );

/* This stuff is either static assigned,
 * or obtained via DHCP (except mac address)
 */
struct host_info {
	uint32 my_ip;
	uint32 net_mask;
	uint32 my_net;
	uint32 gate_ip;
	unsigned char our_mac[ETH_ADDR_SIZE];
};


struct eth_hdr {
    	unsigned char dst[ETH_ADDR_SIZE];
    	unsigned char src[ETH_ADDR_SIZE];
	unsigned short type;
};

/* these bit fields work only on little endian hardware.
 */
struct ip_hdr {
	unsigned char	hl:4,	/* header length */
			ver:4;	/* version */
	unsigned char tos;	/* type of service */
	short len;		/* total length */
	/* - */
	unsigned short id;
	short offset;
	/* - */
	unsigned char ttl;	/* time to live */
	unsigned char proto;	/* protocol */
	unsigned short sum;
	/* - */
	uint32 src;
	uint32 dst;
};

#define IP_OFFMASK		0x1fff
#define IP_OFFMASK_SWAP		0xff1f
#define IP_DF			0x4000	/* don't fragment */
#define IP_MF			0x2000	/* more fragments */

#define BOARD_RAM_START	0x40000000

/* Disable interrupts to lock section */
#define INT_lock	\
	asm volatile (	"mrs     r0, cpsr; \
			orr     r0, r0, #0xc0; \
			msr     cpsr, r0" ::: "r0" )

/* Enable interrupts to unlock section */
#define INT_unlock	\
	asm volatile (	"mrs     r0, cpsr; \
			bic     r0, r0, #0xc0; \
			msr     cpsr, r0" ::: "r0" )

#endif  /* __KYU_NET_H__ */
/*  THE END */
