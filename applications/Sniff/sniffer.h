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

#ifndef _SNIFFER_H
#define	_SNIFFER_H

#ifndef IS_EMB_DEV
#define IS_EMB_DEV 0
#else
#endif

#define DEBUG 0
#define PRINT_PACKET 1 //both DEBUG and PRINT_PACKET should not be 1 at the same time
#define GW_SUPPORT 0
#define DST_ETHER_ADDR	"FF:FF:FF:FF:FF:FF"
#define BCAST_THRESHOLD_DISTANCE 30
#define BUFFER_LEN 512
#define MANIFOLD_UPDATE_TYPE 0x3434
#define HB_INTERVAL 5
#define MAX_NUM_OF_HOSTS 40
#define MAX_NUM_OF_STAT_ENTRIES 40
#define MAX_NODE_NAME_SIZE 10
#define NUM_OF_REAL_HOSTS 6
#define ACTIVE_TIMEOUT 20
#define SOCKET_SELECT_TIMEOUT 0
#define MAX_DST 255
#define TX_FILE_NAME "/proc/txstats"
#define WDL_FILE_NAME "/proc/wdl"
#if IS_EMB_DEV

#define IS_GW_FILE_NAME "/etc/config/mymanet/is_gw.conf"
#define HOSTS_FILE_PATH "/etc/config/mymanet/hosts"
#define HOSTLIST_PATH "/etc/config/mymanet/hosts.txt"
#define STATS_FILE_PATH "/etc/config/mymanet/statlist"
#define STATS_REFINED_FILE_PATH "/etc/config/mymanet/statlist_refined"
#define TRACE_FILE_PATH "/etc/config/mymanet/packet_trace"
#define GW_LOG_FILE_NAME "/etc/config/mymanet/gw_log.conf"
#define GNST_PATH "/etc/config/mymanet/gnst.txt"

#else

#define IS_GW_FILE_NAME "/.LifeNetData/config/is_gw.conf"
#define HOSTS_FILE_PATH "/.LifeNetData/config/hosts"
#define HOSTLIST_PATH "/.LifeNetData/config/hosts.txt"
#define STATS_FILE_PATH "/.LifeNetData/config/statlist"
#define STATS_REFINED_FILE_PATH "/.LifeNetData/config/statlist_refined"
#define TRACE_FILE_PATH "/.LifeNetData/config/packet_trace"
#define GW_LOG_FILE_NAME "/.LifeNetData/config/gw_log.conf"
#define GNST_PATH "/.LifeNetData/config/gnst.txt"

#endif
#define ALLOWED_GW_VD_DEVIATION 20 /*in percent*/

/* This structure is not used currently
struct gateway_list {
    uint32_t ip_long;
    uint32_t ip_dns1;
    uint32_t ip_dns2;
    uint32_t ip_dns3;
    uint8_t distance;

} __attribute__((packed));

struct gateway_list gw_list[MAX_NUM_OF_HOSTS];
int g_gw_count;
 */

struct global_stats { /*Structure to hold the link statistics*/
    uint32_t num_tx;
    uint32_t tx_session;
    uint32_t num_rx;
    uint32_t rx_session;
    uint32_t num_bcast_rx;
    uint32_t num_fwd;
    uint32_t fwd_session;
};
struct global_stats g_stats[MAX_NUM_OF_HOSTS][MAX_NUM_OF_HOSTS];

struct global_bcast_stats {
    uint32_t num_bcast;
    uint32_t bcast_session;
    uint32_t num_fwd;
    uint32_t fwd_session;
};
struct global_bcast_stats g_bcast_stats[MAX_NUM_OF_HOSTS];

struct host {
    unsigned char hostmac[6];
    uint32_t ip_long;
    char node_name[10];
    time_t timestamp;
} hostlist[MAX_NUM_OF_HOSTS];

uint8_t g_broadcast_mac[6];
uint8_t g_distance_table[MAX_NUM_OF_HOSTS][3];
uint8_t g_mac[6];

int (*g_pluggable_func_ptr)(int receiver, unsigned char recvmac[6], int s, int r);

time_t current_time;
int g_my_id;
int g_tolerance;
int g_is_emb_dev; /*1: Is an embedded device. 0: Is NOT an embedded device.*/
int g_host_count;
uint8_t bcast_fwd_threshold;
uint8_t bcast_fwd_deno;

#endif	/* _SNIFFER_H */
