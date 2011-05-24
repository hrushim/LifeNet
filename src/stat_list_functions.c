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
#include<linux/time.h>
#include<linux/string.h>
#include<linux/proc_fs.h>

#include "stat_list_functions.h"
#include "string_functions.h"

#define DEBUG 0

int g_stat_list_count;
struct stat_list *stat_head = NULL;
uint8_t ignore_mac1[6] = {0x1, 0x0, 0x5e, 0x0, 0x0, 0x16};
uint8_t ignore_mac2[6] = {0x1, 0x0, 0x5e, 0x0, 0x0, 0xfb};
uint8_t ignore_mac3[6] = {0x33, 0x33, 0x0, 0x0, 0x0, 0x16};
uint8_t ignore_mac4[6] = {0x33, 0x33, 0x0, 0x0, 0x0, 0x2};

uint8_t bcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/* 
 * Adds a new mac entry in the stat list or updates the RX/TX packet count depending on the tx_rx_flag
 * tx_rx_flag = 0 for RX
 * tx_rx_flag = 1 for TX
 * */

int check_ignore_mac(uint8_t mac[6])
{
	if(compare_mac_address(ignore_mac1, mac) == 1) return 1;
	else if(compare_mac_address(ignore_mac2, mac) == 1) return 1;
	else if(compare_mac_address(ignore_mac3, mac) == 1) return 1;
	else if(compare_mac_address(ignore_mac4, mac) == 1) return 1;
	else return 0;
}

void collect_garbage_entries()
{
	struct timeval current_time;

	struct stat_list *tmp=NULL;
	struct stat_list *tmp_next=NULL;
	struct stat_list *tmp_prev=NULL;

	do_gettimeofday(&current_time);

        if(stat_head == NULL){
                return;
        }

        tmp = stat_head;

        while(tmp != NULL){

		tmp_next = tmp->next;
		tmp_prev = tmp->prev;
		if((current_time.tv_sec - tmp->last_recv_time.tv_sec) > STATLIST_EXPIRY_INTERVAL)
		{
			if(tmp_prev==NULL)
			{
				 // If the current element if the first element of the list
				 // First element is always the broadcast MAC address so 
				 // don't clean anything if this is the case 
				 tmp = tmp->next;
			}
			else if(tmp_next==NULL)
			{
				// If the current element is the last element of the list
				
				if(compare_mac_address(bcast_mac, tmp->mac)==0)
				{

					//rearrange pointers such that tmp is excluded
					(tmp->prev)->next = NULL;

					
					//free tmp
					kfree(tmp);
					
					//re-point tmp
					tmp = tmp_next;

				}
				else
					tmp = tmp->next;
			}
			else
			{
				// If the current element is a middle element in the list i.e. not first and last
				if(compare_mac_address(bcast_mac, tmp->mac)!=1)
				{

					//rearrange pointers such that tmp is excluded
					(tmp->prev)->next = tmp->next;
					(tmp->next)->prev = tmp->prev;
					tmp_next = tmp->next;
					
					//free tmp
					kfree(tmp);
					
					//re-point tmp
					tmp = tmp->next;

				}
				else
					tmp = tmp->next;
			}
		}
		else
		{
			tmp = tmp->next;
		}
        }
}

