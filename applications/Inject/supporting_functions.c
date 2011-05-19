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
extern int g_is_gw_flag;
extern uint8_t curr_session_id;

int pack(unsigned char *ptr, uint8_t *src_mac, int num_bytes) {

    memcpy(ptr, src_mac, num_bytes);
    return (num_bytes);
}

void create_ethernet_header(struct ethhdr *ptr, uint8_t *src_mac, uint8_t *dst_mac, int protocol) {

    char dst_mac_temp[6], mac_temp[6];

    memset(dst_mac_temp, '\0', 6);
    memset(mac_temp, '\0', 6);

    if (ptr == NULL) {
        printf("\nCould not allocate memory to create ethernet header\n");
        exit(1);
    }
    memset(ptr, 0, sizeof (struct ethhdr) + (sizeof (char) * BUFFER_LEN));
    memcpy(ptr->h_source, (char *) src_mac, 6);
    if (string_to_byte_order_etheraddr(dst_mac, dst_mac_temp) != 0) {
        printf("\nError in converting ethernet address string to byte order\n");
        exit(1);
    }
    memcpy(ptr->h_dest, (char *) dst_mac_temp, 6);
    ptr->h_proto = htons(protocol);

#if PRINT_PACKET
    uint8_t *tmp = (uint8_t *) ptr;
    printf("\n\n\n[%x %x %x %x %x %x]", *tmp, *(tmp + 1), *(tmp + 2), *(tmp + 3), *(tmp + 4), *(tmp + 5));
    tmp += 6;
    printf("[%x %x %x %x %x %x]", *tmp, *(tmp + 1), *(tmp + 2), *(tmp + 3), *(tmp + 4), *(tmp + 5));
    tmp += 6;
    printf("[%x %x]", *tmp, *(tmp + 1));
    fflush(stdout);
#endif
}

void get_wan_if_name(char * wan_if) {

    char line[80], command_str[100];

    memset(&line, 0, sizeof (line));
#if IS_EMB_DEV
    sprintf(command_str, "cat %s", WAN_IF_FILE_NAME);
#else
    memset(command_str, '\0', sizeof (command_str));
    sprintf(command_str, "cat %s%s", getenv("HOME"), WAN_IF_FILE_NAME);
#endif
    read_first_line_from_file(line, command_str);
    strncpy(wan_if, line, 10);
    wan_if[strlen(wan_if) - 1] = '\0';
#if DEBUG
    printf("\nWAN iface name -> %s", wan_if);
    fflush(stdout);
#endif


}

int set_gw_status(char *wan_if) {
    char line[80], command_str[100];

    memset(&line, 0, sizeof (line));
    memset(command_str, '\0', 100);
    sprintf(command_str, "ping -I %s -c 1 www.mymanet.org 2> /tmp/inject_log | grep \"ttl=\"", wan_if);
#if DEBUG
    printf("\n\tSet GW status: %d", first_line_exists_in_file(command_str));
    fflush(stdout);
#endif
    return first_line_exists_in_file(command_str);
}

void write_gw_status_into_file(int is_gw) {
    char filename[100], command_str[100];
    memset(filename, '\0', 100);
    memset(command_str, '\0', 100);
#if IS_EMB_DEV
    sprintf(filename, "%s", IS_GW_FILE_NAME);
#else
    sprintf(filename, "%s%s", getenv("HOME"), IS_GW_FILE_NAME);
#endif
    sprintf(command_str, "%d", is_gw);
#if DEBUG
    printf("\nWriting GW status into file -> \n\tFilename: %s\n\tData: %s", filename, command_str);
    fflush(stdout);
#endif
    write_first_line_into_file(command_str, filename);
}

