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
#include <sys/time.h>
#include <time.h>
#include"sniffer.h"

int get_id_from_mac(uint8_t mac[6]) {
    
    int i = 0;
    while (i < g_host_count) {
        if (memcmp(mac, hostlist[i].hostmac, 6) == 0)
            return i;
        i++;
    }
    return -1;
}

void PrintPacketInHex(unsigned char *packet, int len) {
    unsigned char *p = packet;
    printf("\n\n---------Packet---Starts----\n\n");
    while (len--) {
        printf("%.2x ", *p);
        p++;
    }

    printf("\n\n--------Packet---Ends-----\n\n");

}

PrintInHex(const char *mesg, unsigned char *p, int len) {
    printf("%s", mesg);

    while (len--) {
        printf("%.2X ", *p);
        p++;
    }

}

void initialize_timestamps() {
    /*
    int i = 0, j = 0;
    for (i = 0; i < MAX_NUM_OF_HOSTS; i++)
        for (j = 0; j < MAX_NUM_OF_HOSTS; j++)
            g_stats[i][j].timestamp = time(NULL);
     */
}

void initialize_distances() {
    int i = 0;
    for (i = 0; i < MAX_NUM_OF_HOSTS; i++) {
        g_distance_table[i][0] = MAX_DST;
        g_distance_table[i][1] = MAX_DST;
        g_distance_table[i][2] = MAX_DST;
    }
}

/* </editor-fold> */

int check_and_add_new_host(unsigned char srcmac[6], uint32_t ip_long, char node_name[10]/*, uint32_t tx_pkts, uint32_t tx_session*/) {

    /* <editor-fold defaultstate="collapsed" desc="Declarations & Initialization"> */

    char s[100];
    FILE *host_stream;
    int i = 0, found = 0;

    /* </editor-fold> */

    /* <editor-fold defaultstate="collapsed" desc="Log hosts information"> */

    char filename[100];
    memset(filename, '\0', 100);
#if IS_EMB_DEV
    sprintf(filename, "%s", HOSTS_FILE_PATH);
#else
    sprintf(filename, "%s%s", getenv("HOME"), HOSTS_FILE_PATH);
#endif
    if ((host_stream = fopen(filename, "w")) == NULL) {
        printf("\nError: %s could not be opened for appending.\n", filename);
        return;
    }

    for (i = 0; i < g_host_count; i++) {
        memset(&s, '\0', 100);
        struct in_addr temp_ip_in_addr;
        temp_ip_in_addr.s_addr = hostlist[i].ip_long;

        sprintf(s, "%s %s %x:%x:%x:%x:%x:%x %x:%x:%x:%x:%x:%x\n", (char *) hostlist[i].node_name, (char *) inet_ntoa(temp_ip_in_addr), hostlist[i].hostmac[0], hostlist[i].hostmac[1], hostlist[i].hostmac[2], hostlist[i].hostmac[3], hostlist[i].hostmac[4], hostlist[i].hostmac[5], g_mac[0], g_mac[1], g_mac[2], g_mac[3], g_mac[4], g_mac[5]);
        fprintf(host_stream, "%s", s);
    }
    fclose(host_stream);

    /* </editor-fold> */

    /* <editor-fold defaultstate="collapsed" desc="Check if host already existing or known. If yes, then return"> */

    i = 0;
    while (i < g_host_count) {
        if (memcmp(srcmac, hostlist[i].hostmac, 6) == 0) {

            memcpy(hostlist[i].hostmac, srcmac, 6);
            hostlist[i].ip_long = ip_long;
            memcpy(hostlist[i].node_name, node_name, 10);
            //hostlist[i].tx_pkts = tx_pkts;
            //hostlist[i].tx_session = tx_session;
            found = 1;

            struct timeval curr_time;
            gettimeofday(&curr_time, NULL);
            hostlist[i].timestamp = curr_time.tv_sec;
            return 0;

        }
        i++;
    }

    /* </editor-fold> */

    //If host is not known and the host list is not full
    if ((i == g_host_count) && (found == 0) && (g_host_count < MAX_NUM_OF_HOSTS)) {

        /* <editor-fold defaultstate="collapsed" desc="Check for IP conflict"> */

        /*Before adding the new host, check whether this is a duplicate IP address, If yes, return*/
        i = 0;
        while (i < g_host_count) {

            if (hostlist[i].ip_long == ip_long) {
                /*This host has a duplicate IP w.r.t. a node in MyMANET!. Do not add it.*/

                /*Check whether the host node itself is actually participating in an IP address conflict*/
                if ((memcmp(g_mac, srcmac, 6) == 0)) {

                    printf("\nNotification: IP ADDRESS CONFLICT DETECTED.\nPlease change IP and try again.\n");
#if 0
                    chdir("../../scripts");
                    char shell_comm[100];
                    memset(&shell_comm, '\0', 100);
                    sprintf(shell_comm, "bash regenerate_ip.sh %ld", ip_long);
                    system(shell_comm);
                    chdir("/tools/raw");
#endif
                    exit(1);

                }
                return -1;
            }
            i++;
        }

        /* </editor-fold> */

        /* <editor-fold defaultstate="collapsed" desc="If no IP conflict then add the new host in the list, return"> */

        /*Did not find the host in the list. It is a new host and will be added.*/
        memcpy(hostlist[g_host_count].hostmac, srcmac, 6);
        hostlist[g_host_count].ip_long = ip_long;
        memcpy(hostlist[g_host_count].node_name, node_name, 10);
//        hostlist[g_host_count].tx_pkts = tx_pkts;
//        hostlist[g_host_count].tx_session = tx_session;
        hostlist[g_host_count].timestamp = 0;

        struct timeval curr_time;
        gettimeofday(&curr_time, NULL);
        hostlist[g_host_count].timestamp = curr_time.tv_sec;

        g_host_count++;

        return 0;

        /* </editor-fold> */

    }/* <editor-fold defaultstate="collapsed" desc="Return if host list is full"> */

    else if (g_host_count >= MAX_NUM_OF_HOSTS) {

        printf("\nHost List is FULL\nCannot add more.\n");
        return -1;

    }

    /* </editor-fold> */

}

