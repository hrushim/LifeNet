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

#ifndef NEW_KERNEL	/*Parameter to Makefile*/
#define NEW_KERNEL 1	/*Default Value*/
#endif

#ifndef MYMANET_ALPHA
#define MYMANET_ALPHA 20
#endif

#ifndef MYMANET_BETA
#define MYMANET_BETA 5
#endif

#ifndef MYMANET_KERNEL_VERSION_6_30_PLUS
#define MYMANET_KERNEL_VERSION_6_30_PLUS 0
#endif

#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/sched.h>
#include<linux/module.h>
#include<linux/moduleparam.h>
#include<linux/netdevice.h>
#include<linux/etherdevice.h>
#include<linux/random.h>
#if NEW_KERNEL
#include<net/net_namespace.h>
#endif
#include<linux/proc_fs.h>
#include<linux/ip.h>
#include<net/ip.h>
#include<linux/string.h>
#include<linux/list.h>
#include<linux/timer.h>
#include<linux/time.h>

#include "main.h"
#include "string_functions.h"
#include "proc_functions.h"
#include "distance_list_functions.h"
#include "stat_list_functions.h"
#include "timer_functions.h"
#include "timestamp_list_functions.h"
#include "path_list_functions.h"

MODULE_LICENSE("GPL");


/*Start of Manifold Module Code*/

module_param(device_name, charp, S_IRUGO);	/*Device name can be given as a parameter to the module*/
module_param(mymanet_alpha, int, 0);	/*Device name can be given as a parameter to the module*/
module_param(mymanet_beta, int, 0);	/*Device name can be given as a parameter to the module*/

struct mymanet_header{
	uint8_t session_id;
	uint8_t hops_remaining;
	uint8_t distance;
	uint8_t orig_distance;
	//struct timeval timestamp;
	uint32_t timestamp;
	uint8_t original_source[6];
	uint8_t final_destination[6];
	uint8_t eth_type_original[2];
#if MYMANET_STORE_PATH
	uint8_t hop1_mac[6];
	uint8_t hop2_mac[6];
	uint8_t hop3_mac[6];
#endif
};

uint8_t bcast_fwd_threshold = 2;

/*This is not a standalone function and shall be called from wdl_handle_receive
 * function ONLY when there is a need to change the MAC header of the packet.
 * This function simply makes the necessary changes in the packet and then 
 * calls the wdl_hard_start_xmit function which will then do the actual work 
 * of transmission. Currently it retransmits the broadcast packet by spoofing
 * its own MAC as the source MAC
 * */
int wdl_modify_and_transmit(struct sk_buff *skb, struct net_device *netdev, uint8_t dest_mac[6], uint8_t src_mac[6], struct mymanet_header mymanethdr)
{

	int orig_header_room=0;
	uint32_t sec_holder;

	uint8_t eth_type_manifold[2];
	eth_type_manifold[0] = 0x33;
	eth_type_manifold[1] = 0x33;


	orig_header_room = (int)(skb->data - skb->head);
	if(orig_header_room < (ETHERNET_HEADER_SIZE + MANIFOLD_HEADER_SIZE)){
		printk(KERN_ALERT"\nSTRANGE : Insufficient headroom for constructing Manifold Packet\n");
		kfree_skb(skb);
		return -1;
	}

	sec_holder = htonl(mymanethdr.timestamp);
	mymanethdr.timestamp = sec_holder;

#if MYMANET_STORE_PATH	

	if(mymanethdr.hops_remaining == 3) {
		memcpy(mymanethdr.hop1_mac, src_mac, 6);
	} else if (mymanethdr.hops_remaining == 2) {
		memcpy(mymanethdr.hop2_mac, src_mac, 6);
	} else if (mymanethdr.hops_remaining == 1) {
		memcpy(mymanethdr.hop3_mac, src_mac, 6);	  
	} 

#endif	  

	memcpy(skb_push(skb, MANIFOLD_HEADER_SIZE), &mymanethdr, MANIFOLD_HEADER_SIZE);

	memcpy(skb_push(skb, 2), eth_type_manifold, 2);
	memcpy(skb_push(skb, 6), src_mac, 6);
	memcpy(skb_push(skb, 6), dest_mac, 6);

	add_or_update_stat_entry(mymanethdr.final_destination, 2, mymanethdr.session_id, mymanethdr.final_destination);

	g_manifold_transmitted_count++;
	global_transmitted_count++;
	g_manifold_session_transmitted_count++;

#if DEBUG
	printk(KERN_ALERT "MODIFIED and TRANSMITTED pkt from  %x:%x:%x:%x:%x:%x to %x:%x:%x:%x:%x:%x type %x:%x", 
			src_mac[0], src_mac[1], src_mac[2], src_mac[3], src_mac[4], src_mac[5],
			dest_mac[0], dest_mac[1], dest_mac[2], dest_mac[3], dest_mac[4], dest_mac[5], 
			eth_type_manifold[0], eth_type_manifold[1]);
#endif
			
	if(netdev->flags & IFF_UP)
		return (dev_queue_xmit(skb));
	else{
		printk(KERN_ALERT "\n\nDevice was DOWN !! \n\n");
		return NETDEV_TX_BUSY;	
	}
}

