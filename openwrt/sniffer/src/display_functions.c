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
#include<netinet/in.h>
#include<arpa/inet.h>
#include<math.h>
#include"sniffer.h"

void print_distances_from_me() {
    int receiver = 0;

    printf("\n\n\n\n========= Distance Table ==========\n");
    printf("\n%-20s", "CURRENT DISTANCES");

    while (receiver < g_host_count) {
        printf("%-20s", hostlist[receiver].node_name);
        receiver++;
    }
    receiver = 0;
    printf("\n%-20s", "FROM THIS HOST");
    while (receiver < g_host_count) {
        printf("%-20d", g_distance_table[receiver]);
        receiver++;
    }
    printf("\n");

}

void print_real_link_stats_table() {


    int sender = 0, receiver = 0;

    system("clear");
    printf("\nNumber of MyMANET nodes detected: %d\n\n", g_host_count);
    printf("\n===== Global Statistics Table =====\n\n\n");
    printf("%-20s", "SENDER / RECEIVER");

    while (receiver < g_host_count) {
        printf("%-20s", hostlist[receiver].node_name);
        receiver++;
    }

    printf("\n");
    while (sender < g_host_count) {
        printf("\n%-20s", hostlist[sender].node_name);
        receiver = 0;

        while (receiver < g_host_count) {
            if (sender != receiver) {
                int bcast_session = (int) g_bcast_stats[sender].bcast_session;
//                if ((bcast_session - (int) g_stats[sender][receiver].tx_session) == 1 || (bcast_session - (int) g_stats[sender][receiver].tx_session) == 0 || (bcast_session - (int) g_stats[sender][receiver].tx_session) == -1) {
//                    printf("%3d, %-15d", g_stats[sender][receiver].num_tx + g_bcast_stats[sender].num_bcast, g_stats[sender][receiver].num_rx);
//                } else {
                    printf("%3d, %-15d", g_bcast_stats[sender].num_bcast, g_stats[sender][receiver].num_bcast_rx);
//                }
            } else {
                printf("%-20s", "XXXX");
            }
            receiver++;
        }
        printf("\n");
        sender++;

    }
    print_distances_from_me();

}

void print_gnst() {
    int i = 0, j = 0;

    for (i = 0; i < g_host_count; i++) {
        for (j = 0; j < g_host_count; j++) {
            if (j != i) {
                printf("\nSRC %s DST %s (TX %d TX_S %d) (BX %d BX_S %d) (RX %d RX_S %d)", hostlist[i].node_name, hostlist[j].node_name, g_stats[i][j].num_tx, g_stats[i][j].tx_session, g_bcast_stats[i].num_bcast, g_bcast_stats[i].bcast_session, g_stats[i][j].num_rx, g_stats[i][j].rx_session);
            }
        }
    }

}