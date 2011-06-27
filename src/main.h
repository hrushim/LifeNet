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
Developers: Ashwin Paranjpe, Santosh Vempala
Georgia Institute of Technology, Atlanta, USA
*/

#ifndef MAIN_H
#define MAIN_H

#ifndef MYMANET_STORE_PATH
#define MYMANET_STORE_PATH 0
#endif

#define ETH_P_NEWFORMAT 0x3333                  /*Manifold Packet Type*/
#if MYMANET_STORE_PATH
#define MANIFOLD_HEADER_SIZE 44			/*Size of Manifold Header contents*/
#else
#define MANIFOLD_HEADER_SIZE 26			/*Size of Manifold Header contents*/
#endif
#define MAX_HOPS 4				/*This will be entered in every new Manifold packet*/
#define ETHERNET_HEADER_SIZE 14

#ifndef DEBUG_ON				/*Parameter to Makefile*/
#define DEBUG_ON 0				/*Default value*/
#endif

#define DEBUG 0

#define RTX_CNT 7
#define MAX_USABLE_VD 255
#define OUT_OF_ORDER_WINDOW 100000		/* 100 msec */

#define NEW 1
#define OLD 2
#define DUPLICATE 0
#define OUT_OF_ORDER 3

int mymanet_alpha = 100;
int mymanet_beta = 80;
char *device_name=""; 	              		/*Name of the interface to be used [NOT Optional]*/
struct net_device *d=NULL;                      /*Main DEVICE structure for this interface*/


/*Manifold Routing is Controlled using /proc/wdl : start and stop commands*/
int global_manifold_disable=0;           /* 1 : Manifold Routing OFF     0: Manifold Routing ON*/

int g_manifold_session_transmitted_count=0;      /*Count of number of Manifold packets transmitted through this module in this session*/
int g_manifold_last_session_transmitted_count=0;      /*Count of number of Manifold packets transmitted through this module in the previous session*/

int g_per_session_dup_cnt = 0;
int g_last_per_session_dup_cnt = 0;

uint8_t g_broadcast_mac[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

/*Prototype Declarations*/
int wdl_handle_recieve(struct sk_buff *, struct net_device *, struct packet_type *, struct net_device *);
int wdl_hard_start_xmit(struct sk_buff *, struct net_device *);
int (*device_hard_start_xmit_backup) (struct sk_buff *skb, struct net_device *netdev) = NULL;


/*Define a new packet type and a receive function to handle this packet type. 
 *  * Currently, for simplicity we define this packet type as being ETH_P_IP
 *   * */
struct packet_type manifold_ptype = {
  .type = __constant_htons(ETH_P_NEWFORMAT),
  .func = wdl_handle_recieve,
};


#endif	/*End of main.h*/
