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
#include"inject.h"

extern uint8_t g_mac[6];

int create_raw_socket(int protocol_to_send) {
    int raw_sock;

    if ((raw_sock = socket(PF_PACKET, SOCK_RAW, htons(protocol_to_send))) == -1) {
        perror("Error creating raw socket: ");
        exit(-1);
    }

    return raw_sock;
}

int bind_raw_socket_to_interface(char *device, int raw_sock, int protocol) {

    struct sockaddr_ll sll;
    struct ifreq ifr;
    unsigned char *mac;

    bzero(&sll, sizeof (sll));
    bzero(&ifr, sizeof (ifr));

    /*Copy device name*/
    strncpy((char *) ifr.ifr_name, device, IFNAMSIZ);

    /*Get hardware address */
    if (ioctl(raw_sock, SIOCGIFHWADDR, &ifr) < 0) {
        printf("ioctl Error: SIOCGIFHWADDR");
        exit(1);
    }

    memset(g_mac, 0, 6);
    memcpy(g_mac, ifr.ifr_hwaddr.sa_data, 6);

#if DEBUG
    printf("\nMy Mac is : %x:%x:%x:%x:%x:%x\n", g_mac[0], g_mac[1], g_mac[2], g_mac[3], g_mac[4], g_mac[5]);
#endif

    /*Get the Interface Index  */
    if ((ioctl(raw_sock, SIOCGIFINDEX, &ifr)) == -1) {
        printf("Error getting Interface index !\n");
        exit(-1);
    }

    /*Bind Raw socket to this interface*/
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifr.ifr_ifindex;
    sll.sll_protocol = htons(protocol);

    if ((bind(raw_sock, (struct sockaddr *) & sll, sizeof (sll))) == -1) {
        perror("Error binding raw socket to interface\n");
        exit(-1);
    }
    return 1;

}

int SendRawPacket(int rawsock, unsigned char *pkt, int pkt_len) {

    int sent = 0;
#if DEBUG
    printf("\nPacket len: %d\n\n", pkt_len);
#endif

    if ((sent = write(rawsock, pkt, pkt_len)) != pkt_len) {
        /* Print Error */
        printf("Could only send %d bytes of packet of length %d\n", sent, pkt_len);
        return 0;
    }

    return 1;

}