void initialize_stats_tables() {

    int i, j;

    for (i = 0; i < MAX_NUM_OF_HOSTS; i++) {
        for (j = 0; j < MAX_NUM_OF_HOSTS; j++) {
            g_stats[i][j].fwd_session = 0;
            g_stats[i][j].rx_session = 0;
            g_stats[i][j].tx_session = 0;
            g_stats[i][j].num_fwd = 0;
            g_stats[i][j].num_rx = 0;
            g_stats[i][j].num_tx = 0;
        }
        g_bcast_stats[i].bcast_session = 0;
        g_bcast_stats[i].fwd_session = 0;
        g_bcast_stats[i].num_bcast = 0;
        g_bcast_stats[i].num_fwd = 0;
    }

}

int maxnum(int num1, int num2) {
    if (num1 > num2) return num1;
    else return num2;
}

void calculate_distances() {

    int i, j;
    int tx_session;
    char command[100];

    memset(&command, '\0', 100);
    i = get_id_from_mac(g_mac);
    //g_distance_table[i] = 0;

    bcast_fwd_deno = 0;
    for (j = 0; j < g_host_count && i>=0; j++) {

        if (i != j) {

            struct timeval curr_time;
            gettimeofday(&curr_time, NULL);

            if ((hostlist[j].timestamp - curr_time.tv_sec) < 30) {

                tx_session = (int) g_bcast_stats[i].bcast_session;
                if (tx_session == g_stats[i][j].rx_session) {
                    //                 if (((int) g_bcast_stats[i].bcast_session - (int) g_stats[i][j].tx_session) == 1 || ((int) g_bcast_stats[i].bcast_session - (int) g_stats[i][j].tx_session) == 0 || ((int) g_bcast_stats[i].bcast_session - (int) g_stats[i][j].tx_session) == -1) {
                    //                     (*g_pluggable_func_ptr)(j, hostlist[j].hostmac, g_stats[i][j].num_tx + g_bcast_stats[i].num_bcast, g_stats[i][j].num_rx);
                    //                 } else {
                    //                    printf("\nCalling manifold function %d %d", g_bcast_stats[i].num_bcast, g_stats[i][j].num_bcast_rx);
                    (*g_pluggable_func_ptr)(j, hostlist[j].hostmac, g_bcast_stats[i].num_bcast, g_stats[i][j].num_bcast_rx);
                    //                 }
                }
            } else {//This means that heartbeats have not been received from that node for more than 30 secs
                //              printf("\nCalling manifold function %d %d", g_bcast_stats[i].num_bcast, 0);
                (*g_pluggable_func_ptr)(j, hostlist[j].hostmac, g_bcast_stats[i].num_bcast, 0);
            }


        }
    }
    sprintf(command, "echo set_fwd_threshold %d 0 > /proc/wdl", bcast_fwd_deno);
    system(command);
}

