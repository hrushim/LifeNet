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

void write_packet_traces() {

#if LOG_STATS
    FILE *fp = NULL;
    FILE *fptr_wdl = NULL;
    char filename[100];
    char line[100][400];
    int line_cnt = 0;
    char buffer[5];
    time_t mytime = time(0);

    fptr_wdl = fopen(WDL_FILE_NAME, "r");
    if (fptr_wdl == NULL) {
        printf("\n %s could not be read", WDL_FILE_NAME);
        exit(1);
    }

    memset(&line, '\0', 100 * 400);
    memset(&buffer, '\0', sizeof (buffer));

    while ((fgets(line[line_cnt], 400, fptr_wdl) != NULL)) {

        buffer[0] = line[line_cnt][0];
        buffer[1] = line[line_cnt][1];
        buffer[2] = line[line_cnt][2];
        buffer[3] = line[line_cnt][3];
        buffer[4] = '\0';

        if (strncmp(buffer, "PATH", strlen(buffer)) != 0) continue;
        else {
            printf("\n%s", line[line_cnt]);
            line_cnt++;
        }

    }

    fclose(fptr_wdl);

    /*
#if IS_EMB_DEV
    sprintf(filename, "%s", TRACE_FILE_PATH);
#else
    sprintf(filename, "%s%s", getenv("HOME"), TRACE_FILE_PATH);
#endif
    if ((fp = fopen(filename, "a")) == NULL) {
        printf("\nError: %s file could not be opened.\n", filename);
        return;
    }

    int i;
    fprintf(fp, "\n NR\n");
    for (i = 0; i < line_cnt; i++) {
        mytime = time(0);
        char temp[100];
        memset(&temp, '\0', sizeof (temp));
        strncpy(temp, (const char *) asctime(localtime(&mytime)), sizeof (temp));
        temp[strlen(temp) - 1] = '\0';
        fprintf(fp, "%s %s", temp, line[i]);
        fflush(fp);
    }

    fclose(fp);
     */
#endif
}

void write_gnst() {

    int i, j;
    FILE *fp = NULL;
    char filename[100];

    memset(filename, '\0', 100);

#if IS_EMB_DEV
    sprintf(filename, "%s", GNST_PATH);
#else
    sprintf(filename, "%s%s", getenv("HOME"), GNST_PATH);
#endif
    if ((fp = fopen(filename, "w")) == NULL) {
        printf("\nError: %s file could not be opened.\n", filename);
        return;
    }

    for (i = 0; i < g_host_count; i++) {
        for (j = 0; j < g_host_count; j++) {
            if (i != j) {

                int num_tx, tx_session, num_rx, rx_session, num_bcast, num_rx_bcast;
                tx_session = (int) g_stats[i][j].tx_session;
                //                if ((g_bcast_stats[i].bcast_session - g_stats[i][j].tx_session) == 1 || (g_bcast_stats[i].bcast_session - g_stats[i][j].tx_session) == 0 || (g_bcast_stats[i].bcast_session - g_stats[i][j].tx_session) == -1) {
                //                  num_tx = g_stats[i][j].num_tx + g_bcast_stats[i].num_bcast;
                //                  num_rx = g_stats[i][j].num_rx;
                //                  rx_session = g_stats[i][j].rx_session;
                //              } else {
                num_tx = g_stats[i][j].num_tx;
                num_bcast = g_bcast_stats[i].num_bcast;
                num_rx = g_stats[i][j].num_rx;
                num_rx_bcast = g_stats[i][j].num_bcast_rx;
                rx_session = g_stats[i][j].rx_session;
                //             }

                fprintf(fp, "%s %s", hostlist[i].node_name, hostlist[j].node_name);
                fprintf(fp, " #tx %d #tx_sess %d #rx %d #rx_sess %d #bcast %d #bcast_rx %d timestamp %d #bcast_sess %d \n", num_tx, tx_session, num_rx, rx_session, num_bcast, num_rx_bcast, (uint32_t) hostlist[i].timestamp, g_bcast_stats[i].bcast_session);
            }
        }
    }

    fprintf(fp, "\n");
    if (fp)
        fclose(fp);
}

void write_hosts() {

    int i, j;
    FILE *fp = NULL;
    char filename[100];

    memset(filename, '\0', 100);

#if IS_EMB_DEV
    sprintf(filename, "%s", HOSTLIST_PATH);
#else
    sprintf(filename, "%s%s", getenv("HOME"), HOSTLIST_PATH);
#endif
    if ((fp = fopen(filename, "w")) == NULL) {
        printf("\nError: %s file could not be opened.\n", filename);
        return;
    }

    for (i = 0; i < g_host_count; i++) {

        struct in_addr temp_ip_in_addr;
        temp_ip_in_addr.s_addr = hostlist[i].ip_long;
        fprintf(fp, "ID %d Name %s IP %s MAC %x:%x:%x:%x:%x:%x  MyMAC %x:%x:%x:%x:%x:%x \n", i, (char *) hostlist[i].node_name, (char *) inet_ntoa(temp_ip_in_addr), hostlist[i].hostmac[0], hostlist[i].hostmac[1], hostlist[i].hostmac[2], hostlist[i].hostmac[3], hostlist[i].hostmac[4], hostlist[i].hostmac[5], g_mac[0], g_mac[1], g_mac[2], g_mac[3], g_mac[4], g_mac[5]);

    }

    if (fp)
        fclose(fp);
}

void write_refined_stats() {
#if LOG_STATS
    int i, j;
    char filename[500];

    memset(filename, '\0', 500);

#if IS_EMB_DEV
    sprintf(filename, "%s", HOSTLIST_PATH);
#else
    sprintf(filename, "cat %s%s >> %s%s", getenv("HOME"), GNST_PATH, getenv("HOME"), STATS_REFINED_FILE_PATH);
#endif

    system(filename);

#if IS_EMB_DEV
    sprintf(filename, "%s", HOSTLIST_PATH);
#else
    sprintf(filename, "date >> %s%s", getenv("HOME"), STATS_REFINED_FILE_PATH);
#endif

    system(filename);
#endif
}