uint8_t difftime(const uint32_t *old, const uint32_t *new)
{
#define NEW 1
#define OLD 2
#define DUPLICATE 0

	if(*new > old)
		return NEW;
	else if(*new == *old){
		if(*new > *old)
			return NEW;
		else if(*new == *old)
			return DUPLICATE;
		else 
			return OLD;
	}
	else
		return OLD;
}

uint8_t routing_decision(uint8_t distance_from_me, uint8_t distance_from_orig_sender, uint8_t packet_distance, uint32_t old_timestamp, uint32_t new_timestamp)
{
#define DEBUG_ON_2 0
#define DROP 0
#define FORWARD 1
#define NEW 1
#define OLD 2
#define DUPLICATE 0

	if(distance_from_me < (packet_distance + mymanet_alpha)){
		if(difftime(&old_timestamp, &new_timestamp) == OLD){ 	/*This is an old packet, Drop it.*/
			return DROP;
		}
		else if(difftime(&old_timestamp, &new_timestamp) == DUPLICATE){
			if(distance_from_orig_sender > distance_from_me){
				return FORWARD;
			}
			else if((((distance_from_me - distance_from_orig_sender)/255) * 100) < mymanet_beta){
				return FORWARD;
			}
			else{
				return DROP;
			}
		}
		else if(difftime(&old_timestamp, &new_timestamp) == NEW){
			return FORWARD;
		}
	}	
	else{ 
		return DROP;
	}
#if DEBUG			
	printk(KERN_ALERT "\nThis shouldn't be ever printed. \n");
#endif
	return DROP;
}

/*This is the primary receive function for the newly defined packet type
 *It is assumed that when we arrive at this function, the skb->data points to the start of 
 *the payload of the packet after the MAC header
 *It internally calls the function eth_type_trans which actually classifies the packet
 *as being destined to the host, broadcast and so on
 *Based on information returned from eth_type_trans, this function will then re-transmit
 *if the packet is broadcast or destined to some other host.
 **/