int extract_ethernet_header(unsigned char *packet, struct ethhdr *ethernet_header) {

    ethernet_header = (struct ethhdr *) packet;

#if PRINT_PACKET
    printf("\n==> [ %x %x %x %x %x %x ]", *packet, *(packet + 1), *(packet + 2), *(packet + 3), *(packet + 4), *(packet + 5));
    printf("[ %x %x %x %x %x %x ]", *(packet + 6), *(packet + 7), *(packet + 8), *(packet + 9), *(packet + 10), *(packet + 11));
    printf("[ %x %x ]", *(packet + 12), *(packet + 13));
    fflush(stdout);
#endif

    if (ethernet_header->h_proto != MANIFOLD_UPDATE_TYPE) {
        return 0;
    }

    if (memcmp(ethernet_header->h_dest, g_broadcast_mac, 6) != 0) {
        return 0;
    }
    packet = packet + sizeof (struct ethhdr);

    return 1;
}

int extract_basic_hb_info(unsigned char *packet, char *originator_mac, uint32_t *ip_long_nbyte_order, char *node_name, int *len) {

#if PRINT_PACKET
    printf("[ %x %x %x %x %x %x ]", *packet, *(packet + 1), *(packet + 2), *(packet + 3), *(packet + 4), *(packet + 5));
    printf("[ %x %x %x %x ]", *(packet + 6), *(packet + 7), *(packet + 8), *(packet + 9));
    printf("[ %x %x %x %x %x %x %x %x %x %x ]", *(packet + 10), *(packet + 11), *(packet + 12), *(packet + 13), *(packet + 14), *(packet + 15), *(packet + 16), *(packet + 17), *(packet + 18), *(packet + 19));
    fflush(stdout);
#endif

    memcpy(originator_mac, packet, 6);
    packet = packet + 6;
    *len -= 6;
    memcpy(ip_long_nbyte_order, packet, sizeof (uint32_t));
    packet = packet + sizeof (uint32_t);
    *len -= sizeof (uint32_t);
    memcpy(node_name, packet, 10);
    packet = packet + 10;
    *len -= 10;

    return 6 + 4 + 10;
}

int extract_gw_info(unsigned char *packet, int *is_a_gateway, int *num_dns_ip, char dns_ip1[16], char dns_ip2[16], char dns_ip3[16], int *len) {

#if PRINT_PACKET
    printf("[ %x ]", *packet);
    fflush(stdout);
#endif

    /*Check if the packet is coming from a GW node */
    uint32_t dns_ip_long1 = 0, dns_ip_long2 = 0, dns_ip_long3 = 0;
    uint32_t dns_ip_long = 0, dns_ip_long_nbyte_order = 0;
    int i = 0;
    int offset = 0;

    memcpy(is_a_gateway, packet, 1);
    packet = packet + 1;
    offset += 1;
    *len -= 1;

    /*Do the DNS IP extraction only the packet is coming from a GW node*/
    if (*is_a_gateway == 1) {
#if PRINT_PACKET
        printf("[ %x ]", *packet);
        fflush(stdout);
#endif
        /*Extract the # of DNS entries. MAX = 3*/
        memcpy(num_dns_ip, packet, 1);
        packet = packet + 1;

        *len -= 1;
        offset += 1;

        /*Extract DNS IP addresses and store them into dns_ip1, dns_ip2, dns_ip3 variables*/
        char *ip_ptr = NULL;
        for (i = 0; i < *num_dns_ip; i++) {
#if PRINT_PACKET
            printf("[ %x %x %x %x ]", *packet, *(packet + 1), *(packet + 2), *(packet + 3));
            fflush(stdout);
#endif
            memcpy(&dns_ip_long_nbyte_order, packet, sizeof (uint32_t));
            packet = packet + sizeof (uint32_t);
            offset += 4;
            *len -= sizeof (uint32_t);
            dns_ip_long = ntohl(dns_ip_long_nbyte_order);

            if (i == 0) {
                struct in_addr temp_ip_in_addr;

                dns_ip_long1 = dns_ip_long;
                temp_ip_in_addr.s_addr = dns_ip_long1;
                ip_ptr = (char *) inet_ntoa(temp_ip_in_addr);
                strncpy(dns_ip1, (char *) inet_ntoa(temp_ip_in_addr), 15);

            } else if (i == 1) {
                struct in_addr temp_ip_in_addr;

                dns_ip_long2 = dns_ip_long;
                temp_ip_in_addr.s_addr = dns_ip_long2;
                ip_ptr = (char *) inet_ntoa(temp_ip_in_addr);
                strncpy(dns_ip2, ip_ptr, 16);

            } else if (i == 2) {
                struct in_addr temp_ip_in_addr;

                dns_ip_long3 = dns_ip_long;
                temp_ip_in_addr.s_addr = dns_ip_long3;
                ip_ptr = (char *) inet_ntoa(temp_ip_in_addr);
                strncpy(dns_ip3, ip_ptr, 16);

            }
        }
    }
    return offset;
}

