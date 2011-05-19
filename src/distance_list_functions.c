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

#include "distance_list_functions.h"
#include "string_functions.h"

int g_distance_list_count;
struct distance_list *distance_head=NULL;

int add_or_update_entry(uint8_t final_destination[6], int distance){
	struct distance_list *new=NULL;
	struct distance_list *tmp=NULL;

	if(distance_head == NULL){
		distance_head = (struct distance_list *) kmalloc(sizeof(struct distance_list), GFP_KERNEL);
		if(distance_head == NULL){
			printk(KERN_ALERT"\nCould not allocate memory to distance_head\n");
			return -1;
		}

		/*Add contents of List here*/
		distance_head->id = g_distance_list_count;
		memcpy(distance_head->final_destination, final_destination, 6);
		distance_head->distance = distance;
		/*End*/

		distance_head->next = NULL;
		distance_head->prev = NULL;

		g_distance_list_count++;
	}
	else{
		/*Check if entry already exists. If yes, just update it and Return.*/
		if(search_and_update_distance(final_destination, distance) == 1){
			#ifdef MANIFOLD_DEBUG
			print_distance_list();
			#endif
			return 0; /*Entry Updated Successfully*/
		}

		if(g_distance_list_count < 50){
			/*Entry does not exist. Add it and increment list count.*/
			new = (struct distance_list *) kmalloc(sizeof(struct distance_list), GFP_KERNEL);
			if(new == NULL){
				printk(KERN_ALERT"\nCould not allocate memory to a new distance entry\n");
				return -1;
			}

			/*Add contents of List here*/
			new->id = g_distance_list_count;
			memcpy(new->final_destination, final_destination, 6);
			new->distance = distance;
			/*End*/

			tmp = distance_head;
			while(tmp != NULL && tmp->next != NULL){
				tmp = tmp->next;
			}
			tmp->next = new;
			new->prev = tmp;
			new->next = NULL;
			g_distance_list_count++;
		}
	}
	return 0;
}

int print_distance_list(void){

	struct distance_list *tmp=NULL;
	int i=0;

	if(distance_head == NULL){
		printk(KERN_ALERT"\nDistance list is empty\n");
		return -1;
	}

#if DEBUG	
	printk(KERN_ALERT"\nThe Distance list is as follows:\n");
#endif
	tmp = distance_head;
	while(tmp != NULL){

#if DEBUG
		printk(KERN_ALERT"\n%d/%d: id = %d final_destination = %x:%x:%x:%x:%x:%x distance = %d\n",
			(i+1), g_distance_list_count, tmp->id, 
			tmp->final_destination[0], tmp->final_destination[1], tmp->final_destination[2],
			tmp->final_destination[3], tmp->final_destination[4], tmp->final_destination[5],
			tmp->distance);
#endif

		tmp = tmp->next;
		i++;
	}
	return 0;
}

int free_entire_distance_list(void){
	
	struct distance_list *tmp=NULL, *tmp2=NULL;

	if(distance_head == NULL){
		printk(KERN_ALERT"\nDistance list is already empty\n");
		return 0;
	}
	tmp = distance_head;
	while(tmp != NULL){
		tmp2 = tmp->next;
		kfree(tmp);
		g_distance_list_count--;
		tmp = tmp2;
	}
	
	printk(KERN_ALERT"\nFree : Distance list is NOW empty\n");
	return 0;
}

uint8_t search_for_distance(uint8_t final_destination[6]){

	struct distance_list *tmp=NULL;

	if(distance_head == NULL){
		return 0xFF;
	}

	tmp = distance_head;

	while(tmp != NULL){

		if((compare_mac_address(tmp->final_destination, final_destination)) == 1){
			return tmp->distance;
		}
		tmp = tmp->next;
	}
	return 0xFF;
}
	
/*
 * If entry exists, entry is updated and return 1
 * else
 * return 0
 * */
uint8_t search_and_update_distance(uint8_t final_destination[6], uint8_t new_distance){

	struct distance_list *tmp=NULL;

	if(distance_head == NULL){
		return 0;
	}

	tmp = distance_head;

	while(tmp != NULL){

		if((compare_mac_address(tmp->final_destination, final_destination)) == 1){
			tmp->distance = new_distance;
			return 1;
		}
		tmp = tmp->next;
	}
	return 0;
}
	