int wdl_handle_recieve(struct sk_buff *skb, struct net_device *netdev, struct packet_type *ptype, struct net_device *orig_dev)
{

	uint8_t dest_mac[6];
	uint8_t src_mac[6];
	uint8_t eth_type[2];
	uint8_t p_fwd_deno = 2;
	struct mymanet_header mymanethdr;
	uint32_t sec_holder;
	unsigned int rand_value;
	uint8_t flag=0;

	uint8_t distance_from_me = 0;
	uint32_t old_timestamp;

	int orig_header_room=0;

	//If the packet cannot be processed due to some key fields being null, free the socket buffer and return
	if((skb == NULL) || (netdev == NULL) || (ptype == NULL) || (orig_dev == NULL)){
		if(skb != NULL){
			kfree_skb(skb);
		}
		return 0;
	}

	orig_header_room = (int)(skb->data - skb->head);

	//If the packet cannot be processed due to insufficient header room, free the socket buffer and return
	if((orig_header_room) < (ETHERNET_HEADER_SIZE)){
		if(skb != NULL){
			kfree_skb(skb);
		}
		return 0;
	}

	if((global_recieved_count++) == 10) {
#if DEBUG
		printk(KERN_ALERT "\nRECIEVED 10 pkts on device %s", d->name);
#endif
	}

	/*Since we are at the start of the payload after Eth header, go back and fetch the info.*/
	memcpy(dest_mac, ( (skb->data - (2 * 6)) - 2 ), 6);
	memcpy(src_mac, ( (skb->data - 6) - 2 ), 6);
	memcpy(eth_type, ( skb->data - 2 ), 2);

	memcpy(&mymanethdr, skb->data, MANIFOLD_HEADER_SIZE);
	sec_holder = ntohl(mymanethdr.timestamp);
	mymanethdr.timestamp = sec_holder;
	

//#if DEBUG
	printk(KERN_ALERT"    \nReceived Session ID as %d\n", mymanethdr.session_id);
	printk(KERN_ALERT"    Received Hops Remaining as %d\n", mymanethdr.hops_remaining);
	printk(KERN_ALERT"    Received Distance as %d\n", mymanethdr.distance);
	printk(KERN_ALERT"    Received Original Distance as %d\n", mymanethdr.orig_distance);
	printk(KERN_ALERT"    Received Timestamp as %ld\n", mymanethdr.timestamp);
	printk(KERN_ALERT"    Received Original Source MAC as %x:%x:%x:%x:%x:%x\n", mymanethdr.original_source[0], mymanethdr.original_source[1], 
		mymanethdr.original_source[2], mymanethdr.original_source[3], mymanethdr.original_source[4], mymanethdr.original_source[5]);
	printk(KERN_ALERT"    Received Final Dest MAC as %x:%x:%x:%x:%x:%x\n", mymanethdr.final_destination[0], mymanethdr.final_destination[1], 
		mymanethdr.final_destination[2], mymanethdr.final_destination[3], mymanethdr.final_destination[4], mymanethdr.final_destination[5]);
	printk(KERN_ALERT"    Received Original Type as %x:%x\n", mymanethdr.eth_type_original[0], mymanethdr.eth_type_original[1]);
//#endif


	/*Do skb_pull of MANIFOLD_HEADER_SIZE*/
	skb_pull(skb, MANIFOLD_HEADER_SIZE);
	/*End: Extraction of Manifold Header fields ends here*/

	/*eth_type_trans will understand only the original eth_type numbers*/
	/*Build a normal ethernet layer packet header so that eth_type_trans can synthesize it.*/
	memcpy(skb_push(skb, 2), mymanethdr.eth_type_original, 2 );
	memcpy(skb_push(skb, 6), mymanethdr.original_source, 6);
	memcpy(skb_push(skb, 6), mymanethdr.final_destination, 6);

	skb->pkt_type = PACKET_HOST; /* Assumed. eth_type_trans will overwrite it anyways.*/
	/*Remember that eth_type_trans function does skb_pull of length ETH_HLEN. 
	 * So the newly created Eth header will be stripped off
	 * */
	skb->protocol = eth_type_trans(skb, netdev);

	/*Start of Actual Processing*/
	/*No Need to do SKB expansion since this is a Manifold packet which has already been expanded
	 * to have sufficient head-room*/

	if (skb->pkt_type == PACKET_HOST) {
	
		/*Add or Update this MAC entry to our Stat List*/
		add_or_update_stat_entry(mymanethdr.original_source, 0, mymanethdr.session_id, mymanethdr.final_destination);

#if MYMANET_STORE_PATH
		add_or_update_path_entry(mymanethdr.original_source, mymanethdr.hop1_mac, mymanethdr.hop2_mac, mymanethdr.hop3_mac, mymanethdr.session_id);
#endif

		netif_rx(skb);
		return 0;
	}
	if ((skb->pkt_type == PACKET_BROADCAST) || (skb->pkt_type == PACKET_OTHERHOST)) {
		/*We need to forward such packets after making some changes i.e. broadcast retransmit*/

		g_manifold_recieved_count++;

		/*Get my distance from this final destination*/
		distance_from_me = search_for_distance(mymanethdr.final_destination);

		/*Initialize old timestamp*/
		old_timestamp = 0;

		/*Get the timestamp of this final destination*/
		old_timestamp = search_for_timestamp(mymanethdr.original_source);
		if((old_timestamp == 0)){
			/*This mac entry does not exist. Add it.*/	
			add_or_update_timestamp_entry(mymanethdr.original_source, mymanethdr.timestamp);
		}
		else if(difftime(&old_timestamp, &mymanethdr.timestamp)==1){		
			/*This is a NEW packet from an existing mac. Update the entry*/
			add_or_update_timestamp_entry(mymanethdr.original_source, mymanethdr.timestamp);
		}
		else{
			/*This is neither a new mac entry, nor a new packet from an existing mac entry. 
			Possibly a duplicate packet. Do not update our timestamps.*/
		}

		/*Reduce the Number of Remaining Hops for this packet*/
		mymanethdr.hops_remaining = mymanethdr.hops_remaining - 1;

		if((!global_manifold_disable) && (distance_from_me != 0) && (mymanethdr.hops_remaining != 0)
				&& (memcmp(mymanethdr.final_destination, netdev->dev_addr, 6) != 0)) { 
			if(routing_decision(distance_from_me, mymanethdr.orig_distance, mymanethdr.distance, old_timestamp, mymanethdr.timestamp) || 
				(distance_from_me == 0xFF)) {

//				if(skb->pkt_type == PACKET_BROADCAST){

					if(bcast_fwd_threshold < 3) {
						p_fwd_deno = 2;
					}
					else {
						p_fwd_deno = bcast_fwd_threshold;
					}
					rand_value = random32()%INT_MAX;
					if(rand_value < (unsigned int)(2*(float)(INT_MAX))/p_fwd_deno) {
						mymanethdr.distance = distance_from_me;
						wdl_modify_and_transmit((skb_copy(skb, GFP_ATOMIC)), netdev, g_broadcast_mac, netdev->dev_addr, mymanethdr);
					}
//				}
//				else {

//					mymanethdr.distance = distance_from_me;
//					wdl_modify_and_transmit((skb_copy(skb, GFP_ATOMIC)), netdev, g_broadcast_mac, netdev->dev_addr, mymanethdr);
//				}
			}
		}

		if(skb->pkt_type == PACKET_BROADCAST){
			flag = 1;
			/*Add or Update this MAC entry to our Stat List*/
			add_or_update_stat_entry(mymanethdr.original_source, 0, mymanethdr.session_id, mymanethdr.final_destination);
			netif_rx(skb); /*This is originally a Broadcast packet. For E.g. Heart-Beats*/
		}
		if((flag != 1) && (skb != NULL)){
			kfree_skb(skb);
		}

		return 0;
	}
	else{	/*Packet is neither broadcast nor unicast. In the case of MyMANET, it appears to be be a garbled packet. Free the skb.*/
		if(skb != NULL){
			kfree_skb(skb);
		}
		return 0;	
	}
}


