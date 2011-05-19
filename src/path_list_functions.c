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

#include "path_list_functions.h"
#include "string_functions.h"


int g_path_list_count;
struct path_list *path_head = NULL;

int add_or_update_path_entry(uint8_t mac0[6], uint8_t mac1[6], uint8_t mac2[6], uint8_t mac3[6], uint32_t session_id) {
  
	struct path_list *new = NULL;
	struct path_list *tmp = NULL;
	
	if(path_head == NULL) {
	  
		path_head = (struct path_list *) kmalloc(sizeof(struct path_list), GFP_KERNEL);
		memset(path_head, 0, sizeof(struct path_list));

                if(path_head == NULL){
                        printk(KERN_ALERT"\nCould not allocate memory to stat_head\n");
                        return -1;
                }	  
                
		path_head->id = g_path_list_count;
		memcpy(path_head->src_mac, mac0, 6);
		memcpy(path_head->hop1_mac, mac1, 6);
		memcpy(path_head->hop2_mac, mac2, 6);
		memcpy(path_head->hop3_mac, mac3, 6);
		
		path_head->curr_session_id = session_id;
		path_head->prev_session_id = 0;
		path_head->curr_num_packets = 1;
		path_head->prev_num_packets = 0;

		path_head->next = NULL;
		path_head->prev = NULL;

		g_path_list_count++;		

		return 1;
		
	}
	else {

		if(search_and_update_path(mac0, mac1, mac2, mac3, session_id) == 1)
			return 0; 
		
		if(g_path_list_count <50){
		  
			new = (struct path_list *) kmalloc(sizeof(struct path_list), GFP_KERNEL);

			memset(new, 0, sizeof(struct path_list));

			if(new == NULL){
				printk(KERN_ALERT"\nCould not allocate memory to a new path entry\n");
				return -1;
			}

			printk(KERN_ALERT"Check");

			new->id = g_path_list_count;
			memcpy(new->src_mac, mac0, 6);
			memcpy(new->hop1_mac, mac1, 6);
			memcpy(new->hop2_mac, mac2, 6);
			memcpy(new->hop3_mac, mac3, 6);			
			
			new->curr_session_id = session_id;
			new->prev_session_id = 0;
			new->curr_num_packets = 1;
			new->prev_num_packets = 0;
			
			tmp = (struct path_list *)path_head;
			while(tmp != NULL && tmp->next != NULL){
				tmp = tmp->next;
			}
			tmp->next = new;
			new->prev = tmp;
			new->next = NULL;

			g_path_list_count++;
			
			return 1;
			
		}
		return 0;
	}
	
	return 0;
}

uint8_t search_and_update_path(uint8_t mac0[6], uint8_t mac1[6], uint8_t mac2[6], uint8_t mac3[6], uint32_t session_id) {
  
        struct path_list *tmp=NULL;

        if(path_head == NULL){
                return 0;
        }

        tmp = path_head;
 
        while(tmp != NULL){

                if((compare_mac_address(tmp->src_mac, mac0)) == 1 && (compare_mac_address(tmp->hop1_mac, mac1)) == 1 && (compare_mac_address(tmp->hop2_mac, mac2)) == 1 && (compare_mac_address(tmp->hop3_mac, mac3)) == 1){

			if(tmp->curr_session_id > 4 && session_id < 2) {
			  
				tmp->curr_session_id = session_id;
				tmp->prev_session_id = 0;
				tmp->curr_num_packets = 1;
				tmp->prev_num_packets = 0;
				
			}
			else if(session_id > tmp->curr_session_id) {

				tmp->prev_session_id = tmp->curr_session_id;
				tmp->prev_num_packets = tmp->curr_num_packets;
				tmp->curr_session_id = session_id;
				tmp->curr_num_packets = 1;
				
			}
			else {
			  
				tmp->curr_num_packets++;
			}
			return 1;
		}
		else {
			tmp = tmp->next;
		}
	}
	
	return 0;
  
}