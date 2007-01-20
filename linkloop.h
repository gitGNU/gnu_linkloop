/*
 * Copyright (C) 2004, 2005, 2006, 2007 Oron Peled
 *
 * Some code snippets adapted from spak
 * (http://www.xenos.net/software/spak/)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef	LINKLOOP_H
#define	LINKLOOP_H

#include "config.h"
#include <inttypes.h>
#include <net/if_arp.h>
#include <net/ethernet.h>

#define	COPYRIGHT_NOTICE	\
		"\nCopyright (C) 2003, 2005, 2006 Oron Peled\n" \
		"This is free software.  You may redistribute copies of it under the terms of\n" \
		"the GNU General Public License <http://www.gnu.org/licenses/gpl.html>.\n" \
		"There is NO WARRANTY, to the extent permitted by law.\n\n"

#define	PACKED		__attribute__((packed))
#define	PACK_SIZE	1514	/* This is what HPUX sends and responds to */
#define	PAYLOAD_SIZE	(PACK_SIZE - sizeof(struct llc) - sizeof(struct ether_header))

/* Describe an 802.2LLC packet */
struct llc {
	unsigned char dsap;
	unsigned char ssap;
	unsigned char ctrl;
} PACKED;

struct llc_packet {
	struct ether_header eth_hdr;
	struct llc	llc;
	/* Some test data */
	unsigned char data[PAYLOAD_SIZE];
} PACKED;

#define	TEST_CMD	0xE3	/* From 802.2LLC (HPUX sends 0xF3) */

void dump_packet(const struct llc_packet *pack);
char *mac2str(const uint8_t *s);
int parse_address(uint8_t mac[], const char *str);
void get_hwaddr(int sock, const char name[], uint8_t mac[]);
void mk_test_packet(struct llc_packet *pack, const uint8_t src[], const uint8_t dst[], size_t len, int response);
void send_packet(int sock, const char iface[], struct llc_packet *pack);
int recv_packet(int sock, struct llc_packet *pack);

#endif	/* LINKLOOP_H */