int update_gstats_with_tx_stats(uint8_t mac1[6], uint8_t mac2[6], uint32_t num_tx, uint32_t tx_session, uint32_t num_fwd, uint32_t fwd_session) {

    int index1 = get_id_from_mac(mac1), index2 = get_id_from_mac(mac2);
    uint8_t bcast_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    if (memcmp(mac2, bcast_mac, 6) == 0) {
        g_bcast_stats[index1].bcast_session = tx_session;
        g_bcast_stats[index1].num_bcast = num_tx;
        g_bcast_stats[index1].fwd_session = fwd_session;
        g_bcast_stats[index1].num_fwd = num_fwd;
    } else {
        if (index1 >= 0 && index2 >= 0) {
            g_stats[index1][index2].num_tx = num_tx;
            g_stats[index1][index2].tx_session = tx_session;
            g_stats[index1][index2].num_fwd = num_fwd;
            g_stats[index1][index2].fwd_session = fwd_session;
        }
    }

    /*
    if (memcmp(mac2, bcast_mac, 6) == 0) {
        if (tx_session >= g_bcast_stats[index1].bcast_session || (g_bcast_stats[index1].bcast_session - tx_session) > 1) {
            g_bcast_stats[index1].bcast_session = tx_session;
            g_bcast_stats[index1].num_bcast = num_tx;
        }
        if (fwd_session >= g_bcast_stats[index1].fwd_session || (g_bcast_stats[index1].fwd_session - fwd_session) > 1) {
            g_bcast_stats[index1].fwd_session = fwd_session;
            g_bcast_stats[index1].num_fwd = num_fwd;
        }
    } else {
        if (tx_session >= g_stats[index1][index2].tx_session || (g_stats[index1][index2].tx_session - tx_session) > 1) {
            g_stats[index1][index2].num_tx = num_tx;
            g_stats[index1][index2].tx_session = tx_session;
        }
        if (fwd_session >= g_stats[index1][index2].fwd_session || (g_stats[index1][index2].fwd_session - fwd_session) > 1) {
            g_stats[index1][index2].num_fwd = num_fwd;
            g_stats[index1][index2].fwd_session = fwd_session;
        }
    }
     */

}

int update_gstats_with_rx_stats(uint8_t mac1[6], uint8_t mac2[6], uint32_t num_rx, uint32_t num_bcast_rx, uint32_t rx_session) {

    int index1 = get_id_from_mac(mac1), index2 = get_id_from_mac(mac2);
    uint8_t bcast_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};


    struct timeval curr_time;
    gettimeofday(&curr_time, NULL);

    if ((hostlist[index1].timestamp - curr_time.tv_sec) < 30) {

        if (memcmp(mac2, bcast_mac, 6) != 0) {
            g_stats[index2][index1].num_rx = num_rx;
            g_stats[index2][index1].num_bcast_rx = num_bcast_rx;
            g_stats[index2][index1].rx_session = rx_session;
        }
    } else {
        if (index1 >= 0 && index2 >= 0) {
            g_stats[index2][index1].num_rx = 0;
            g_stats[index2][index1].num_bcast_rx = 0;
            g_stats[index2][index1].rx_session = 0;
        }
    }

    /*
    if (memcmp(mac2, bcast_mac, 6) != 0) {

        if (rx_session >= g_stats[index2][index1].rx_session || (g_stats[index2][index1].rx_session - rx_session) > 1) {
            g_stats[index2][index1].num_rx = num_rx;
            g_stats[index2][index1].rx_session = rx_session;
        }
    }
     */

}

