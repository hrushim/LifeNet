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

#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/sched.h>
#include<linux/module.h>
#include<linux/moduleparam.h>
#include<linux/netdevice.h>
#include<linux/etherdevice.h>

#include<linux/string.h>
#include<linux/proc_fs.h>

#include "timestamp_list_functions.h"
#include "string_functions.h"

#define DEBUG 0

int g_timestamp_list_count;
struct timestamp_list *timestamp_head=NULL;

int add_or_update_timestamp_entry(uint8_t mac[6], uint32_t timestamp, uint32_t timestamp_frac){
	struct timestamp_list *new=NULL;
	struct timestamp_list *tmp=NULL;

	if(timestamp_head == NULL){
		timestamp_head = (struct timestamp_list *) kmalloc(sizeof(struct timestamp_list), GFP_KERNEL);
		if(timestamp_head == NULL){
			printk(KERN_ALERT"\nCould not allocate memory to distance_head\n");
			return -1;
		}

		/*Add contents of List here*/
		timestamp_head->id = g_timestamp_list_count;
		memcpy(timestamp_head->mac, mac, 6);
		timestamp_head->timestamp.tv_sec = timestamp;
		timestamp_head->timestamp.tv_usec = timestamp_frac;
		/*End*/

		timestamp_head->next = NULL;
		timestamp_head->prev = NULL;

		g_timestamp_list_count++;
	}
	else{
		/*Check if entry already exists. If yes, just update it and Return.*/
		if(search_and_update_timestamp(mac, timestamp, timestamp_frac) == 1){
			return 0; /*Entry Updated Successfully*/
		}

		/*Entry does not exist. Add it and increment list count.*/
		new = (struct timestamp_list *) kmalloc(sizeof(struct timestamp_list), GFP_KERNEL);
		if(new == NULL){
			printk(KERN_ALERT"\nCould not allocate memory to a new timestamp entry\n");
			return -1;
		}

		/*Add contents of List here*/
		new->id = g_timestamp_list_count;
		memcpy(new->mac, mac, 6);
		new->timestamp.tv_sec = timestamp;
		new->timestamp.tv_usec = timestamp_frac;
		/*End*/

		tmp = timestamp_head;
		while(tmp != NULL && tmp->next != NULL){
			tmp = tmp->next;
		}
		tmp->next = new;
		new->prev = tmp;
		new->next = NULL;
		g_timestamp_list_count++;
	}
	return 0;
}

int free_entire_timestamp_list(void){
	
	struct timestamp_list *tmp=NULL, *tmp2=NULL;

	if(timestamp_head == NULL){
		printk(KERN_ALERT"\nTimestamp list is already empty\n");
		return 0;
	}
	tmp = timestamp_head;
	while(tmp != NULL){
		tmp2 = tmp->next;
		kfree(tmp);
		g_timestamp_list_count--;
		tmp = tmp2;
	}
	
	printk(KERN_ALERT"\nFree :  Timestamp list is NOW empty\n");
	return 0;
}

uint32_t search_for_timestamp_frac(uint8_t orig_sender[6]){

	struct timestamp_list *tmp=NULL;
	struct timeval result;
	result.tv_sec = 0;
	result.tv_usec = 0;

	if(timestamp_head == NULL){
		return(result.tv_usec);
	}

	tmp = timestamp_head;

	while(tmp != NULL){

		if((compare_mac_address(tmp->mac, orig_sender)) == 1){
			result = tmp->timestamp;
#if DEBUG
			printk(KERN_ALERT "\nsearch_for_timestamp : %ld:%ld", result.tv_sec, result.tv_usec);
#endif
			return(result.tv_usec);
		}
		tmp = tmp->next;
	}
	return(result.tv_usec);
}

uint32_t search_for_timestamp(uint8_t orig_sender[6]){

	struct timestamp_list *tmp=NULL;
	struct timeval result;
	result.tv_sec = 0;
	result.tv_usec = 0;

	if(timestamp_head == NULL){
		return(result.tv_sec);
	}

	tmp = timestamp_head;

	while(tmp != NULL){

		if((compare_mac_address(tmp->mac, orig_sender)) == 1){
			result = tmp->timestamp;
#if DEBUG
			printk(KERN_ALERT "\nsearch_for_timestamp : %ld:%ld", result.tv_sec, result.tv_usec);
#endif
			return(result.tv_sec);
		}
		tmp = tmp->next;
	}
	return(result.tv_sec);
}
	
/*
 * If entry exists, entry is updated and return 1
 * else
 * return 0
 * */
uint8_t search_and_update_timestamp(uint8_t orig_sender[6], uint32_t new_timestamp, uint32_t timestamp_frac){

	struct timestamp_list *tmp=NULL;

	if(timestamp_head == NULL){
		return 0;
	}

	tmp = timestamp_head;

	while(tmp != NULL){

		if((compare_mac_address(tmp->mac, orig_sender)) == 1){
			tmp->timestamp.tv_sec = new_timestamp;
			tmp->timestamp.tv_usec = timestamp_frac;
			return 1;
		}
		tmp = tmp->next;
	}
	return 0;
}

