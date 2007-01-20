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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include "config.h"
#include "linkloop.h"

static const char rcsid[] = "$Id: linkloop_reply.c,v 1.1 2007/01/20 22:29:39 oron Exp $";
static const char	*program = NULL;
static const char	*arg;
static char		*iface = "eth0";
static uint8_t		mac_src[IFHWADDRLEN];
static uint8_t		mac_dst[IFHWADDRLEN];
extern int		debug_flag;

#define	OPTSTRING	"di:V"

void usage()
{
	fprintf(stderr, "Usage: %s [option...]\n"
		"\t-d		Debug\n"
		"\t-i<iface>	Network interface\n"
		"\t-V		Print program version\n"
		, program
	);
	exit(1);
}

void handle_options(int argc, char * const argv[])
{
	int c;

	program = argv[0];
	while((c = getopt(argc, argv, OPTSTRING)) != -1)
		switch(c) {
		case 'd':
			debug_flag = 1;
			break;
		case 'i':
			iface = optarg;
			break;
		case 'V':
			printf("%s version %s\n%s", program, VERSION, COPYRIGHT_NOTICE);
			exit(0);
		default:
			fprintf(stderr, "%s: unknown option code 0x%x\n", program, c);
		case '?':
			usage();
		}
	if(debug_flag)
		fprintf(stderr, "%s: interface=%s\n", program, iface);
	if(optind != argc) {
		fprintf(stderr, "%s: Extra paramaters\n", program);
		usage();
	}
	arg = argv[optind];
}

int main(int argc, char *argv[])
{
	int sock;
	size_t len;
	struct llc_packet spack;
	struct llc_packet rpack;

	handle_options(argc, argv);

	/* Open a socket */
	if ((sock = socket(PF_PACKET, SOCK_PACKET, htons(ETH_P_802_2))) == -1) {
		perror("socket");
		return 1;
	}
	get_hwaddr(sock, iface, mac_src);
	printf ("Waiting for Test packets on %s (HW addr %s)\n", iface, mac2str(mac_src));
	do {
		len = recv_packet(sock, &rpack);
		memcpy(mac_dst, rpack.eth_hdr.ether_shost, IFHWADDRLEN);
		printf ("Got packet from: %s\n", mac2str(mac_dst));
		mk_test_packet(&spack, mac_src, mac_dst, len, 1);
		send_packet(sock, iface, &spack);
	} while(1);
	return EXIT_SUCCESS;
}
