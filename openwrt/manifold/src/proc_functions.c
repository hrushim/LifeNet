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
Developers: Ashwin Paranjpe, Santosh Vempala. Hrushikesh Mehendale
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
#include<linux/unistd.h>
#include<linux/proc_fs.h>


#include "distance_list_functions.h"
#include "stat_list_functions.h"
#include "path_list_functions.h"
#include "string_functions.h"

/*Extern Declarations*/
extern struct net_device *d;

extern int global_manifold_disable;
extern uint8_t bcast_fwd_threshold;

//extern int global_recieved_count;
//extern int global_transmitted_count;
//extern int g_manifold_recieved_count;
//extern int g_manifold_transmitted_count;
//extern int g_manifold_session_transmitted_count;
extern int g_session_id;
//extern int g_manifold_last_session_transmitted_count;
extern int g_last_session_id;

extern struct distance_list *distance_head;

#define MAX_WDL_BUFFER_SIZE 40
#define COMMAND_LENGTH 20
#define ARGUMENT_LENGTH 20

static unsigned long wdl_buffer_size = 0;
static char wdl_buffer[MAX_WDL_BUFFER_SIZE];
static int tokenize(char *, int, char *, char *, char *);
static int handle_commands(char *, char *, char *);
struct path_list *tmp = NULL;
int read_flag = 0;

/*This function is called everytime the proc entry is read. i.e. cat /proc/wdl
 *  * It prints statistics about the number of packets received and transmitted through this module
 *   * */
int wdl_read(char* page, char** start, off_t off, int count, int* eof, void *data){

	int len=0;

#if MYMANET_STORE_PATH

	//printk(KERN_ALERT"Read_flag %d", read_flag);
	
	if(off > 0) {
	  *eof = 1;
	  return len;
	}
	
        if (!d) {
	  
                len = sprintf(page, "No device specified.\n");
                return len;
		
        }
        
	if(read_flag == 0) {
	  
		len=0;
		memset(page, '\0', count);
		tmp = (struct path_list *) path_head;
		while(tmp != NULL){

			if((len + 85)>=count) {
				break;
			}
			else {
			  
				len += sprintf(page+len, "PATH %x:%x:%x:%x:%x:%x %x:%x:%x:%x:%x:%x %x:%x:%x:%x:%x:%x %x:%x:%x:%x:%x:%x %d %d %d %d\n", tmp->src_mac[0], tmp->src_mac[1], 
			  tmp->src_mac[2], tmp->src_mac[3], tmp->src_mac[4], tmp->src_mac[5], tmp->hop1_mac[0], tmp->hop1_mac[1], 
			  tmp->hop1_mac[2], tmp->hop1_mac[3], tmp->hop1_mac[4], tmp->hop1_mac[5], tmp->hop2_mac[0], tmp->hop2_mac[1],
			  tmp->hop2_mac[2], tmp->hop2_mac[3], tmp->hop2_mac[4], tmp->hop2_mac[5], tmp->hop3_mac[0], tmp->hop3_mac[1],
			  tmp->hop3_mac[2], tmp->hop3_mac[3], tmp->hop3_mac[4], tmp->hop3_mac[5], tmp->prev_num_packets, tmp->prev_session_id, tmp->curr_num_packets, tmp->curr_session_id);
			  
			}
			tmp = tmp->next;				    
				    
		}
		
		if(tmp!=NULL) {
		
			read_flag = 1;
			*eof = 1;
			return len;
		
		}
		else {
		
			read_flag = 0;
			*eof = 1;
			return len;
		
		}
			
	} else {
	
		len=0;
		memset(page, '\0', count);
	  
		while(tmp != NULL){

			if((len + 85)>=count) {
				break;
			}
			else {
			  
				len += sprintf(page+len, "PATH %x:%x:%x:%x:%x:%x %x:%x:%x:%x:%x:%x %x:%x:%x:%x:%x:%x %x:%x:%x:%x:%x:%x %d %d\n", tmp->src_mac[0], tmp->src_mac[1], 
			  tmp->src_mac[2], tmp->src_mac[3], tmp->src_mac[4], tmp->src_mac[5], tmp->hop1_mac[0], tmp->hop1_mac[1], 
			  tmp->hop1_mac[2], tmp->hop1_mac[3], tmp->hop1_mac[4], tmp->hop1_mac[5], tmp->hop2_mac[0], tmp->hop2_mac[1],
			  tmp->hop2_mac[2], tmp->hop2_mac[3], tmp->hop2_mac[4], tmp->hop2_mac[5], tmp->hop3_mac[0], tmp->hop3_mac[1],
			  tmp->hop3_mac[2], tmp->hop3_mac[3], tmp->hop3_mac[4], tmp->hop3_mac[5], tmp->prev_num_packets, tmp->prev_session_id);
			  
			}
			tmp = tmp->next;				    
				    
		}
		
		if(tmp!=NULL) {
		
			read_flag = 1;
			*eof = 1;
			return len;	
		
		}
		else {

			read_flag = 0;
			*eof = 1;
			return len;
		
		}
	}
#endif
	return len;
}



