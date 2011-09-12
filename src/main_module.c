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

#ifndef MYMANET_HEADER_ID
#define MYMANET_HEADER_ID 0x3333
#endif

#include<linux/kernel.h>
#include<linux/kmod.h>
#include<linux/version.h>
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

#include "platform_dep_flags.h"
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

#define SET_SKB_FIELD(skb, object, size) 	\
	memcpy(skb_push(skb, size), object, size);

module_param(device_name, charp, S_IRUGO);	/*Device name can be given as a parameter to the module*/
module_param(mymanet_alpha, int, 0);	/*Device name can be given as a parameter to the module*/
module_param(mymanet_beta, int, 0);	/*Device name can be given as a parameter to the module*/

struct mymanet_header{
	uint8_t session_id;
	uint8_t hops_remaining;
	uint8_t distance;
	uint8_t orig_distance;
	uint32_t timestamp;
	uint32_t timestamp_frac;
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
	uint32_t sec_holder = 0;

	uint16_t eth_type_manifold = MYMANET_HEADER_ID;

	orig_header_room = (int)(skb->data - skb->head);
	if(orig_header_room < (ETHERNET_HEADER_SIZE + MANIFOLD_HEADER_SIZE)){
		printk(KERN_ALERT"ERROR : Insufficient headroom for constructing Manifold Packet\n");
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

	SET_SKB_FIELD(skb, &mymanethdr, MANIFOLD_HEADER_SIZE);
	SET_SKB_FIELD(skb, &eth_type_manifold, 2);
	SET_SKB_FIELD(skb, src_mac, 6);
	SET_SKB_FIELD(skb, dest_mac, 6);

	add_or_update_stat_entry(mymanethdr.final_destination, 2, mymanethdr.session_id, mymanethdr.final_destination);

#if DEBUG
	printk(KERN_ALERT "MODIFIED and TRANSMITTED pkt from  %x:%x:%x:%x:%x:%x to %x:%x:%x:%x:%x:%x type %x:%x",
			src_mac[0], src_mac[1], src_mac[2], src_mac[3], src_mac[4], src_mac[5],
			dest_mac[0], dest_mac[1], dest_mac[2], dest_mac[3], dest_mac[4], dest_mac[5],
			eth_type_manifold[0], eth_type_manifold[1]);
#endif

	if(netdev->flags & IFF_UP)
	{
		return dev_queue_xmit(skb);
	}
	else{
#if DEBUG
		printk(KERN_ALERT "INFO: Device is busy.\n");
#endif
		return NETDEV_TX_BUSY;
	}
}

uint8_t difftime(const uint32_t *old, const uint32_t *old_frac, const uint32_t *new, const uint32_t *new_frac)
{

	if(*new > *old)
		return NEW;
	else if(*new == *old){
		if(*new_frac > *old_frac)
			return NEW;
		else if(*new_frac == *old_frac)
			return DUPLICATE;
		else
			return OUT_OF_ORDER;
	}
	else
	{
		if((*old - *new) > 1)
			return OLD;
		else
		{
			if((1000000 - *new_frac + *old_frac)> OUT_OF_ORDER_WINDOW)
				return OLD;
			else
				return OUT_OF_ORDER;
		}
	}
}

uint8_t routing_decision(uint8_t distance_from_me, uint8_t distance_from_orig_sender, uint8_t packet_distance, uint32_t old_timestamp, uint32_t old_timestamp_frac, uint32_t new_timestamp, uint32_t new_timestamp_frac)
{
#define DROP 0
#define FORWARD 1

	if(distance_from_me < (packet_distance + mymanet_alpha)){
		if(difftime(&old_timestamp, &old_timestamp_frac, &new_timestamp, &new_timestamp_frac) == OLD){ 	/*This is an old packet, Drop it.*/
			return DROP;
		}
		else if(difftime(&old_timestamp, &old_timestamp_frac, &new_timestamp, &new_timestamp_frac) == DUPLICATE){
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
		else if(difftime(&old_timestamp, &old_timestamp_frac, &new_timestamp, &new_timestamp_frac) == NEW){
			return FORWARD;
		}
	}
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
	uint32_t old_timestamp_frac;

	int orig_header_room=0;

	//If the packet cannot be prcessed due to some key fields being null, free the socket buffer and return
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

	/*Since we are at the start of the payload after Eth header, go back and fetch the info.*/
	memcpy(dest_mac, ( (skb->data - (2 * 6)) - 2 ), 6);
	memcpy(src_mac, ( (skb->data - 6) - 2 ), 6);
	memcpy(eth_type, ( skb->data - 2 ), 2);

	memcpy(&mymanethdr, skb->data, MANIFOLD_HEADER_SIZE);
	sec_holder = ntohl(mymanethdr.timestamp);
	mymanethdr.timestamp = sec_holder;

	/*Do skb_pull of MANIFOLD_HEADER_SIZE*/
	skb_pull(skb, MANIFOLD_HEADER_SIZE);
	/*End: Extraction of Manifold Header fields ends here*/

	/*eth_type_trans will understand only the original eth_type numbers*/
	/*Build a normal ethernet layer packet header so that eth_type_trans can synthesize it.*/
	SET_SKB_FIELD(skb, mymanethdr.eth_type_original, 2);
	SET_SKB_FIELD(skb, mymanethdr.original_source, 6);
	SET_SKB_FIELD(skb, mymanethdr.final_destination, 6);

	skb->pkt_type = PACKET_HOST; /* Assumed. eth_type_trans will overwrite it anyways.*/
	/*Remember that eth_type_trans function does skb_pull of length ETH_HLEN.
	 * So the newly created Eth header will be stripped off
	 * */
	skb->protocol = eth_type_trans(skb, netdev);

	/*Start of Actual Processing*/

	/*
	 * No Need to do SKB expansion since this is a Manifold packet which has already been expanded
	 * to have sufficient head-room
	*/

	if (skb->pkt_type == PACKET_HOST) {

		/*Add or Update this MAC entry to our Stat List*/
		add_or_update_stat_entry(mymanethdr.original_source, 0, mymanethdr.session_id, mymanethdr.final_destination);

#if MYMANET_STORE_PATH

#if IS_EMB_DEV
#else
		add_or_update_path_entry(mymanethdr.original_source, mymanethdr.hop1_mac, mymanethdr.hop2_mac, mymanethdr.hop3_mac, mymanethdr.session_id);
#endif
#endif
		netif_rx(skb);
		return 0;
	}
	if ((skb->pkt_type == PACKET_BROADCAST) || (skb->pkt_type == PACKET_OTHERHOST)) {
		/*We need to forward such packets after making some changes i.e. broadcast retransmit*/

		/*Get my distance from this final destination*/
		distance_from_me = search_for_distance(mymanethdr.final_destination);

		/*Initialize old timestamp*/
		old_timestamp = 0;
		old_timestamp_frac = 0;

		/*Get the timestamp of this final destination*/
		old_timestamp = search_for_timestamp(mymanethdr.original_source);
		old_timestamp_frac = search_for_timestamp_frac(mymanethdr.original_source);
		if((old_timestamp == 0)){
			/*This mac entry does not exist. Add it.*/
			add_or_update_timestamp_entry(mymanethdr.original_source, mymanethdr.timestamp, mymanethdr.timestamp_frac);
		}
		else{
			uint8_t  verdict =  difftime(&old_timestamp, &old_timestamp_frac, &mymanethdr.timestamp, &mymanethdr.timestamp_frac);
			if(verdict == NEW || verdict == OUT_OF_ORDER){
				/*This is a NEW packet from an existing mac. Update the entry*/
				add_or_update_timestamp_entry(mymanethdr.original_source, mymanethdr.timestamp, mymanethdr.timestamp_frac);
			}
			else{
				/*This is neither a new mac entry, nor a new packet from an existing mac entry.
				  Possibly a duplicate packet. Do not update our timestamps.*/
			}
		}

		/*Reduce the Number of Remaining Hops for this packet*/
		mymanethdr.hops_remaining = mymanethdr.hops_remaining - 1;

		if(!(global_fwd_disable) && (!global_manifold_disable) && (distance_from_me != 0)
			&& (mymanethdr.hops_remaining != 0)
			&& (memcmp(mymanethdr.final_destination, netdev->dev_addr, 6) != 0)) {
			if(routing_decision(distance_from_me, mymanethdr.orig_distance,	mymanethdr.distance,
					    old_timestamp, old_timestamp_frac, mymanethdr.timestamp,
					    mymanethdr.timestamp_frac) || (distance_from_me == 0xFF)) {

				if(skb->pkt_type == PACKET_BROADCAST){

					if(bcast_fwd_threshold > 0)
					  p_fwd_deno = bcast_fwd_threshold;
					else
					  p_fwd_deno = 1;

					rand_value = random32()%INT_MAX;
					if(rand_value < (unsigned int)(1*(float)(INT_MAX))/(p_fwd_deno)) {
						mymanethdr.distance = distance_from_me;
						wdl_modify_and_transmit((skb_copy(skb, GFP_ATOMIC)), netdev,
									g_broadcast_mac, netdev->dev_addr, mymanethdr);
					}
				}
				else {
					if(bcast_fwd_threshold < 3) {
						p_fwd_deno = 2;
					}
					else {
						p_fwd_deno = bcast_fwd_threshold;
					}
					rand_value = random32() % INT_MAX;
					if(rand_value < (unsigned int)(2*(float)(INT_MAX))/p_fwd_deno) {
						mymanethdr.distance = distance_from_me;
						wdl_modify_and_transmit((skb_copy(skb, GFP_ATOMIC)), netdev,
									g_broadcast_mac, netdev->dev_addr, mymanethdr);
					}
				}
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
		/*Add or Update this MAC entry to our Stat List*/
		add_or_update_stat_entry(mymanethdr.original_source, 0, mymanethdr.session_id, mymanethdr.final_destination);

#if MYMANET_STORE_PATH

#if IS_EMB_DEV
#else
		add_or_update_path_entry(mymanethdr.original_source, mymanethdr.hop1_mac, mymanethdr.hop2_mac, mymanethdr.hop3_mac, mymanethdr.session_id);
#endif
#endif

		netif_rx(skb);
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
	int i = 0;
	int ret = 0;
	//unsigned int rand_value;
	int prob_fwd;

#if MYMANET_STORE_PATH
	uint8_t bcast_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif

	eth_type_manifold[0]=0x33;
	eth_type_manifold[1]=0x33;

	if((device_hard_start_xmit_backup) && netdev){

		memcpy(mymanethdr.final_destination, skb_original->data, 6);
		memcpy(mymanethdr.original_source, skb_original->data + 6, 6);
		memcpy(mymanethdr.eth_type_original, skb_original->data + 12, 2);

		/*CHECK whether this is a Manifold packet
		 *coming from the reception path */
		if((mymanethdr.eth_type_original[0] == eth_type_manifold[0]) && (mymanethdr.eth_type_original[1] == eth_type_manifold[1])){

			/*
			 * This is a Manifold packet coming from our Reception Path.
			 * Dont touch this packet. Just send it as it is.
			 *
			*/

			/*Actual Transmission happens here*/
			if(netdev->flags & IFF_UP)
			{
				ret = (*device_hard_start_xmit_backup)(skb_original, netdev);
				return ret;
			}
			else{
				printk(KERN_ALERT "INFO: Device was busy. \n\n");
				return NETDEV_TX_BUSY;
			}

		}
		else{

			/*This is NOT a Manifold Packet.
			 * Convert it into a Manifold Packet.
			 * */

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
			sec_holder = htonl(temp_time.tv_usec);
			mymanethdr.timestamp_frac = sec_holder;
#if MYMANET_STORE_PATH
			memcpy(mymanethdr.hop1_mac, bcast_mac, 6);
			memcpy(mymanethdr.hop2_mac, bcast_mac, 6);
			memcpy(mymanethdr.hop3_mac, bcast_mac, 6);
#endif

			memcpy(skb_push(skb, MANIFOLD_HEADER_SIZE), &mymanethdr, MANIFOLD_HEADER_SIZE);

			/*Spoof our own address as source address and Dest MAC = Broadcast*/
			SET_SKB_FIELD(skb, eth_type_manifold, 2 );
			SET_SKB_FIELD(skb, netdev->dev_addr, 6);
			SET_SKB_FIELD(skb, g_broadcast_mac, 6);

			new_header_room = (int)(skb->data - skb->head);

			kfree_skb(skb_original);

			/*Actual Transmission happens here*/
			if(netdev->flags & IFF_UP)
			{

				prob_fwd = ((mymanethdr.distance) * RTX_CNT)/MAX_USABLE_VD;

				if(compare_mac_address(mymanethdr.final_destination, g_broadcast_mac)==0)
				{
					if(mymanethdr.distance != 255 && mymanethdr.distance!=0)
					{
						//@HrushiM, please add a max cap for prob_fwd.
						if(prob_fwd > 2)
						{
							for(i = 0; i < prob_fwd && prob_fwd >= 0; i++)
							{
								//Directly calling hard_start_xmit fails for some kernels
								dev_queue_xmit(skb_copy(skb, GFP_ATOMIC));
							}
						}
						else
						{
							//@hrushi: this part needs a comment.
							dev_queue_xmit(skb_copy(skb, GFP_ATOMIC));
							dev_queue_xmit(skb_copy(skb, GFP_ATOMIC)); /*This is intentional*/
						}
					}
					else
					{
						//@hrushi: this part needs a comment.
						dev_queue_xmit(skb_copy(skb, GFP_ATOMIC));
						dev_queue_xmit(skb_copy(skb, GFP_ATOMIC)); /*This is intentional*/
					}
				}

				//@hrushim: Why do we use device_hard_start_xmit_backup here instead of dev_queue_xmit.
				//this part needs comment.
				return (*device_hard_start_xmit_backup)(skb, netdev);
				//return dev_queue_xmit(skb);
			}
			else{
				printk(KERN_ALERT "\n\nINFO: Device was busy.\n\n");
				return NETDEV_TX_BUSY;
			}
		}
	}
	kfree_skb(skb_original);
	return 0;
}


/*Basic Module specific code starts here*/

/*
 * We need to re-instates the hard_start_xmit function to the lower level function,
 * delete the proc entry, and remove the new registered packet type when
 * the module is unloaded.
*/
static struct net_device_ops *original_netdev_ops;
static void hello_exit(void)
{
	mod_timer(&g_session_timer, jiffies + 1);
	del_timer_sync(&g_session_timer);

	if((device_hard_start_xmit_backup) && (d)) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
		/*static const struct net_device_ops original_netdev_ops = {
			.ndo_open               = lifenet_device_open,
			.ndo_stop               = lifenet_device_stop,
			.ndo_get_stats		= lifenet_device_get_stats,
			.ndo_do_ioctl	 	= lifenet_device_do_ioctl,
			.ndo_start_xmit         = lifenet_device_hard_start_xmit,
		};*/
		d->netdev_ops = original_netdev_ops;
		printk(KERN_ALERT "INFO: net device struct is %p. \n\n", d->netdev_ops);
#else
		d->hard_start_xmit = device_hard_start_xmit_backup;
#endif
	}

	if(d)
		dev_put(d);

	dev_remove_pack(&manifold_ptype);

	remove_proc_entry("wdl", NULL);
	remove_proc_entry("rxstats", NULL);
	remove_proc_entry("txstats", NULL);

	free_entire_distance_list();
	free_entire_stat_list();

	printk(KERN_ALERT "\nINFO: LifeNet has been disabled.\n");
}


int lifenet_device_open(struct net_device *netdev){
	if(!device_open_backup){
		printk(KERN_ALERT "\n device_open_backup: not implemented.\n");
		return 0;
	}
	return ((*device_open_backup)(netdev));
}

int lifenet_device_stop(struct net_device *netdev){
	if(!device_stop_backup){
		printk(KERN_ALERT "\n device_stop_backup: not implemented.\n");
		return 0;
	}
	return (*device_stop_backup)(netdev);
}

int lifenet_device_hard_start_xmit(struct sk_buff *skb, struct net_device *netdev){
	if(!device_hard_start_xmit_backup){
		printk(KERN_ALERT "\n device_hard_start_xmit_backup: not implemented.\n");
		return 0;
	}
	return ((*device_hard_start_xmit_backup)(skb, netdev));
}

void lifenet_device_tx_timeout(struct net_device *netdev){
	if(!device_ndo_tx_timeout_backup){
		printk(KERN_ALERT "\n device_ndo_tx_timeout_backup: not implemented.\n");
		return;
	}
	((*device_ndo_tx_timeout_backup)(netdev));
}

void lifenet_device_multicast_list(struct net_device *netdev){
	if(!device_ndo_set_multicast_list_backup){
		printk(KERN_ALERT "\n device_ndo_set_multicast_list_backup: not implemented.\n");
		return;
	}
	(*device_ndo_set_multicast_list_backup)(netdev);
}

int lifenet_device_do_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd){
	if(!device_do_ioctl_backup){
		printk(KERN_ALERT "\n device_do_ioctl_backup: not implemented.\n");
		return 0;
	}
	return ((*device_do_ioctl_backup)(dev, ifr, cmd));
}

struct net_device_stats dummy_stats;
struct net_device_stats * lifenet_device_get_stats(struct net_device *netdev){
	if(!device_ndo_get_stats_backup){
		printk(KERN_ALERT "\n device_ndo_get_stats_backup: not implemented.\n");
		memset(&dummy_stats, 0, sizeof(dummy_stats));
		return &dummy_stats;
	}
	return ((*device_ndo_get_stats_backup)(netdev));
}

int lifenet_device_set_mac_address(struct net_device *netdev, void *p){
	if(!device_set_mac_address_backup){
		printk(KERN_ALERT "\n device_set_mac_address_backup: not implemented.\n");
		return 0;
	}
	return ((*device_set_mac_address_backup)(netdev, p));
}

int lifenet_device_change_mtu(struct net_device *netdev, int new_mtu){
	if(!device_ndo_change_mtu_backup){
		printk(KERN_ALERT "\n device_ndo_change_mtu_backup: not implemented.\n");
		return 0;
	}
	return ((*device_ndo_change_mtu_backup)(netdev, new_mtu));
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
static struct net_device_ops lifenet_netdev_ops = {
	.ndo_open               = lifenet_device_open,
	.ndo_stop               = lifenet_device_stop,
	.ndo_get_stats		= lifenet_device_get_stats,
	.ndo_do_ioctl	 	= NULL,
	.ndo_start_xmit         = lifenet_device_hard_start_xmit,	/*
									 * replace this with
									 * wdl_hard_start_xmit once the
									 * kernel panics are resolved.
									 */
};
#endif

/*
* We need to be in-line for the
* reception and transmission of every packet.
*/
static int hello_init(void)
{

	struct proc_dir_entry *proc_entry, *proc_entry_rxstats, *proc_entry_txstats, *proc_entry_distance;

	dev_add_pack(&manifold_ptype);

	proc_entry = create_proc_entry("wdl", 0444, NULL);
	if (proc_entry == NULL) {
		hello_exit();
		return -ENOMEM;
	} else {
		proc_entry->read_proc = wdl_read;
		proc_entry->write_proc = wdl_write;
		printk(KERN_ALERT "INFO: proc entry created\n");
	}

	proc_entry_rxstats = create_proc_entry("rxstats", 0444, NULL);
	if (proc_entry_rxstats == NULL) {
		hello_exit();
		return -ENOMEM;
	} else {
		proc_entry_rxstats->read_proc = rxstats_read;
		proc_entry_rxstats->write_proc = NULL;
		printk(KERN_ALERT "INFO: rxstats proc entry created\n");
	}

	proc_entry_txstats = create_proc_entry("txstats", 0444, NULL);
	if (proc_entry_txstats == NULL) {
		hello_exit();
		return -ENOMEM;
	} else {
		proc_entry_txstats->read_proc = txstats_read;
		proc_entry_txstats->write_proc = NULL;
		printk(KERN_ALERT "INFO: txstats proc entry created\n");
	}

	proc_entry_distance = create_proc_entry("distance", 0444, NULL);
	if (proc_entry_distance == NULL) {
		hello_exit();
		return -ENOMEM;
	} else {
		proc_entry_distance->read_proc = distance_read;
		proc_entry_distance->write_proc = NULL;
		printk(KERN_ALERT "ash: distance proc entry created\n");
	}

	printk(KERN_ALERT "INFO: LifeNet has been enabled.	\
		Current process is: %s pid is : %d module param is: %s \
		NEW_KERNEL : %d MYMANET_ALPHA is : %d MYMANET_BETA : %d:	\
		MYMANET_STORE_PATH : %d: MANIFOLD_HEADER_SIZE : %d \n",
		current->comm, current->pid, device_name, NEW_KERNEL,
		mymanet_alpha, mymanet_beta, MYMANET_STORE_PATH,
		MANIFOLD_HEADER_SIZE);

	initialize_session_timer();

	if(strlen(device_name) == 0){
		printk(KERN_ALERT"\nERROR: Device Name Not specified. Cannot load module. Try again.\n");
		hello_exit();
		return 0;
	}
#if NEW_KERNEL
	d = dev_get_by_name(&init_net, device_name);
#else
	d = dev_get_by_name(device_name);
#endif
	if(d){
		printk(KERN_ALERT "INFO: Name of device is %s  irq number is : %d flags %d type %d", d->name, 
				d->irq, d->flags, d->type);
	} else {
		hello_exit();
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
	if(d && d->netdev_ops && d->netdev_ops->ndo_start_xmit) { 	/*
									* Required fields.
								   	* Bail out if ndo_start_xmit is NULL
									*/
		if(!d->netdev_ops->ndo_open)
			printk(KERN_ALERT "ndo_open is NULL");
		else
			device_open_backup = d->netdev_ops->ndo_open;

		if(!d->netdev_ops->ndo_stop)
			printk(KERN_ALERT "ndo_stop is NULL");
		else
			device_stop_backup = d->netdev_ops->ndo_stop;

		if(!d->netdev_ops->ndo_start_xmit)
			printk(KERN_ALERT "ndo_start_xmit is NULL");
		else
			device_hard_start_xmit_backup = d->netdev_ops->ndo_start_xmit;

		if(!d->netdev_ops->ndo_do_ioctl)
			printk(KERN_ALERT "ndo_do_ioctl is NULL");
		else
			device_do_ioctl_backup = d->netdev_ops->ndo_do_ioctl;

		if(!d->netdev_ops->ndo_get_stats)
			printk(KERN_ALERT "ndo_get_stats is NULL");
		else
			device_ndo_get_stats_backup = d->netdev_ops->ndo_get_stats;

		/*
		 * We need to re-instate netdev_ops
		 * when module is unloaded.
		 */
		printk(KERN_ALERT "INFO: net device struct is %p. \n\n", d->netdev_ops);
		original_netdev_ops = d->netdev_ops;

		/*
		 * Overrding w/ LifeNet functions
		 */
		d->netdev_ops = &lifenet_netdev_ops;

	} else {
		printk(KERN_ALERT "ERROR: net device struct is corrupt. \n\n");
	}
#else
	device_hard_start_xmit_backup = d->hard_start_xmit;
	d->hard_start_xmit = wdl_hard_start_xmit;
#endif
	return 0;
}

module_init(hello_init);
module_exit(hello_exit);

