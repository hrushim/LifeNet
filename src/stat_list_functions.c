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

#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/sched.h>
#include<linux/module.h>
#include<linux/moduleparam.h>
#include<linux/netdevice.h>
#include<linux/etherdevice.h>

#include<linux/string.h>
#include<linux/proc_fs.h>
#include<linux/list.h>

#include "platform_dep_flags.h"
#include "distance_list_functions.h"
#include "string_functions.h"
#include "stat_list_functions.h"

#define MAX_NUM_NODES 50

int g_stat_list_count;

uint8_t bcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/*
 * Adds a new mac entry in the stat list or updates the RX/TX packet count depending on the tx_rx_flag
 * tx_rx_flag = 0 for RX
 * tx_rx_flag = 1 for TX
 * */

int collect_garbage_entries(int dummy_to_avoid_compile_warning)
{
	struct stat_list *tmp = NULL;
	struct list_head *pos = NULL, *q = NULL;
	struct timeval current_time;

	if(list_empty(&stat_head.list)){
		return 0;
	}

	do_gettimeofday(&current_time);

	list_for_each_safe(pos, q, &stat_head.list){
		tmp = list_entry(pos, struct stat_list, list);

		if((current_time.tv_sec - tmp->last_recv_time.tv_sec) > STATLIST_EXPIRY_INTERVAL)
		{
			if(compare_mac_address(bcast_mac, tmp->mac) == 0)
			{
				list_del(pos);
				remove_distance_entry(tmp->mac);
				kfree(tmp);
				g_stat_list_count--;
			}
		}
        }
	return 0;
}

int add_or_update_stat_entry(uint8_t mac[6], uint8_t tx_rx_flag, uint32_t session_id, uint8_t dest_mac[6])
{
	struct stat_list *new = NULL;
	struct stat_list *tmp = NULL;

	if (search_and_update_stat(mac, tx_rx_flag, session_id, dest_mac) == 1)
		return 0;

	if (g_stat_list_count < MAX_NUM_NODES) {
		/*Entry does not exist. Add it and increment Stat list count.*/
		new = (struct stat_list *) kmalloc(sizeof(struct stat_list), GFP_KERNEL);

		if(new == NULL){
			printk(KERN_ALERT"\nCould not allocate memory to a new stat entry\n");
			return -1;
		}

		memset(new, 0, sizeof(struct stat_list));
		list_add(&new->list, &stat_head.list);
		tmp = new;	/* Update stats of the new node*/

	} else {
		return 0;

	}

	tmp->id = g_stat_list_count++;
	memcpy(tmp->mac, mac, 6);

	if(tx_rx_flag == 0){
		tmp->session_id = session_id;

		if(compare_mac_address(dest_mac, bcast_mac) == 1)
			tmp->num_rx_bcast = 1;
		else
			tmp->num_rx = 1;

		do_gettimeofday(&tmp->last_recv_time);

	} else if (tx_rx_flag == 1) {
		tmp->tx_session_id = session_id;
		tmp->num_tx = 1;

	} else if (tx_rx_flag == 2) {
		tmp->fwd_session_id = session_id;
		tmp->num_fwd = 1;

	}

	return 0;
}

int free_entire_stat_list(void){

	struct stat_list *tmp=NULL;
	struct list_head *pos = NULL, *q = NULL;

	if(list_empty(&stat_head.list)){
		printk(KERN_ALERT"\nStat list is already empty\n");
		return 0;
	}

	list_for_each_safe(pos, q, &stat_head.list){
		tmp = list_entry(pos, struct stat_list, list);
		list_del(pos);
		kfree(tmp);
		g_stat_list_count--;
	}

	printk(KERN_ALERT"\nFree : Stat list is NOW empty\n");
	return 0;
}

/*
 * If entry exists, entry is updated based on value of tx_rx_flag and return 1
 * tx_rx_flag = 0 for RX
 * tx_rx_flag = 1 for TX
 * else
 * return 0
 * */
uint8_t search_and_update_stat(uint8_t mac[6], uint8_t tx_rx_flag, uint32_t session_id, uint8_t dest_mac[6])
{

	struct stat_list *tmp = NULL;
	struct list_head *pos = NULL, *q = NULL;

	if(list_empty(&stat_head.list)){
		return 0;
	}

	list_for_each_safe(pos, q, &stat_head.list){
		tmp = list_entry(pos, struct stat_list, list);

		if((compare_mac_address(tmp->mac, mac)) == 1){
			if(tx_rx_flag == 0){
				/*Check whether session has changed. If yes; reset rx counters*/
				if (tmp->session_id > 4 && session_id < 2) {
					tmp->last_session_id = 0;
					tmp->last_num_rx = 0;
					tmp->last_num_rx_bcast = 0;
					tmp->session_id = session_id;
					if(compare_mac_address(dest_mac, bcast_mac) == 1)
						tmp->num_rx_bcast = 1;
					else
						tmp->num_rx = 1;
				} else if (session_id > tmp->session_id) {
					/*Update the details of the previous session statistics.
 					*This will be shown in proc entries.*/
					tmp->last_session_id = tmp->session_id;
					tmp->last_num_rx = tmp->num_rx;
					tmp->last_num_rx_bcast = tmp->num_rx_bcast;
					/*Update the new sessions' statistics.*/
					tmp->session_id = session_id;
					if(compare_mac_address(dest_mac, bcast_mac) == 1)
					{
						tmp->num_rx_bcast = 1;
						tmp->num_rx = 0;
					}
					else
					{
						tmp->num_rx_bcast = 0;
						tmp->num_rx = 1;
					}

				} else {
					if(compare_mac_address(dest_mac, bcast_mac) == 1)
						tmp->num_rx_bcast++;
					else
						tmp->num_rx++;
				}
				do_gettimeofday(&tmp->last_recv_time);
			}
			else if (tx_rx_flag == 1) {
				if(tmp->tx_session_id > 4 && session_id < 2) {
					tmp->last_tx_session_id = 0;
					tmp->last_num_tx = 0;
					tmp->tx_session_id = session_id;
					tmp->num_tx = 1;
				} else if(session_id > tmp->tx_session_id){
					tmp->last_tx_session_id = tmp->tx_session_id;
					tmp->last_num_tx = tmp->num_tx;
					/*Update the new sessions' statistics.*/
					tmp->tx_session_id = session_id;
					tmp->num_tx = 1;
				} else {
				  	tmp->num_tx++;
				}
			}
			else if (tx_rx_flag == 2) {
				if(tmp->fwd_session_id > 4 && session_id < 2) {
					tmp->last_fwd_session_id = 0;
					tmp->last_num_fwd = 0;
					tmp->fwd_session_id = session_id;
					tmp->num_fwd = 1;
				} else if(session_id > tmp->fwd_session_id){
					tmp->last_fwd_session_id = tmp->fwd_session_id;
					tmp->last_num_fwd = tmp->num_fwd;
					/*Update the new sessions' statistics.*/
					tmp->fwd_session_id = session_id;
					tmp->num_fwd = 1;
				} else {
				  	tmp->num_fwd++;
				}
			}
                        return 1;
                }
        }
        return 0;
}

