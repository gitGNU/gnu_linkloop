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

static const char rcsid[] = "$Id: common.c,v 1.1 2007/01/20 22:29:39 oron Exp $";

#include <netinet/in.h>		/* for htons(3)			*/

#include <net/if.h>		/* for IFNAMSIZ, IFHWADDRLEN */
#include <sys/ioctl.h>		/* for SIOCGIFHWADDR		*/
#include <stdio.h>
#include <string.h>     	/* for memcmp(3) memset(3)	*/
#include <stdlib.h>		/* for exit(3)			*/
#include <errno.h>
#include <assert.h>
#include "linkloop.h"
#if HAVE_ETHER_HOSTTON
#include <netinet/ether.h>	/* for ether_hostton(3)		*/
#endif

/* These are used to define different data structs... */
#if IFHWADDRLEN != ETH_ALEN
#error "SOMETHING IS VERY FISHY: IFHWADDRLEN != ETH_ALEN"
#endif

int debug_flag = 0;

void dump_packet(const struct llc_packet *pack)
{
	int i;
	uint8_t *p = (uint8_t *)pack;
	size_t len = ntohs(pack->eth_hdr.ether_type);

	printf("PACKET DUMP: data size=%d (0x%x)", len, len);
	for(i = 0; i < len; i++, p++) {
		if((i % 16) == 0)
			printf("\n%04x\t", i);
		printf("%02x ", (unsigned)*p);
	}
	printf("\nEND PACKET DUMP\n");
}

char *mac2str(const uint8_t s[])
{
	static char buf[3 * IFHWADDRLEN + 1];

	sprintf (buf, "%02X:%02X:%02X:%02X:%02X:%02X",
			s[0], s[1], s[2], s[3], s[4], s[5]);
	return buf;
}

#if 0
/* FIXME: use ether_ntoa() instead of mac2str() */
char *ether_ntoa(const struct ether_addr *addr);
#endif

int parse_address(uint8_t mac[], const char *str)
{
	unsigned a, b, c, d, e, f;
	struct ether_addr ea;

	if(sscanf(str,"%x:%x:%x:%x:%x:%x", &a, &b, &c, &d, &e, &f) == 6) {
		/* A colon separated notation */
		mac[0] = (unsigned char) a;
		mac[1] = (unsigned char) b;
		mac[2] = (unsigned char) c;
		mac[3] = (unsigned char) d;
		mac[4] = (unsigned char) e;
		mac[5] = (unsigned char) f;
	} else if(sscanf(str,
			"0x%02x%02x%02x%02x%02x%02x", &a, &b, &c, &d, &e, &f) == 6) {
		/* Hexadecimal notation (like HPUX) */
		mac[0] = (unsigned char) a;
		mac[1] = (unsigned char) b;
		mac[2] = (unsigned char) c;
		mac[3] = (unsigned char) d;
		mac[4] = (unsigned char) e;
		mac[5] = (unsigned char) f;
#if HAVE_ETHER_HOSTTON
	} else if(ether_hostton(str, &ea) == 0) {
		/* A name from /etc/ethers */
		memcpy(mac, ea.ether_addr_octet, 8);
#endif
	} else
		return 0;
	return 1;
}

void get_hwaddr(int sock, const char name[], uint8_t mac[])
{
	struct ifreq ifr;

	/* Make sure we have space for null terminator */
	strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name) - 1);
	if(ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
		perror("ioctl(SIOCGIFHWADDR)");
		exit(1);
	}
	memcpy(mac, ifr.ifr_hwaddr.sa_data, IFHWADDRLEN);
}

void mk_test_packet(struct llc_packet *pack, const uint8_t src[], const uint8_t dst[], size_t len, int response)
{
	int i;

	if(len > PACK_SIZE) {		/* 0x05DC == 1500 */
		fprintf(stderr, "packet too long (%d bytes)\n", len);
		exit(1);
	}
	if(debug_flag)
		printf("Create test packet (%d bytes)\n", len);
	memcpy(pack->eth_hdr.ether_dhost, dst, sizeof(pack->eth_hdr.ether_dhost));
	memcpy(pack->eth_hdr.ether_shost, src, sizeof(pack->eth_hdr.ether_shost));
	pack->eth_hdr.ether_type = htons(len);
	pack->llc.dsap = (response) ? 0x80 : 0x00;
	pack->llc.ssap = (response) ? 0x01 : 0x80;	/* XNS? */
	pack->llc.ctrl = TEST_CMD;			/* TEST */
	/* Payload */
	// FIXME: ? len -= sizeof(struct llc) + sizeof(struct ether_header);
	for(i = 0; i < len; i++)
		pack->data[i] = i;
}

void send_packet(int sock, const char iface[], struct llc_packet *pack)
{
	struct sockaddr sa;
	int ret;

	memset((char *)&sa, 0, sizeof(sa));
	sa.sa_family = AF_INET;
	strncpy(sa.sa_data, iface, sizeof(sa.sa_data) - 1);
	/* Send the packet */
	ret = sendto(sock, pack, sizeof(*pack), 0, (struct sockaddr *)&sa, sizeof(sa));
	if(ret == -1) {
		perror("sendto");
		exit(1);
	}
	if(ret != sizeof(*pack))
		fprintf(stderr, "Warning: Incomplete packet sent\n");
	if(debug_flag)
		printf("sent TEST packet to %s\n", mac2str(pack->eth_hdr.ether_dhost));
}

int recv_packet(int sock, struct llc_packet *pack)
{
	struct sockaddr sa;
	socklen_t len;
	int ret;

	len = sizeof(sa);
	ret = recvfrom(sock, pack, sizeof(*pack), 0, (struct sockaddr *)&sa, &len);
	if(ret == -1) {
		if(errno == EINTR)	/* We have a timeout */
			return 0;
		perror("recvfrom");
		exit(1);
	}
	if((pack->llc.ctrl & TEST_CMD) != TEST_CMD) {
		fprintf(stderr, "got unexpected packet\n");
		dump_packet(pack);
		exit(1);
	}
	ret = ntohs(pack->eth_hdr.ether_type);
	if(debug_flag)
		printf("received TEST packet (%d bytes) from %s\n", ret, mac2str(pack->eth_hdr.ether_shost));
	return ret;
}

