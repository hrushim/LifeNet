/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Project: MyMANET - A Platform to Build Customized MANET's
Developers: Hrushikesh Mehendale, Ashwin Paranjpe, Santosh Vempala
Georgia Institute of Technology, Atlanta, USA
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>	/*for uint32_t*/
#include<sys/socket.h>
#include<features.h>
#include<linux/if_packet.h>
#include<linux/if_ether.h>
#include<errno.h>
#include<sys/ioctl.h>
#include<net/if.h>
#include<net/ethernet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include"sniffer.h"

void parse_ethernet_packet(unsigned char *packet, int len, uint8_t gw_status) {

    struct ethhdr *ethernet_header;
    char node_name[10] = "";
    uint8_t originator_mac[6] = {0};
    uint8_t am_i_gw = 0;
    char dns_ip1[16], dns_ip2[16], dns_ip3[16];

    uint32_t ip_long_nbyte_order = 0;
    uint32_t tx_pkts_nbyte_order = 0;
    uint32_t tx_session_nbyte_order = 0;

    int is_a_gateway = 0; /*This field indicates whether the current heart-beat packet has originated from a node that is a GW.*/
    int num_dns_ip = 0;

    /*Check basic packet size: Packet has to contain SRC_MAC (1 byte X 6), IP Address (32 bits), Node_name (MAX_NODE_NAME_SIZE), IS_A_GW flag (1 byte), #tx_packets (32 bits), #session_id (32 bits) at minimum*/
    if (len < (sizeof (struct ethhdr) + (sizeof (uint8_t) * 6) + MAX_NODE_NAME_SIZE + 1 + (sizeof (uint32_t) * 3))) {
        printf("\nSniffer: Packet size too small ! Garbled packets observed.\n\n");
        return;
    }
    /*Extract Ethernet header*/
    if (extract_ethernet_header(packet, ethernet_header) != 1) return;
    packet += sizeof (struct ethhdr);
    len -= sizeof (struct ethhdr);
    /*Extract Basic HB information - SRC_MAC, IP_ADD, NODE_NAME*/
    packet += extract_basic_hb_info(packet, (char *) & originator_mac, &ip_long_nbyte_order, node_name, &len);
    /*Extract GW information, IS_A_GW, DNS_IPs*/
    packet += extract_gw_info(packet, &is_a_gateway, &num_dns_ip, dns_ip1, dns_ip2, dns_ip3, &len);
    /*Check the host and update if required*/
    if (check_and_add_new_host(originator_mac, ntohl(ip_long_nbyte_order), node_name/*, ntohl(tx_pkts_nbyte_order), ntohl(tx_session_nbyte_order)*/) == -1) {
        printf("\nThis is an invalid host (might be having duplicate IP). Do not consider its statlist. Return.");
        return;
    }
    /*Extract host Tx information*/
    packet += extract_host_tx_info(packet, originator_mac);
    /*Extract host Rx information*/
    packet += extract_host_rx_info(packet, originator_mac, &len);
#if DEBUG
    print_gnst();
#endif
    /*Check GW status of itself*/
    am_i_gw = (uint8_t) my_gw_status();
#if DEBUG
    printf("\nAm i GW? %d\n", am_i_gw);
    fflush(stdout);
#endif
    /*Process GW*/
    process_gw_info(am_i_gw, is_a_gateway, ip_long_nbyte_order, num_dns_ip, dns_ip1, dns_ip2, dns_ip3);
    /*START: Code for printing in statlist and statlist_refined files.*/
    calculate_distances();
#if IS_EMB_DEV
#else
    write_hosts();
    write_gnst();
    write_refined_stats();
#endif
}