/*This function will do the actual job of transmitting the packet through this module
 * It calls the lower level hard_start_xmit function which was earlier saved during the module initialization*/
int wdl_hard_start_xmit(struct sk_buff *skb_original, struct net_device *netdev)
{

  	struct timeval temp_time;
	uint8_t eth_type_manifold[2];
	int orig_header_room = 0, new_header_room = 0;
	struct sk_buff *skb = NULL;
	struct mymanet_header mymanethdr;
	uint32_t sec_holder;

#if MYMANET_STORE_PATH	
	uint8_t bcast_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif

	eth_type_manifold[0]=0x33;
	eth_type_manifold[1]=0x33;

	if((device_hard_start_xmit_backup != NULL) && (netdev != NULL)){

		memcpy(mymanethdr.final_destination, skb_original->data, 6);
		memcpy(mymanethdr.original_source, skb_original->data + 6, 6);
		memcpy(mymanethdr.eth_type_original, skb_original->data + 12, 2);

		/*CHECK whether this is a Manifold packet
		 *coming from the reception path */
		if((mymanethdr.eth_type_original[0] == eth_type_manifold[0]) && (mymanethdr.eth_type_original[1] == eth_type_manifold[1])){

			//g_manifold_transmitted_count++;
			//global_transmitted_count++;
			//g_manifold_session_transmitted_count++;
			/*This is a Manifold packet coming from our Reception Path.
			 * Dont touch this packet. Just send it as it is.
			 * */
			/*Actual Transmission happens here*/
			if(netdev->flags & IFF_UP)
				return (*device_hard_start_xmit_backup)(skb_original, netdev);
			else{
				printk(KERN_ALERT "\n\nDevice was DOWN !! \n\n");
				return NETDEV_TX_BUSY;	
			}

		}
		else{

			g_manifold_transmitted_count++;
			global_transmitted_count++;
			g_manifold_session_transmitted_count++;
			/*This is NOT a Manifold Packet.
			 * Convert it into a Manifold Packet.
			 * */

			//printk(KERN_ALERT "\n\nTotal # Tx: %d, Total # Tx in this session: %d \n\n", global_transmitted_count, g_manifold_session_transmitted_count);

			/*Add or Update this MAC entry in our Stat List*/
			add_or_update_stat_entry(mymanethdr.final_destination, 1, g_session_id, mymanethdr.final_destination);

			orig_header_room = (int)(skb_original->data - skb_original->head);

			/*Here add the header room required for the new fields in Manifold packet*/
			skb = skb_copy_expand(skb_original, (orig_header_room + MANIFOLD_HEADER_SIZE), 0, GFP_ATOMIC);

			skb_pull(skb, ETHERNET_HEADER_SIZE);

			/*Get the distance for this final Destination*/
			mymanethdr.distance = search_for_distance(mymanethdr.final_destination);
			mymanethdr.orig_distance = mymanethdr.distance;
			mymanethdr.hops_remaining = MAX_HOPS;
			mymanethdr.session_id = g_session_id;
			do_gettimeofday(&temp_time);
			sec_holder = htonl(temp_time.tv_sec);
			mymanethdr.timestamp = sec_holder;
#if MYMANET_STORE_PATH
			memcpy(mymanethdr.hop1_mac, bcast_mac, 6);
			memcpy(mymanethdr.hop2_mac, bcast_mac, 6);
			memcpy(mymanethdr.hop3_mac, bcast_mac, 6);			
#endif

			memcpy(skb_push(skb, MANIFOLD_HEADER_SIZE), &mymanethdr, MANIFOLD_HEADER_SIZE);

			/*Spoof our own address as source address and Dest MAC = Broadcast*/
			memcpy(skb_push(skb, 2), eth_type_manifold, 2 );
			memcpy(skb_push(skb, 6), netdev->dev_addr, 6);
			memcpy(skb_push(skb, 6), g_broadcast_mac, 6);

			new_header_room = (int)(skb->data - skb->head);

			kfree_skb(skb_original);

			/*Actual Transmission happens here*/
			if(netdev->flags & IFF_UP)
				return (*device_hard_start_xmit_backup)(skb, netdev);
			else{
				printk(KERN_ALERT "\n\nDevice was DOWN !! \n\n");
				return NETDEV_TX_BUSY;	
			}
		}
	}
	kfree_skb(skb_original);
	return 0;
}