int extract_host_tx_info(unsigned char *packet, uint8_t originator_mac[6]) {

    int offset = 0;
    uint8_t num_list = 0;
    uint32_t num_fwd_nbyte_order = 0, fwd_session_nbyte_order = 0, num_tx_nbyte_order = 0, tx_session_nbyte_order = 0;
    uint8_t mac_temp[6];

    memcpy(&num_list, packet, 1);
    packet = packet + 1;
    offset += 1;

#if PRINT_PACKET
    printf("[ %x ]", (uint8_t)*(packet - 1));
    fflush(stdout);
#endif
    int i;
    for (i = 0; i < num_list; i++) {
        memcpy(mac_temp, packet, 6);
        packet = packet + 6;
        offset += 6;
#if PRINT_PACKET
        printf("[ %x %x %x %x %x %x ]", (uint8_t) *(packet - 6), (uint8_t)*(packet - 5), (uint8_t)*(packet - 4), (uint8_t)*(packet - 3), (uint8_t)*(packet - 2), (uint8_t)*(packet - 1));
        fflush(stdout);
#endif
        memcpy(&num_tx_nbyte_order, packet, 4);
        packet = packet + 4;
        offset += 4;
#if PRINT_PACKET
        printf("[ %x %x %x %x ]", (uint8_t)*(packet - 4), (uint8_t)*(packet - 3), (uint8_t)*(packet - 2), (uint8_t)*(packet - 1));
        fflush(stdout);
#endif

        memcpy(&tx_session_nbyte_order, packet, sizeof (uint32_t));
        packet = packet + 4;
        offset += 4;
#if PRINT_PACKET
        printf("[ %x %x %x %x ]", (uint8_t)*(packet - 4), (uint8_t)*(packet - 3), (uint8_t)*(packet - 2), (uint8_t)*(packet - 1));
        fflush(stdout);
#endif

        memcpy(&num_fwd_nbyte_order, packet, sizeof (uint32_t));
        packet = packet + 4;
        offset += 4;
#if PRINT_PACKET
        printf("[ %x %x %x %x ]", (uint8_t)*(packet - 4), (uint8_t)*(packet - 3), (uint8_t)*(packet - 2), (uint8_t)*(packet - 1));
        fflush(stdout);
#endif

        memcpy(&fwd_session_nbyte_order, packet, sizeof (uint32_t));
        packet = packet + 4;
        offset += 4;
#if PRINT_PACKET
        printf("[ %x %x %x %x ]", (uint8_t)*(packet - 4), (uint8_t)*(packet - 3), (uint8_t)*(packet - 2), (uint8_t)*(packet - 1));
        fflush(stdout);
#endif
        update_gstats_with_tx_stats(originator_mac, mac_temp, ntohl(num_tx_nbyte_order), ntohl(tx_session_nbyte_order), ntohl(num_fwd_nbyte_order), ntohl(fwd_session_nbyte_order));

    }
    return offset;
}

int extract_host_rx_info(unsigned char *packet, unsigned char *originator_mac, int *len) {

    int offset = 0;
    uint8_t num_list = 0;
    uint32_t num_rx_nbyte_order = 0, rx_session_nbyte_order = 0, num_bcast_nbyte_order = 0;
    uint8_t mac_temp[6];

    memcpy(&num_list, packet, 1);
    packet = packet + 1;
    offset += 1;
    *len -= 1;
#if PRINT_PACKET
    printf("[ %x ]", (uint8_t)*(packet - 1));
    fflush(stdout);
#endif

    int i;
    for (i = 0; i < num_list; i++) {
        memcpy(mac_temp, packet, 6);
        packet = packet + 6;
        offset += 6;
#if PRINT_PACKET
        printf("[ %x %x %x %x %x %x ]", (uint8_t) *(packet - 6), (uint8_t)*(packet - 5), (uint8_t)*(packet - 4), (uint8_t)*(packet - 3), (uint8_t)*(packet - 2), (uint8_t)*(packet - 1));
        fflush(stdout);
#endif
        memcpy(&num_rx_nbyte_order, packet, 4);
        packet = packet + 4;
        offset += 4;
#if PRINT_PACKET
        printf("[ %x %x %x %x ]", (uint8_t)*(packet - 4), (uint8_t)*(packet - 3), (uint8_t)*(packet - 2), (uint8_t)*(packet - 1));
        fflush(stdout);
#endif

        memcpy(&rx_session_nbyte_order, packet, sizeof (uint32_t));
        packet = packet + 4;
        offset += 4;
#if PRINT_PACKET
        printf("[ %x %x %x %x ]", (uint8_t)*(packet - 4), (uint8_t)*(packet - 3), (uint8_t)*(packet - 2), (uint8_t)*(packet - 1));
        fflush(stdout);
#endif

        memcpy(&num_bcast_nbyte_order, packet, sizeof (uint32_t));
        packet = packet + 4;
        offset += 4;
#if PRINT_PACKET
        printf("[ %x %x %x %x ]", (uint8_t)*(packet - 4), (uint8_t)*(packet - 3), (uint8_t)*(packet - 2), (uint8_t)*(packet - 1));
        fflush(stdout);
#endif

        update_gstats_with_rx_stats(originator_mac, mac_temp, ntohl(num_rx_nbyte_order), ntohl(num_bcast_nbyte_order), ntohl(rx_session_nbyte_order));
    }

    return offset;

}

