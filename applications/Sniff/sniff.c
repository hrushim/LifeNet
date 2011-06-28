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
Developers: Hrushikesh Mehendale, Ashwin Paranjpe, Santosh Vempala,
Georgia Institute of Technology, Atlanta, USA
 */

#include<stdio.h>
#include <arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>	//for uint8_t and uint32_t
#include<sys/socket.h>
#include<features.h>
#include<linux/if_packet.h>
#include<linux/if_ether.h>
#include<errno.h>
#include<sys/ioctl.h>
#include<net/if.h>
#include<time.h>
#include"sniffer.h"
#include <arpa/inet.h>
#include <netinet/in.h>

//struct global_stats g_stats[MAX_NUM_OF_HOSTS][MAX_NUM_OF_HOSTS];

uint8_t g_broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//uint8_t g_distance_table[MAX_NUM_OF_HOSTS][3];
//static uint8_t g_mac[6];

extern int manifold_routing(int, unsigned char *, int, int);
int (*g_pluggable_func_ptr)(int receiver, unsigned char recvmac[6], int s, int r) = NULL;

time_t current_time;
int g_my_id = -1;
int g_tolerance = 5;
int g_is_emb_dev = 0; /*1: Is an embedded device. 0: Is NOT an embedded device.*/
int g_host_count = 0;

/*argv[1] is the interface*/
/*argv[2] is the TOLERANCE parameter for kernel-space distance table updates.*/
/*argv[3] is the flag to indicate whether this node is a GW*/

/*argv[4] is the flag to indicate whether this is an embedded router having several constraints.
This is used for certain optimizations to conserve power and memory. */

int main(int argc, char **argv) {

    if (argc != 6) {
        printf("\nusage: sudo ./sniff [INTERFACE] [TOLERANCE] [IS_GW] [IS_EMB_DEV]\n\n");
        exit(0);
    }

    struct sockaddr_ll packet_info;
    struct timeval timeout;

    unsigned char packet_buffer[BUFFER_LEN];

    int raw_sock;
    int len;
    int packet_info_size = sizeof (packet_info);
    int retval = 0;

    /*Clear the percentage loss table and set values to -1*/
    memset(g_stats, -1, (sizeof (struct global_stats) * MAX_NUM_OF_HOSTS * MAX_NUM_OF_HOSTS));
    /*Initialize Time-Stamps*/
    initialize_stats_tables();
    /*Initialize Distance Table*/
    initialize_distances();
    /*Initialize the pluggable routing function.*/
    g_pluggable_func_ptr = &manifold_routing;
    /*Get the tolerance parameter*/
    g_tolerance = atoi(argv[2]);
    /*Get the optimization parameter for embedded devices*/
    g_is_emb_dev = atoi(argv[4]);
    /*Get the broadcasting threshold distance parameter*/
    bcast_fwd_threshold = atoi(argv[5]);
#if DEBUG
    if (g_is_emb_dev)
        printf("\nThis is a memory and CPU constrained embedded device. Optimizations switched ON.\n");
    else
        printf("\nThis is NOT a memory and CPU constrained embedded device. Optimizations switched OFF.\n");
#endif
    /*Create Raw socket */
    raw_sock = create_raw_socket(MANIFOLD_UPDATE_TYPE);
    /*Bind socket to interface */
    bind_raw_socket_to_interface(argv[1], raw_sock, MANIFOLD_UPDATE_TYPE, g_mac);
    /*Start Sniffing*/
    while (check_manifold_lkm_status() == 1) {


        fd_set fds;
        len = 0;
        retval = 0;
        timeout.tv_sec = SOCKET_SELECT_TIMEOUT;
        timeout.tv_usec = SOCKET_SELECT_TIMEOUT * 1000000;
        memset(packet_buffer, 0, BUFFER_LEN);

        /*Clear the descriptor set, add the new raw socket to the descriptor, check if it is ready for reading*/
        FD_ZERO(&fds);
        FD_SET(raw_sock, &fds);
        retval = select((raw_sock + 1), &fds, 0, 0, &timeout);
#if DEBUG
        printf("\nRetval: %d", retval);
        fflush(stdout);
#endif

        if (retval) {
            len = recvfrom(raw_sock, packet_buffer, BUFFER_LEN, 0, (struct sockaddr*) & packet_info, &packet_info_size);
#if DEBUG
            printf("\n\nRecvfrom len: %d", len);
            fflush(stdout);
#endif
        } else if (retval == -1) {
            perror("Error in select():");
            fflush(stderr);
            fflush(stdout);

        } else {

            sleep(1);

#if IS_EMB_DEV
#else
#if DEBUG
#else
#if PRINT_PACKET
#else
            //print_real_link_stats_table();
            //write_packet_traces();
#endif
#endif
#endif
        }
        if (len == -1) {

            perror("Recv from returned -1: ");
            fflush(stderr);
            fflush(stdout);

        } else if (len > 0) {
#if DEBUG
            /*A Packet has been received*/
            PrintPacketInHex(packet_buffer, len);
#endif
            /*Parse Ethernet Packet*/
            parse_ethernet_packet(packet_buffer, len, atoi(argv[3]));
        }

    }
    return (EXIT_SUCCESS);
}