/*Basic Module specific code starts here*/

/* This function is called everytime when the module is un-loaded
 * It re-instates the hard_start_xmit function to the lower level function and 
 * deletes the proc entry as well as removes the new registered packet type*/
static void hello_exit(void)
{
	#if MYMANET_KERNEL_VERSION_6_30_PLUS
		struct net_device_ops new_netdev_ops;
		struct net_device_ops *new_netdev_ops_ptr = &new_netdev_ops;
	#endif
	mod_timer(&g_session_timer, jiffies + 1);
	del_timer_sync(&g_session_timer);

	if((device_hard_start_xmit_backup != NULL) && (d !=NULL)) {
		#if MYMANET_KERNEL_VERSION_6_30_PLUS
			memcpy(&new_netdev_ops, d->netdev_ops, sizeof(struct net_device_ops));
			new_netdev_ops_ptr->ndo_start_xmit = device_hard_start_xmit_backup;
			memcpy((void *)d->netdev_ops, &new_netdev_ops, sizeof(struct net_device_ops));
		#else
			d->hard_start_xmit = device_hard_start_xmit_backup;
		#endif
	}

	if(d != NULL)
		dev_put(d);

	dev_remove_pack(&manifold_ptype);

	remove_proc_entry("wdl", NULL);
	remove_proc_entry("rxstats", NULL);
	remove_proc_entry("txstats", NULL);

	free_entire_distance_list();
	free_entire_stat_list();

	printk(KERN_ALERT "\nByee! nice world\n");
}

