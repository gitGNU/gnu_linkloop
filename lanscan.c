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
#include <net/if.h>
#include "config.h"
#include "linkloop.h"

static const char rcsid[] = "$Id: lanscan.c,v 1.1 2007/01/20 22:29:39 oron Exp $";

#define	MAX_IFACES	20
#define	OPTSTRING	"V"
const char *program = NULL;
const char *arg;

void usage()
{
	fprintf(stderr, "Usage: %s [option...]\n"
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
		case 'V':
			printf("%s version %s\n%s", program, VERSION, COPYRIGHT_NOTICE);
			exit(0);
		default:
			fprintf(stderr, "%s: unknown option code 0x%x\n", program, c);
		case '?':
			usage();
		}
	if(optind != argc) {
		fprintf(stderr, "%s: Extra paramaters\n", program);
		usage();
	}
	arg = argv[optind];
}

#define	IFF(x)	{ IFF_ ## x, #x }

struct if_flag {
	short int flag;
	char *flag_name;
} if_flags[] = {
	IFF(ALLMULTI),
	IFF(BROADCAST),
	IFF(DEBUG),
	IFF(LOOPBACK),
	IFF(NOARP),
	IFF(NOTRAILERS),
	IFF(POINTOPOINT),
	IFF(PROMISC),
	IFF(RUNNING),
	IFF(UP),
#ifdef	LINUX
	IFF(AUTOMEDIA),
	IFF(MASTER),
	IFF(SLAVE),
	IFF(PORTSEL),
#endif
#ifdef	HPUX
	IFF(ALT_SR8025),
	IFF(AR_SR8025),
	IFF(CANTCHANGE),
	IFF(CKO),
	IFF(CKO_ETC),
	IFF(LFP),
	IFF(LNP),
	IFF(LOCALSUBNETS),
	IFF(MULTI_BCAST),
	IFF(MULTICAST),
	IFF(NOACC),
	IFF(NOSR8025),
	IFF(OACTIVE),
	IFF(UNNUMBERED),
#endif
};
#undef	IFF

short int get_ifflags(int sock, const char name[])
{
	struct ifreq ifr;

	strncpy(ifr.ifr_name, name, IF_NAMESIZE - 1);
	if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
		perror("ioctl(SIOCGIFFLAGS)");
		exit(1);
	}
	return ifr.ifr_flags;
}

int main(int argc, char * const argv[])
{
	struct ifconf ifc;
	struct ifreq ifr_x[MAX_IFACES];
	int sock, err;
	uint8_t myMAC[IFHWADDRLEN];

	handle_options(argc, argv);
	if ((sock = socket(PF_PACKET, SOCK_PACKET, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	ifc.ifc_len = MAX_IFACES * sizeof(struct ifreq);
	ifc.ifc_req = ifr_x;
	if((err = ioctl(sock, SIOCGIFCONF, &ifc)) < 0) {
		perror("ioctl");
		exit(1);
	}
	printf("retrieved info for %i interface(s)\n", ifc.ifc_len / sizeof(struct ifreq));
	for (err = 0; err < ifc.ifc_len / sizeof(struct ifreq); err++) {
		int i;
		short int flags = get_ifflags(sock, ifr_x[err].ifr_name);
		get_hwaddr(sock, ifr_x[err].ifr_name, myMAC);
		printf("%s\t%s\t<", ifr_x[err].ifr_name, mac2str(myMAC));
		for(i = 0; i < sizeof(if_flags)/sizeof(if_flags[0]); i++)
			if(flags & if_flags[i].flag)
				printf(" %s", if_flags[i].flag_name);
		printf(" >\n");
	}
	return EXIT_SUCCESS;
}