/*This function is called everytime the proc entry is written. i.e. for e.g. echo 1 > /proc/wdl
 *  * It can be used to transfer control information from Userspace to Kernel space.
 *   * */
int wdl_write(struct file *file, const char *buffer, unsigned long count, void *data){

	char command[COMMAND_LENGTH]="\0", arg1[ARGUMENT_LENGTH]="\0", arg2[ARGUMENT_LENGTH]="\0";

        /*Get the wdl buffer size*/
        wdl_buffer_size = count;

        if(wdl_buffer_size >= MAX_WDL_BUFFER_SIZE){
                wdl_buffer_size = MAX_WDL_BUFFER_SIZE - 1;
        }

        /*Extract data from the wdl buffer*/
        if(copy_from_user(wdl_buffer, buffer, wdl_buffer_size)){
                return -EFAULT;
        }

        wdl_buffer[wdl_buffer_size] = '\0';

        if(strncmp(wdl_buffer, "stop", strlen("stop"))==0){
                /*Stop the Manifold Re-transmission*/
                global_manifold_disable = 1;
                printk(KERN_ALERT"\nManifold Routing Disabled\n");
        }
        else if(strncmp(wdl_buffer, "start", strlen("start"))==0){
                /*Start the Manifold Re-transmission*/
                global_manifold_disable = 0;
                printk(KERN_ALERT"\nManifold Routing Enabled\n");
        }
	else{
		tokenize(wdl_buffer, wdl_buffer_size, command, arg1, arg2);
		/*Parse the command and perform appropriate actions*/
		if((handle_commands(command, arg1, arg2)) == -1)
			printk(KERN_ALERT"\nCould not execute command\n");
	}
	
        return wdl_buffer_size;

}


static int tokenize(char wdl_buffer[40], int wdl_buffer_size, char command[COMMAND_LENGTH], char arg1[ARGUMENT_LENGTH], char arg2[ARGUMENT_LENGTH])
{
	int i=0, j=0, argnum=0;
	for(i = 0; i < wdl_buffer_size; i++){

		if((argnum == 0)){
			if((wdl_buffer[i] != ' ') && (j < COMMAND_LENGTH))
				command[j++] = wdl_buffer[i];
			else{ 
				command[j] = '\0';
				argnum = 1;
				j = 0;
			}

		}
		else if((argnum == 1)){
			if((wdl_buffer[i] != ' ') && (j < ARGUMENT_LENGTH))
				arg1[j++] = wdl_buffer[i];
			else{ 
				arg1[j] = '\0';
				argnum = 2;
				j = 0;
			}
		}
		else if((argnum == 2)){

			if((wdl_buffer[i] != ' ') && (j < ARGUMENT_LENGTH) &&(wdl_buffer[i] != '\0'))
				arg2[j++] = wdl_buffer[i];
			else{ 
				arg2[j] = '\0';
				argnum = 3;
				j = 0;
			}
		}
	}
	return 0;

}

