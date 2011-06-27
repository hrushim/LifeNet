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
Developers: Ashwin Paranjpe, Hrushikesh Mehendale, Santosh Vempala
Georgia Institute of Technology, Atlanta, USA
 */


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
#include <netinet/in.h>
#include <arpa/inet.h>
#include"inject.h"

int g_is_gw_flag = 0;
int packet_len = 0;
uint8_t g_mac[6];
uint8_t curr_session_id = 0;

int main(int argc, char **argv) {

    int raw_sock; //handler for raw socket
    unsigned char *packet;
    int tmp = 0;

    if (argc != 5) {
        printf("\nUsage: ./inject [INTERFACE] [IP_ADDRESS] [NODE_NAME] [HB_INTERVAL]\n\n");
        exit(0);
    }
#if DEBUG
    printf("[INTERFACE] = %s\n[IP] = %s\n[NODE_NAME] = %s\n[HB_INTERVAL] = %s", argv[1], argv[2], argv[3], argv[4]);
#endif

#if DEBUG
    printf("\nRaw socket = %d", raw_sock);
#endif
    /*Create Raw socket*/
    raw_sock = create_raw_socket(MANIFOLD_UPDATE_TYPE);
#if DEBUG
    printf("\nRaw socket = %d", raw_sock);
#endif

    /*Bind raw socket to interface*/
    bind_raw_socket_to_interface(argv[1], raw_sock, MANIFOLD_UPDATE_TYPE);

    while (check_manifold_lkm_status()==1) {

        sleep(atoi(argv[4]));

        tmp = 0;

        packet = (unsigned char *) create_gnsm_packet((char *) & g_mac, (char *) DST_ETHER_ADDR, (int) MANIFOLD_UPDATE_TYPE, argv[2], argv[3], 0);

        if (packet == NULL) {
            printf("\nPacket is null");
            fflush(stdout);
            continue;
        }

#if DEBUG
        printf("\nPacket is :\n");
        while (tmp < (packet_len)) {
            printf("%x ", (int) (*(packet + tmp)));
            tmp++;
        }
        fflush(stdout);
#endif

        if (!SendRawPacket(raw_sock, packet, packet_len)) {
            perror("Error sending packet");
        }
        free(packet);
    }

    close(raw_sock);

    return (EXIT_SUCCESS);
}

