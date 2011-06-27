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


extern int g_additional_len;
extern uint8_t g_mac[6];

void write_first_line_into_file(char *buf, char *file_name) {

    char command_str[100];

    memset(command_str, '\0', 100);
    sprintf(command_str, "echo %s > %s", buf, file_name);
    system(command_str);
}

int read_and_pack_rxstats(char *buf) {

    char line[80];
    char mac_temp[6];
    FILE *fptr;
    int offset = 0;
    uint8_t line_count = 0;
    /*Open file for reading */

    fptr = fopen(RX_FILE_NAME, "r");
    if (fptr == NULL) {
        printf("\n%s file could not be read\n", RX_FILE_NAME);
        exit(1);
    }

    while ((fgets(line, 80, fptr) != NULL)) {
        char *mac = strtok(line, " ");
        if (string_to_byte_order_etheraddr(mac, mac_temp) != 0) {
            printf("\n2: Error in string_etheraddr\n");
            exit(1);
        }
        if (memcmp(g_mac, mac_temp, 6) != 0) {
            line_count++;
        }
    }

    buf += pack(buf, (uint8_t *) (int) & line_count, 1);
    offset += 1;
#if PRINT_PACKET
    printf("[%x]", (uint8_t)*(buf - 1));
    fflush(stdout);
#endif
#if DEBUG
    printf("\nHere %d", line_count);
    fflush(stdout);
#endif

    if (fptr)
        fclose(fptr);

    memset(&line, '\0', sizeof (line));
    memset(&mac_temp, '\0', sizeof (mac_temp));

    fptr = fopen(RX_FILE_NAME, "r");
    if (fptr == NULL) {
        printf("\n%s file could not be read\n", RX_FILE_NAME);
        exit(1);
    }

    /*Read file line by line and Pack MAC, #rx_packets and #session_id*/
    while ((fgets(line, 80, fptr) != NULL)) {

#if DEBUG
        printf("%s\n", line);
#endif

        char *mac = strtok(line, " ");
        char *num_of_pkts_string = strtok(NULL, " ");
        char *session_id_string = strtok(NULL, " ");
        char *num_of_bcast_pkts_string = strtok(NULL, " ");

        uint32_t num_of_pkts = atoi(num_of_pkts_string);
        uint32_t num_of_bcast_pkts = atoi(num_of_bcast_pkts_string);
        uint32_t session_id = atoi(session_id_string);
        uint32_t num = htonl(num_of_pkts);
        uint32_t num_bcast = htonl(num_of_bcast_pkts);
        uint32_t session = htonl(session_id);

#if DEBUG
        printf("Reading and packing Rxstats--> \n\tmac : %s, \n\tnum_of_pkts : %d, \n\tsession : %d\n",
                mac, num_of_pkts, session_id);
#endif
        if (string_to_byte_order_etheraddr(mac, mac_temp) != 0) {
            printf("\n2: Error in string_etheraddr\n");
            exit(1);
        }

        if (memcmp(g_mac, mac_temp, 6) != 0) {

            buf += pack(buf, (uint8_t *) (int) & mac_temp, 6);
#if PRINT_PACKET
            printf("[%x %x %x %x %x %x]", (uint8_t)*(buf - 6), (uint8_t)*(buf - 5), (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
            fflush(stdout);
#endif
            buf += pack(buf, (uint8_t *) (int) & num, sizeof (num));
#if PRINT_PACKET
            printf("[%x %x %x %x]", (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
            fflush(stdout);
#endif
            buf += pack(buf, (uint8_t *) (int) & session, sizeof (session));
#if PRINT_PACKET
            printf("[%x %x %x %x]", (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
            fflush(stdout);
#endif
            buf += pack(buf, (uint8_t *) (int) & num_bcast, sizeof (num_bcast));
#if PRINT_PACKET
            printf("[%x %x %x %x]", (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
            fflush(stdout);
#endif
            offset = offset + 6 + 2 * sizeof (num) + sizeof (session);

        }

        memset(line, 0, 80);
    }
    if (fptr)
        fclose(fptr);

    return offset;

}

int read_and_pack_txstats(char *buf) {
    char line[80];
    FILE *fptr;
    int offset = 0;
    uint8_t line_count = 0;
    uint8_t mac_temp[6];


    fptr = fopen(TX_FILE_NAME, "r");
    if (fptr == NULL) {
        printf("\n %s could not be read", TX_FILE_NAME);
        exit(1);
    }

    memset(&line, '\0', sizeof (line));

    while ((fgets(line, 80, fptr) != NULL)) {
        char *mac = strtok(line, " ");
        if (string_to_byte_order_etheraddr(mac, mac_temp) != 0) {
            printf("\n2: Error in string_etheraddr\n");
            exit(1);
        }
        if (memcmp(g_mac, mac_temp, 6) != 0) {
            line_count++;
        }
    }

    buf += pack(buf, (uint8_t *) (int) & line_count, 1);
    offset += 1;
#if PRINT_PACKET
    printf("[%x]", (uint8_t)*(buf - 1));
    fflush(stdout);
#endif

    if (fptr)
        fclose(fptr);

    fptr = fopen(TX_FILE_NAME, "r");
    if (fptr == NULL) {
        printf("\n %s could not be read", TX_FILE_NAME);
        exit(1);
    }

    memset(&line, '\0', sizeof (line));

    while ((fgets(line, 80, fptr) != NULL)) {

        memset(&mac_temp, '\0', 6);
        char *mac = strtok(line, " ");

        if (string_to_byte_order_etheraddr(mac, mac_temp) != 0) {
            printf("\n2: Error in string_etheraddr\n");
            exit(1);
        }
        if (memcmp(g_mac, mac_temp, 6) != 0) {
            buf += pack(buf, (uint8_t *) (int) & mac_temp, 6);
            offset += 6;
#if PRINT_PACKET
            printf("[%x %x %x %x %x %x]", (uint8_t)*(buf - 6), (uint8_t)*(buf - 5), (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
            fflush(stdout);
#endif
        }

        char *num_of_pkts_string = strtok(NULL, " ");
        char *session_id_string = strtok(NULL, " ");

        uint32_t num_of_pkts = atoi(num_of_pkts_string);
        uint32_t session_id = atoi(session_id_string);
        uint32_t num_of_pkts_nbyte_order = htonl(num_of_pkts);
        uint32_t session_id_nbyte_order = htonl(session_id);

        if (memcmp(g_mac, mac_temp, 6) != 0) {
            buf += pack(buf, (uint8_t *) (int) & num_of_pkts_nbyte_order, 4);
#if PRINT_PACKET
            printf("[%x %x %x %x]", (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
            fflush(stdout);
#endif
            buf += pack(buf, (uint8_t *) (int) & session_id_nbyte_order, 4);
#if PRINT_PACKET
            printf("[%x %x %x %x]", (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
            fflush(stdout);
#endif
            offset += 8;
        }

#if DEBUG
        printf("\nReading and packing txstats for %s ->\n\tNumber of packets -> %d\n\tSession id -> %d", mac, num_of_pkts, session_id);
        fflush(stdout);
#endif

        num_of_pkts_string = strtok(NULL, " ");
        num_of_pkts_string = strtok(NULL, " ");
        num_of_pkts_string = strtok(NULL, " ");
        session_id_string = strtok(NULL, " ");
        num_of_pkts = atoi(num_of_pkts_string);
        session_id = atoi(session_id_string);
        num_of_pkts_nbyte_order = htonl(num_of_pkts);
        session_id_nbyte_order = htonl(session_id);

        if (memcmp(g_mac, mac_temp, 6) != 0) {

            buf += pack(buf, (uint8_t *) (int) & num_of_pkts_nbyte_order, 4);
#if PRINT_PACKET
            printf("[%x %x %x %x]", (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
            fflush(stdout);
#endif
            buf += pack(buf, (uint8_t *) (int) & session_id_nbyte_order, 4);
#if PRINT_PACKET
            printf("[%x %x %x %x]", (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
            fflush(stdout);
#endif
            offset += 8;

#if DEBUG
            printf("\n\n\tNumber of fwds -> %d\n\tFWD Session id -> %d\n", num_of_pkts, session_id);
            fflush(stdout);
#endif
            memset(line, 0, 80);
        }
    }

    if (fptr)
        fclose(fptr);

    return offset;
}

int read_and_pack_dns(char *buf) {

#if DEBUG
    printf("\nInside read_and_pack_dns ->");
    fflush(stdout);
#endif


    int offset = 0;
    uint8_t num_dns_ip = 0;
    uint32_t ip_long = 0, ip_long_nbyte_order = 0;
    char dns_ip1[16], dns_ip2[16], dns_ip3[16];

    memset(&dns_ip1, '\0', 16);
    memset(&dns_ip2, '\0', 16);
    memset(&dns_ip3, '\0', 16);

    num_dns_ip = read_from_first_three_lines_of_file_having_pattern(dns_ip1, dns_ip2, dns_ip3, "/etc/resolv.conf", "nameserver", 1);

#if DEBUG
    printf("\n\tNum of DNS ip -> %d %s %s %s", num_dns_ip, dns_ip1, dns_ip2, dns_ip3);
    fflush(stdout);
#endif

    buf += pack(buf, &num_dns_ip, 1);
#if PRINT_PACKET
    printf("[%x]", (uint8_t)*(buf - 1));
#endif
    offset += 1;
#if DEBUG
    printf("\n\tAfter packing num of dns", num_dns_ip);
    fflush(stdout);
#endif
    /*Pack the number of DNS IPs that were successfully extracted, 1 < num_dns_ip <= 3*/
    if (num_dns_ip == 1) {
        /*Pack only a single DNS IP address, which was successfully extracted*/

        ip_long = inet_addr(dns_ip1);
        ip_long_nbyte_order = htonl(ip_long);
        buf += pack(buf, (uint8_t *) & ip_long_nbyte_order, 4);
        offset += 4;
#if PRINT_PACKET
        printf("[%x %x %x %x]", (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
        fflush(stdout);
#endif

    } else if (num_dns_ip == 2) {
        /*Pack two DNS IP addresses*/

        ip_long = inet_addr(dns_ip1);
        ip_long_nbyte_order = htonl(ip_long);
        buf += pack(buf, (uint8_t *) & ip_long_nbyte_order, 4);
        offset += 4;
#if PRINT_PACKET
        printf("[%x %x %x %x]", (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
        fflush(stdout);
#endif

        ip_long = inet_addr(dns_ip2);
        ip_long_nbyte_order = htonl(ip_long);
        buf += pack(buf, (uint8_t *) & ip_long_nbyte_order, 4);
        offset += 4;
#if PRINT_PACKET
        printf("[%x %x %x %x]", (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
        fflush(stdout);
#endif

    } else if (num_dns_ip == 3) {
        /*Pack three DNS IP addresses*/

        ip_long = inet_addr(dns_ip1);
        ip_long_nbyte_order = htonl(ip_long);
        buf += pack(buf, (uint8_t *) & ip_long_nbyte_order, 4);
        offset += 4;
#if PRINT_PACKET
        printf("[%x %x %x %x]", (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
        fflush(stdout);
#endif

        ip_long = inet_addr(dns_ip2);
        ip_long_nbyte_order = htonl(ip_long);
        buf += pack(buf, (uint8_t *) & ip_long_nbyte_order, 4);
        offset += 4;
#if PRINT_PACKET
        printf("[%x %x %x %x]", (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
        fflush(stdout);
#endif

        ip_long = inet_addr(dns_ip3);
        ip_long_nbyte_order = htonl(ip_long);
        buf += pack(buf, (uint8_t *) & ip_long_nbyte_order, 4);
        offset += 4;
#if PRINT_PACKET
        printf("[%x %x %x %x]", (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
        fflush(stdout);
#endif        
    }

    return offset;
}

int read_from_first_three_lines_of_file_having_pattern(char *buf1, char *buf2, char *buf3, char *file_name, char *pattern, int token_num) {

    int i = 0;
    FILE *fptr;
    char line[80];

    memset(&line, '\0', sizeof (line));
    fptr = fopen(file_name, "r");
    if (fptr == NULL) {
        printf("\nCannot open %s. Terminating...", file_name);
        exit(1);
    }

    while (fgets(line, 80, fptr) != NULL) {
        char *nameserver = NULL, *buf = NULL;

        nameserver = strtok(line, " \n\t");
        buf = strtok(NULL, " \n\t");

        if (strncmp(nameserver, "nameserver", 10) == 0) {
            if (i == 0) {
                strncpy(buf1, buf, 16);
            } else if (i == 1) {
                strncpy(buf2, buf, 16);
            } else if (i == 2) {
                strncpy(buf3, buf, 16);
            }
            i++;
            if (i == MAX_NUM_DNS_TX) {
                break;
            }
        }
        memset(&line, '\0', 80);
    }
    if (fptr) {
        fclose(fptr);
    }

    return i;

}

void read_first_line_from_file(char *buf, char *file_name) {

    FILE *fptr;

    fptr = popen(file_name, "r");
    if (fptr == NULL) {
        printf("\nCould not open %s. Terminating...", file_name);
        exit(1);
    }

    if (fgets(buf, 80, fptr) == NULL) {
        printf("\nCould not read from %s. Terminating...", file_name);
        exit(1);
    }

    pclose(fptr);

}

int first_line_exists_in_file(char *file_name) {

    FILE *fptr;
    char buf[80];

    fptr = popen(file_name, "r");
    if (fptr == NULL) {
        printf("\nCould not open %s. Returning 0...", file_name);
        return 0;
    }

    if (fgets(buf, 80, fptr) == NULL) {
#if DEBUG
        printf("\n\tCould not read from %s. Returning 0...", file_name);
        fflush(stdout);
#endif
        pclose(fptr);
        return 0;
    }

    pclose(fptr);
    return 1;

}