static int handle_commands(char command[COMMAND_LENGTH], char arg1[ARGUMENT_LENGTH], char arg2[ARGUMENT_LENGTH])
{
	uint8_t mac[6];
	if(strncmp(command, "distance", strlen("distance")) == 0){
		/*Add Distance entry in the List*/
		if(hwaddr_aton(arg1, mac) != 6)
			return -1;	
		add_or_update_entry(mac, simple_strtol(arg2, &arg2, 10));
		print_distance_list();
	}
	if(strncmp(command, "set_fwd_threshold", strlen("set_fwd_threshold")) == 0){
		if(strlen(arg1) == 1) {
			bcast_fwd_threshold = arg1[0] - 0x30;
		}
		if(strlen(arg1) == 2) {
			bcast_fwd_threshold = 0;
			bcast_fwd_threshold = arg1[1] - 0x30;
			bcast_fwd_threshold += (arg1[0] - 0x30) * 10;
		}
	}
	return 0;
} 



/* /proc/rxstats functions*/

/*This function is called everytime the proc entry is read. i.e. cat /proc/rxstats
 * It prints the number of packets received and the MAC of Senders
 */
int rxstats_read(char* page, char** start, off_t off, int count, int* eof, void *data)
{
        int len=0;
	struct stat_list *tmp = NULL;	

        if (off > 0) {
                *eof = 1;
                return len;
        }

        if (!d) {
                len = sprintf(page, "No device specified.\n");
                return len;
        }
        
	tmp = (struct stat_list *) stat_head;
	while(tmp != NULL){

		len += sprintf(page+len, "%x:%x:%x:%x:%x:%x %d %d %d %d %d\n", tmp->mac[0], tmp->mac[1], 
			tmp->mac[2], tmp->mac[3], tmp->mac[4], tmp->mac[5], tmp->last_num_rx, tmp->last_session_id, tmp->last_num_rx_bcast, 
			tmp->num_rx, tmp->session_id);

		tmp = tmp->next;
	}
	return len;
}


/* /proc/txstats functions*/

/*This function is called everytime the proc entry is read. i.e. cat /proc/txstats
 * since we are only tranmitting Broadcast packets,
 * this function, currently just prints the Manifold packets transmitted by this device.
 * TODO: It prints the number of packets transmitted and the MAC of destinations
 */
int txstats_read(char* page, char** start, off_t off, int count, int* eof, void *data){
  
        int len=0;
	struct stat_list *tmp = NULL;	

        if (off > 0) {
                *eof = 1;
                return len;
        }

        if (!d) {
                len = sprintf(page, "No device specified.\n");
                return len;
        }
        
	tmp = (struct stat_list *) stat_head;
	while(tmp != NULL){

		len += sprintf(page+len, "%x:%x:%x:%x:%x:%x %d %d %d %d %d %d %d %d\n", tmp->mac[0], tmp->mac[1], tmp->mac[2], tmp->mac[3], tmp->mac[4], tmp->mac[5], tmp->last_num_tx, tmp->last_tx_session_id, tmp->num_tx, tmp->tx_session_id, tmp->last_num_fwd, tmp->last_fwd_session_id, tmp->num_fwd, tmp->fwd_session_id);

		tmp = tmp->next;
	}
	return len;
}


/* /proc/distance functions*/

/*This function is called everytime the proc entry is read. i.e. cat /proc/distance
 */
int distance_read(char* page, char** start, off_t off, int count, int* eof, void *data)
{

	struct distance_list *tmp=NULL;
	int i=0;
	int len=0;

	if (off > 0) {
		*eof = 1;
		return len;
	}

	if (!d) {
		len = sprintf(page, "No device specified.\n");
		return len;
	}


	if(distance_head == NULL){
		len = sprintf(page,"\nDistance list is empty\n");
		return len;
	}                       

	tmp = distance_head;
	while(tmp != NULL){

		len += sprintf(page+len,"\n%d/%d: ID = %d Final Destination = %x:%x:%x:%x:%x:%x Distance = %d\n\n",
				(i+1), g_distance_list_count, tmp->id,
				tmp->final_destination[0], tmp->final_destination[1], tmp->final_destination[2],
				tmp->final_destination[3], tmp->final_destination[4], tmp->final_destination[5],
				tmp->distance);

		tmp = tmp->next;
		i++;
	}       

	return len;
}