/*This is the MAIN module initialization function for the module.
 *It's called everytime we try to load the module
 *It creates a new proc entry and also registers a new packet type
 *It grabs the main DEVICE structure(net_device) for the interface specified in the module.
 *It then saves the lower level transmit function in a function pointer for later use; and then
 *replaces it with the module's own transmit function i.e. wdl_hard_start_xmit
 * */
static int hello_init(void)
{

	struct proc_dir_entry *proc_entry, *proc_entry_rxstats, *proc_entry_txstats, *proc_entry_distance;

	#if MYMANET_KERNEL_VERSION_6_30_PLUS
		struct net_device_ops new_netdev_ops;
		struct net_device_ops *new_netdev_ops_ptr = &new_netdev_ops;
	#endif

	dev_add_pack(&manifold_ptype);

	proc_entry = create_proc_entry("wdl", 0444, NULL);
	if (proc_entry == NULL) {
		hello_exit();
		return -ENOMEM;
	} 
	else {
		proc_entry->read_proc = wdl_read;
		proc_entry->write_proc = wdl_write;
		printk(KERN_ALERT "ash: proc entry created\n");
	}

	proc_entry_rxstats = create_proc_entry("rxstats", 0444, NULL);
	if (proc_entry_rxstats == NULL) {
		hello_exit();
		return -ENOMEM;
	} 
	else {
		proc_entry_rxstats->read_proc = rxstats_read;
		proc_entry_rxstats->write_proc = NULL;
		printk(KERN_ALERT "ash: rxstats proc entry created\n");
	}

	proc_entry_txstats = create_proc_entry("txstats", 0444, NULL);
	if (proc_entry_txstats == NULL) {
		hello_exit();
		return -ENOMEM;
	} 
	else {
		proc_entry_txstats->read_proc = txstats_read;
		proc_entry_txstats->write_proc = NULL;
		printk(KERN_ALERT "ash: txstats proc entry created\n");
	}

	proc_entry_distance = create_proc_entry("distance", 0444, NULL);
	if (proc_entry_distance == NULL) {
		hello_exit();
		return -ENOMEM;
	} 
	else {
		proc_entry_distance->read_proc = distance_read;
		proc_entry_distance->write_proc = NULL;
		printk(KERN_ALERT "ash: distance proc entry created\n");
	}

	global_transmitted_count = 0;

	printk(KERN_ALERT "Hello, world. the current process is: %s pid is : %d module param is: %s NEW_KERNEL : %d MYMANET_ALPHA is : %d MYMANET_BETA : %d: MYMANET_STORE_PATH : %d\n", current->comm, current->pid, device_name, NEW_KERNEL, mymanet_alpha, mymanet_beta, MYMANET_STORE_PATH);

	initialize_session_timer();

	if(strncmp(device_name, "", strlen(device_name)) == 0){
		printk(KERN_ALERT"\nDevice Name Not specified. Cannot load module. Try again.\n");
		hello_exit();
		return 0;
	}
#if NEW_KERNEL
	d = dev_get_by_name(&init_net, device_name);
#else
	d = dev_get_by_name(device_name);
#endif
	if(d != NULL)
		printk(KERN_ALERT "Yeah !! name of device is %s  irq number is : %d flags %d type %d", d->name, 
				d->irq, d->flags, d->type);
	else
		hello_exit();

	#if MYMANET_KERNEL_VERSION_6_30_PLUS
		device_hard_start_xmit_backup = d->netdev_ops->ndo_start_xmit;
		memcpy(&new_netdev_ops, d->netdev_ops, sizeof(struct net_device_ops));
		new_netdev_ops_ptr->ndo_start_xmit = wdl_hard_start_xmit;
		memcpy((void *)d->netdev_ops, &new_netdev_ops, sizeof(struct net_device_ops));
	#else
		device_hard_start_xmit_backup = d->hard_start_xmit;
		d->hard_start_xmit = wdl_hard_start_xmit;
	#endif

	return 0;
}

module_init(hello_init);
module_exit(hello_exit);