uint8_t my_gw_status() {
    char filename[100], command_str[100], line[80];
    FILE *fptr_gw_rd = NULL;

    memset(filename, '\0', 100);
    memset(command_str, '\0', 100);
#if IS_EMB_DEV
    sprintf(filename, "%s", IS_GW_FILE_NAME);
#else
    sprintf(filename, "%s%s", getenv("HOME"), IS_GW_FILE_NAME);
#endif
    sprintf(command_str, "cat %s", filename);
    fptr_gw_rd = popen(command_str, "r");
    if (fptr_gw_rd == NULL) {
        return 0;
    }

    memset(&line, 0, 80);
    if (fptr_gw_rd != NULL) {
        if (fgets(line, 80, fptr_gw_rd) != NULL) {
            pclose(fptr_gw_rd);
            return atoi(line);
        } else {
            pclose(fptr_gw_rd);
            return 0;
        }
    }
}

void process_gw_info(int am_i_gw, int is_a_gateway, uint32_t ip_long_nbyte_order, int num_dns_ip, char dns_ip1[16], char dns_ip2[16], char dns_ip3[16]) {

    FILE *fp_dns = NULL;
    char *ptr_def_gw_ip_str;
    char def_gw_ip_str[16], command_str[100], line[100];
    int i;

    memset(&def_gw_ip_str, '\0', 16);
    memset(command_str, '\0', 100);

    if (am_i_gw != 1 && is_a_gateway == 1) {
#if DEBUG
        printf("\nProcessing GW information..");
        fflush(stdout);
#endif
        struct in_addr temp_ip_in_addr;
        temp_ip_in_addr.s_addr = ntohl(ip_long_nbyte_order);
        ptr_def_gw_ip_str = (char *) inet_ntoa(temp_ip_in_addr);
        strcpy(def_gw_ip_str, ptr_def_gw_ip_str);
        memset(&command_str, '\0', 100);
        sprintf(command_str, "sudo route add default gw %s", def_gw_ip_str);
        system(command_str);

        if ((fp_dns = fopen("/etc/resolv.conf", "w")) == NULL) {
            printf("\nCould open /etc/resolv.conf to write. Terminating...");
            exit(1);
        }
        memset(line, '\0', 80);
        for (i = 0; i < num_dns_ip; i++) {
            if (i == 0) {
                fprintf(fp_dns, "nameserver %s\n", dns_ip1);
            }
            if (i == 1) {
                fprintf(fp_dns, "nameserver %s\n", dns_ip2);
            }
            if (i == 2) {
                fprintf(fp_dns, "nameserver %s\n", dns_ip3);
            }
        }
        fclose(fp_dns);
    }

}

uint8_t check_manifold_lkm_status() {

    FILE *fp_lkm = NULL;

    uint8_t ret_value = 255;

    if ((fp_lkm = popen("lsmod | grep manifold | wc -l", "r")) == NULL) {
        printf("\nCould open lsmod.. Terminating...");
        exit(1);
    } else {
        char line[10];

        memset(&line, '\0', 10);
        while (fgets(line, 10, fp_lkm)) {
            ret_value = (uint8_t) atoi(line);
        }
    }

    pclose(fp_lkm);
    return ret_value;
}