int handle_dns_and_iptables(unsigned char *buf, int is_gw) {

    char filename[100];
    int offset = 0;
    if (is_gw == 1) {

        /*If GW present pack DNS addresses into packet*/
        offset += read_and_pack_dns(buf);
        buf += offset;

        /*This system command configures the iptables as this node is now the gateway*/
        if (g_is_gw_flag == 0) {
            memset(filename, '\0', 100);
#if IS_EMB_DEV
            sprintf(filename, "%s", FIREWALL_FWD_ALL_FILE_NAME);
#else
            sprintf(filename, "bash %s%s", getenv("HOME"), FIREWALL_FWD_ALL_FILE_NAME);
#endif
            system(filename);
            g_is_gw_flag = 1;
        }

        return offset;

    } else {
        /*Else flush off all the forwarding rules*/
        memset(filename, '\0', 100);
#if IS_EMB_GW
        sprintf(filename, "%s", FIREWALL_FLUSH_ALL_FILE_NAME);
#else
        sprintf(filename, "bash %s%s", getenv("HOME"), FIREWALL_FLUSH_ALL_FILE_NAME);
#endif
        system(filename);
        g_is_gw_flag = 0;

        return offset;
    }
}

int pack_curr_node_info(unsigned char *buf, char ip_address[15], uint8_t *src_mac, char node_name[10]) {

    uint32_t ip_long = 0, ip_long_nbyte_order = 0, offset = 0;

    ip_long = inet_addr(ip_address);
    ip_long_nbyte_order = htonl(ip_long);

    buf += pack(buf, src_mac, 6);
    offset += 6;
#if DEBUG
    printf("\nCurrent node info ->");
    printf("\n\tSource MAC: %x %x %x %x %x %x", *(buf - 6), *(buf - 5), *(buf - 4), *(buf - 3), *(buf - 2), *(buf - 1));
#endif
#if PRINT_PACKET
    printf("[%x %x %x %x %x %x]", (uint8_t)*(buf - 6), (uint8_t)*(buf - 5), (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
#endif

    buf += pack(buf, (uint8_t *) & ip_long_nbyte_order, 4);
    offset += 4;
#if DEBUG
    struct in_addr temp_ip_in_addr;
    temp_ip_in_addr.s_addr = ntohl(ip_long_nbyte_order);
    printf("\n\tIP Address -> %s (%x %x %x %x)", (char *) inet_ntoa(temp_ip_in_addr), *(buf - 4), *(buf - 3), *(buf - 2), *(buf - 1));
    fflush(stdout);
#endif
#if PRINT_PACKET
    printf("[%x %x %x %x]", (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
#endif

    buf += pack(buf, (uint8_t *) node_name, 10);
    offset += 10;
#if DEBUG
    printf("\n\tNode name -> %s (%x %x %x %x %x %x %x %x %x %x)", node_name, *(buf - 10), *(buf - 9), *(buf - 8), *(buf - 7), *(buf - 6), *(buf - 5), *(buf - 4), *(buf - 3), *(buf - 2), *(buf - 1));
    fflush(stdout);
#endif
#if PRINT_PACKET
    printf("[%x %x %x %x %x %x %x %x %x %x]", (uint8_t)*(buf - 10), (uint8_t)*(buf - 9), (uint8_t)*(buf - 8), (uint8_t)*(buf - 7), (uint8_t)*(buf - 6), (uint8_t)*(buf - 5), (uint8_t)*(buf - 4), (uint8_t)*(buf - 3), (uint8_t)*(buf - 2), (uint8_t)*(buf - 1));
    fflush(stdout);
#endif


#if DEBUG
    printf("\nReturning %d", offset);
#endif
    return offset;
}

int string_to_byte_order_etheraddr(const u_char *asc, char *addr) {
    int cnt;

    for (cnt = 0; cnt < 6; ++cnt) {
        unsigned int number;
        char ch;

        ch = tolower(*asc++);
        if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
            return 1;
        number = isdigit(ch) ? (ch - '0') : (ch - 'a' + 10);

        ch = tolower(*asc);
        if ((cnt < 5 && ch != ':') || (cnt == 5 && ch != '\0' && !isspace(ch))) {
            ++asc;
            if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
                return 1;
            number <<= 4;
            number += isdigit(ch) ? (ch - '0') : (ch - 'a' + 10);
            ch = *asc;
            if (cnt < 5 && ch != ':')
                return 1;
        }

        /* Store result.  */
        addr[cnt] = (unsigned char) number;

        /* Skip ':'.  */
        ++asc;
    }

    return 0;
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
        while(fgets(line, 10, fp_lkm)) {
            ret_value = (uint8_t)atoi(line);
        }
    }

    pclose(fp_lkm);
    return ret_value;
}