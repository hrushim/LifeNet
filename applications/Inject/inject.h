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
Developers: Ashwin Paranjpe, Santosh Vempala, Hrushikesh Mehendale
Georgia Institute of Technology, Atlanta, USA
 */



#ifndef _INJECT_H
#define	_INJECT_H

#ifndef IS_EMB_DEV
#define IS_EMB_DEV 0
#else
#endif

#define DEBUG 0
#define PRINT_PACKET 0  //both DEBUG and PRINT_PACKET should not be 1 at the same time
#define GW_SUPPORT 0
#define DST_ETHER_ADDR	"FF:FF:FF:FF:FF:FF"
#define BUFFER_LEN 1024
#define MANIFOLD_UPDATE_TYPE 0x3434
#define HB_INTERVAL 5
#define MAX_NUM_OF_HOSTS 40
#define MAX_NUM_OF_STAT_ENTRIES 40
#define MAX_NODE_NAME_SIZE 10
#define NUM_OF_REAL_HOSTS 6
#define ACTIVE_TIMEOUT 20
#define SOCKET_SELECT_TIMEOUT 0
#define MAX_DST 255
#define TX_LOG_FILE_NAME "/.LifeNetData/config/tx.log"
#define GW_CHECK_URL "www.google.com"
#define MAX_NUM_DNS_TX 3
#define TX_FILE_NAME "/proc/txstats"
#define RX_FILE_NAME "/proc/rxstats"

#if IS_EMB_DEV

	#define IS_GW_FILE_NAME "/etc/config/mymanet/is_gw.conf"
        #define WAN_IF_FILE_NAME "/etc/config/mymanet/eth_iface.conf"
        #define FIREWALL_FWD_ALL_FILE_NAME "/home/scripts/fwd_all.sh"
        #define FIREWALL_FLUSH_ALL_FILE_NAME "/home/scripts/flush_all.sh"
        #define HOSTS_FILE_PATH "/etc/config/mymanet/hosts"
        #define STATS_FILE_PATH "/etc/config/mymanet/statlist"
        #define STATS_REFINED_FILE_PATH "/etc/config/mymanet/statlist_refined"
        #define GW_LOG_FILE_NAME "/etc/config/mymanet/gw_log.conf"

#else
	#define IS_GW_FILE_NAME "/.LifeNetData/config/is_gw.conf"
	#define WAN_IF_FILE_NAME "/.LifeNetData/config/eth_iface.conf"
	#define FIREWALL_FWD_ALL_FILE_NAME "/.LifeNetData/firewall/fwd_all.sh"
	#define FIREWALL_FLUSH_ALL_FILE_NAME "/.LifeNetData/firewall/flush_all.sh"
	#define HOSTS_FILE_PATH "/.LifeNetData/config/hosts"
	#define STATS_FILE_PATH "/.LifeNetData/config/statlist"
	#define STATS_REFINED_FILE_PATH "/.LifeNetData/config/statlist_refined"
	#define GW_LOG_FILE_NAME "/.LifeNetData/config/gw_log.conf"
#endif
#define ALLOWED_GW_VD_DEVIATION 20 /*in percent*/

#endif	/* _INJECT_H */