int add_or_update_stat_entry(uint8_t mac[6], uint8_t tx_rx_flag, uint32_t session_id, uint8_t dest_mac[6])
{
	struct stat_list *new = NULL;
	struct stat_list *tmp = NULL;
	
	if (check_ignore_mac(mac) == 1) return 0;
	
	if (check_ignore_mac(dest_mac) == 1) return 0;

	if(stat_head == NULL){
	  
		stat_head = (struct stat_list *) kmalloc(sizeof(struct stat_list), GFP_KERNEL);
		memset(stat_head, 0, sizeof(struct stat_list));

                if(stat_head == NULL){
                        printk(KERN_ALERT"\nCould not allocate memory to stat_head\n");
                        return -1;
                }

		stat_head->id = g_stat_list_count;
		memcpy(stat_head->mac, mac, 6);
		if(tx_rx_flag == 0){
			stat_head->session_id = session_id;
			
			if(compare_mac_address(dest_mac, bcast_mac) == 1)
			{
			      stat_head->num_rx_bcast = 1;
			}
			else
			{
			      stat_head->num_rx = 1;
			}
			
			stat_head->num_tx = 0;
			stat_head->num_fwd = 0;
			stat_head->tx_session_id = 0;
			stat_head->fwd_session_id = 0;			
			
			do_gettimeofday(&stat_head->last_recv_time);
		}
		else if(tx_rx_flag == 1){
			stat_head->tx_session_id = session_id;
			stat_head->num_tx = 1;	
			
			stat_head->num_rx = 0;
			stat_head->num_fwd = 0;
			stat_head->session_id = 0;
			stat_head->fwd_session_id = 0;			
			
		}
		else if(tx_rx_flag == 2){
			stat_head->fwd_session_id = session_id;
			stat_head->num_fwd = 1;	
			
			stat_head->num_tx = 0;
			stat_head->num_rx = 0;
			stat_head->session_id = 0;
			stat_head->tx_session_id = 0;						
		}
		
		stat_head->last_num_rx = 0;
		stat_head->last_num_rx_bcast = 0;
		stat_head->last_session_id = 0;		
		stat_head->last_num_tx = 0;
		stat_head->last_tx_session_id = 0;		
		stat_head->last_num_fwd = 0;
		stat_head->last_fwd_session_id = 0;
		
		stat_head->next = NULL;
		stat_head->prev = NULL;

		g_stat_list_count++;

	}
	else{
		if(search_and_update_stat(mac, tx_rx_flag, session_id, dest_mac) == 1)
			return 0;
		
		if(g_stat_list_count <50){
			
			/*Entry does not exist. Add it and increment Stat list count.*/
			new = (struct stat_list *) kmalloc(sizeof(struct stat_list), GFP_KERNEL);

			memset(new, 0, sizeof(struct stat_list));

			if(new == NULL){
				printk(KERN_ALERT"\nCould not allocate memory to a new stat entry\n");
				return -1;
			}

			new->id = g_stat_list_count;
			memcpy(new->mac, mac, 6);
			if(tx_rx_flag == 0){
			  
				new->session_id = session_id;
				
				if(compare_mac_address(dest_mac, bcast_mac) == 1)
				{
					new->num_rx_bcast = 1;
				}
				else
				{
					new->num_rx = 1;
				}
				
				new->num_tx = 0;
				new->num_fwd = 0;
				new->tx_session_id = 0;
				new->fwd_session_id = 0;	
				do_gettimeofday(&new->last_recv_time);
			}
			else if(tx_rx_flag == 1){
				new->tx_session_id = session_id;
				new->num_tx = 1;	
				
				new->num_rx = 0;
				new->num_fwd = 0;
				new->session_id = 0;
				new->fwd_session_id = 0;		
			}
			else if(tx_rx_flag == 2){
				new->fwd_session_id = session_id;
				new->num_fwd = 1;	
				
				new->num_tx = 0;
				new->num_rx = 0;
				new->session_id = 0;
				new->tx_session_id = 0;						
			}			
			
			new->last_num_rx = 0;
			new->last_num_rx_bcast = 0;
			new->last_session_id = 0;		
			new->last_num_tx = 0;
			new->last_tx_session_id = 0;		
			new->last_num_fwd = 0;
			new->last_fwd_session_id = 0;
			
			tmp = (struct stat_list *)stat_head;
			while(tmp != NULL && tmp->next != NULL){
				tmp = tmp->next;
			}
			tmp->next = new;
			new->prev = tmp;
			new->next = NULL;

			g_stat_list_count++;
		}

	}
	return 0;
}

int free_entire_stat_list(void){

        struct stat_list *tmp=NULL, *tmp2=NULL;

        if(stat_head == NULL){
                printk(KERN_ALERT"\nStat list is already empty\n");
                return 0;
        }
        tmp = stat_head;
        while(tmp != NULL){
                tmp2 = tmp->next;
                kfree(tmp);
                g_stat_list_count--;
                tmp = tmp2;
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

        struct stat_list *tmp=NULL;

        if(stat_head == NULL){
                return 0;
        }

        tmp = stat_head;

        while(tmp != NULL){

                if((compare_mac_address(tmp->mac, mac)) == 1){
			if(tx_rx_flag == 0){

				/*Check whether session has changed. If yes; reset rx counters*/
				if(tmp->session_id > 4 && session_id < 2) {
					tmp->last_session_id = 0;
					tmp->last_num_rx = 0;
					tmp->last_num_rx_bcast = 0;
					tmp->session_id = session_id;
					if(compare_mac_address(dest_mac, bcast_mac) == 1)
					{
						tmp->num_rx_bcast = 1;
					}
					else
					{
						tmp->num_rx = 1;
					}
				} else if(session_id > tmp->session_id){
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

				}
				else{
					if(compare_mac_address(dest_mac, bcast_mac) == 1)
					{
						tmp->num_rx_bcast++;
					}
					else
					{
						tmp->num_rx++;
					}
				}
				do_gettimeofday(&tmp->last_recv_time);
			}
			else if(tx_rx_flag == 1){
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
			else if(tx_rx_flag == 2){
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
                tmp = tmp->next;
        }
        return 0;
}

