#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<stdint.h>	/*for uint32_t*/
#include<sys/socket.h>
#include<features.h>
#include<linux/if_packet.h>
#include<linux/if_ether.h>
#include<errno.h>
#include<ctype.h>
#include<sys/ioctl.h>
#include<net/if.h>
#include<net/ethernet.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"inject.h"

extern int g_additional_len;
extern int g_is_gw_flag;
extern uint8_t curr_session_id;
extern int packet_len;

unsigned char* create_gnsm_packet(uint8_t *src_mac, uint8_t *dst_mac, int protocol, char ip_address[15], char node_name[10], uint8_t is_gw) {

    char wan_if[10];
    struct ethhdr *eth_header = NULL;
    unsigned char *buf = NULL, *packet = NULL, *start = NULL;

    /*Create ethernet header -> Src MAC, Dest MAC, Protocol type*/
    eth_header = (struct ethhdr *) malloc(sizeof (struct ethhdr) + (sizeof (char) * BUFFER_LEN));
    create_ethernet_header(eth_header, src_mac, dst_mac, protocol);
    buf = (unsigned char *) ((char *) eth_header + sizeof (struct ethhdr));
    start = (char *) eth_header;

    /* Pack SRC_MAC, IP_ADD, Node_name */
    buf += pack_curr_node_info(buf, ip_address, src_mac, node_name);
    /*Check for presence of GW and pack*/
    /*Read the name of the WAN interface from WAN_IF_FILE_NAME file*/
    get_wan_if_name(wan_if);
    /*Execute ping command to check internet connectivity and set the is_gw flag accordingly*/
    is_gw = (uint8_t) set_gw_status(wan_if);
    /*Write the GW status into IS_GW_FILE_NAME for reading by the sniffer*/
    write_gw_status_into_file(is_gw);
    /*Pack the GW flag status*/
    buf += pack(buf, (uint8_t *) & is_gw, 1);
#if PRINT_PACKET
    printf("[%x]", (uint8_t)*(buf - 1));
    fflush(stdout);
#endif
    /*Pack DNS and call iptables scripts */
    buf += handle_dns_and_iptables(buf, is_gw);
    /*Pack txstats information*/
    buf += read_and_pack_txstats(buf);
    /*Pack rxstats information*/
    buf += read_and_pack_rxstats(buf);
    /*Return packet*/
    packet_len = buf - start;
    packet = (char *) eth_header;
    return ((unsigned char *) packet);

}
