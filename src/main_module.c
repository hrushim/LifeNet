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
Developers: Hrushikesh Mehendale, Ashwin Paranjpe, Santosh Vempala
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

//
// Kernel Module Parameters
//

module_param(device_name, charp, S_IRUGO);	// Name of the wireless device interface e.g. wlan0 or ath0, etc.
module_param(mymanet_alpha, int, 0);		// Effective Distance difference threshold used for making routing decisions
module_param(mymanet_beta, int, 0);	        // Again used for making routing decisions


//
// Mandatory header fields
//
//  -----------------------------------
// | SESSID |  TTL   |  DIST  | ODIST  |
//  -----------------------------------
// |            TIMESTAMP              |
//  -----------------------------------
// |   TIMESTAMP FRACTIONAL PART       |
//  -----------------------------------
// |  OSRC  |  DEST  |  ETH   |
//  -------------------------- 
//
// Additional header fields if path tracing is enabled (works for TTL = 4)
//
//  -----------------------------------
// |  MAC of 1st forwarder node        |
//  -----------------------------------
// |                 |                 |
//  -----------------------------------
// |  MAC of 2nd forwarder node        |
//  -----------------------------------
// |  MAC of 3rd forwarded node        |
//  -----------------------------------
// |                 |
//  -----------------
//
//

struct mymanet_header{
	uint8_t session_id;		// Network statistics are refreshed after session elapses
	uint8_t hops_remaining;		// Used to bound the maximum path length
	uint8_t distance;		// Current packet distance (updated by the transmitter or forwarder node)
	uint8_t orig_distance;		// Distance of the destination from the original transmitter (one-time write only)
	uint32_t timestamp;		// Serves as a sequence number
	uint32_t timestamp_frac;	//
	uint8_t original_source[6];	// This and the next two fields are used for reconstructing the ethernet header when the packet is received
	uint8_t final_destination[6];
	uint8_t eth_type_original[2];
#if MYMANET_STORE_PATH
	uint8_t hop1_mac[6];
	uint8_t hop2_mac[6];
	uint8_t hop3_mac[6];
#endif
};

uint8_t bcast_fwd_threshold = 2;

// This is not a standalone function and shall be called from wdl_handle_receive
// function ONLY when there is a need to change the MAC header of the packet.
// This function simply makes the necessary changes in the packet and then 
// calls the wdl_hard_start_xmit function which will then do the actual work 
// of transmission. Currently it retransmits the broadcast packet by spoofing
// its own MAC as the source MAC

int wdl_modify_and_transmit(struct sk_buff *skb, struct net_device *netdev, uint8_t dest_mac[6], uint8_t src_mac[6], struct mymanet_header mymanethdr)
{

	int orig_header_room=0;
	uint32_t sec_holder;
	int i = 0;
	int prob_fwd;

	uint8_t eth_type_manifold[2];
	eth_type_manifold[0] = 0x33;
	eth_type_manifold[1] = 0x33;


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

	memcpy(skb_push(skb, MANIFOLD_HEADER_SIZE), &mymanethdr, MANIFOLD_HEADER_SIZE);

	memcpy(skb_push(skb, 2), eth_type_manifold, 2);
	memcpy(skb_push(skb, 6), src_mac, 6);
	memcpy(skb_push(skb, 6), dest_mac, 6);

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
		printk(KERN_ALERT "\n\nDevice was DOWN !! \n\n");
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

//
// This function is used to decide whether a received packet, which is destined to some other host can be forwarded or not. 
// The logic used for forwarding is very simple. A node should forward a packet only if it increases the chance of the 
// packet reaching its destination. If the packet distance is considerably less than the distance of the destination from the 
// node making the forwarding decision, it means that the packet's previous hop host was actually closer to the destination
// than the current node making the forwarding decision. In this case, the packet is dropped. 
// A second stricter check involves comparing the distance of the node making the forwarding decision with that of the original
// source (present in the packet).
//

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
	else{ 
		return DROP;
	}
	printk(KERN_ALERT "\nThis shouldn't be ever printed. \n");
	return DROP;
}

//
// This is the primary receive function for the newly defined packet type. When we arrive at this function, 
// the skb->data points to the start of the payload of the packet after the MAC header (i.e. it points to the 
// start of the mymanethdr. The received packet is processed as follows:
//
// 1.	Do initial sanity checks on the packet and make sure it can be processed. If any check fails, free the buffer 
//	and return.
//
// 2.	Go back and fetch the ethernet header. Create a mymanethdr object and copy the mymanethdr from the packet into
//	the newly created object.
//
// 3.	Using eth_type_trans function classify the packet as - (1) Broadcast packet (2) Host Packet and (3) Other host 
//	packet. There is a catch here, eth_type_trans function understands packets only in the standard format. Hence
//	before giving the packet to eth_type_trans we have to reconstruct the original packet without the mymanethdr. 
//	Remember that we had stored the original Ethernet header into the mymanethdr in wdl_hard_start_xmit(). It was 
//	precisely for this reason. Original packet is reconstructed as follows - Suck out the mymanethdr object, 
//	using that mymanethdr, recreate the original Ethernet header and push it back into the packet so that it goes 
//	and sits just after that network/IP header. Then feed the socket buffer to eth_type_trans for processing, which
//	then classifies the packet as mentioned above. It is important to remember that after synthesizing the packet, 
//	the eth_type_trans function will strip off the newly created Ethernet header. 
//
// 4.	If the packet is a host packet, then update necessary reception statistics and simply queue the packet for the 
//	higher layers to process. 
//
// 5.	If the packet belongs to a different host, it has to be forwarded. Update the reception statistics, distance in 
//	the mymanethdr. Also decrement the number of hops by one. Then check if the routing_decision function decides 
//	to forward the packet. If yes, then submit the packet for forwarding to the wdl_modify_and_transmit function with 
//	a probability. This probability is used to mitigate boradcast storms in dense clusters. Every node maintains a 
//	notion of a cluster around it, which is essentially the count of nodes that are at an effective distance less than
//	a threshold distance. We assume that then when a node in a cluster receives a packet, most of the other nodes in 
//	the cluster would have received it as well. The probability ensures that only a few nodes out of the cluster actually
//	participate in forwarding. 
//
// 6.	If the packet is a broadcast packet, then it has to be forwarded. The algorithm is similar to a packet, which belongs
//	to any other host. The only difference is, every broadcast packet is also queued for the higher layers to process.
//
//
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

	//Since we are at the start of the payload after Eth header, go back and fetch the info.
	memcpy(dest_mac, ( (skb->data - (2 * 6)) - 2 ), 6);
	memcpy(src_mac, ( (skb->data - 6) - 2 ), 6);
	memcpy(eth_type, ( skb->data - 2 ), 2);

	memcpy(&mymanethdr, skb->data, MANIFOLD_HEADER_SIZE);
	sec_holder = ntohl(mymanethdr.timestamp);
	mymanethdr.timestamp = sec_holder;
	


	// printk(KERN_ALERT"    \nReceived Session ID as %d\n", mymanethdr.session_id);
	// printk(KERN_ALERT"    Received Hops Remaining as %d\n", mymanethdr.hops_remaining);
	// printk(KERN_ALERT"    Received Distance as %d\n", mymanethdr.distance);
	// printk(KERN_ALERT"    Received Original Distance as %d\n", mymanethdr.orig_distance);
	// printk(KERN_ALERT"    Received Timestamp as %ld\n", mymanethdr.timestamp);
	// printk(KERN_ALERT"    Received Original Source MAC as %x:%x:%x:%x:%x:%x\n", mymanethdr.original_source[0], mymanethdr.original_source[1], mymanethdr.original_source[2], mymanethdr.original_source[3], mymanethdr.original_source[4], mymanethdr.original_source[5]);
	// printk(KERN_ALERT"    Received Final Dest MAC as %x:%x:%x:%x:%x:%x\n", mymanethdr.final_destination[0], mymanethdr.final_destination[1], mymanethdr.final_destination[2], mymanethdr.final_destination[3], mymanethdr.final_destination[4], mymanethdr.final_destination[5]);
	// printk(KERN_ALERT"    Received Original Type as %x:%x\n", mymanethdr.eth_type_original[0], mymanethdr.eth_type_original[1]);



	// Do skb_pull of MANIFOLD_HEADER_SIZE
	skb_pull(skb, MANIFOLD_HEADER_SIZE);
	// End: Extraction of Manifold Header fields ends here

	// eth_type_trans will understand only the original eth_type numbers
	// Build a normal ethernet layer packet header so that eth_type_trans can synthesize it.
	memcpy(skb_push(skb, 2), mymanethdr.eth_type_original, 2 );
	memcpy(skb_push(skb, 6), mymanethdr.original_source, 6);
	memcpy(skb_push(skb, 6), mymanethdr.final_destination, 6);

	skb->pkt_type = PACKET_HOST; // Assumed. eth_type_trans will overwrite it anyways.
	// Remember that eth_type_trans function does skb_pull of length ETH_HLEN. 
	// So the newly created Eth header will be stripped off

	skb->protocol = eth_type_trans(skb, netdev);

	// Start of Actual Processing
	// No Need to do SKB expansion since this is a Manifold packet which has already been expanded
	// to have sufficient head-room

	if (skb->pkt_type == PACKET_HOST) {

		// Add or Update this MAC entry to our Stat List
		add_or_update_stat_entry(mymanethdr.original_source, 0, mymanethdr.session_id, mymanethdr.final_destination);

#if MYMANET_STORE_PATH

		#if IS_EMB_DEV
		#else
			add_or_update_path_entry(mymanethdr.original_source, mymanethdr.hop1_mac, mymanethdr.hop2_mac, mymanethdr.hop3_mac, mymanethdr.session_id);
		#endif		
#endif

		// Initialize old timestamp
		old_timestamp = 0;
		old_timestamp_frac = 0;

		// Get the timestamp of this final destination
		old_timestamp = search_for_timestamp(mymanethdr.original_source);
		old_timestamp_frac = search_for_timestamp_frac(mymanethdr.original_source);

		netif_rx(skb);
		return 0;
	}
	if ((skb->pkt_type == PACKET_BROADCAST) || (skb->pkt_type == PACKET_OTHERHOST)) {
		// We need to forward such packets after making some changes i.e. broadcast retransmit

		// Get my distance from this final destination
		distance_from_me = search_for_distance(mymanethdr.final_destination);

		// Initialize old timestamp
		old_timestamp = 0;
		old_timestamp_frac = 0;

		// Get the timestamp of this final destination
		old_timestamp = search_for_timestamp(mymanethdr.original_source);
		old_timestamp_frac = search_for_timestamp_frac(mymanethdr.original_source);
		if((old_timestamp == 0)){
			// This mac entry does not exist. Add it.
			add_or_update_timestamp_entry(mymanethdr.original_source, mymanethdr.timestamp, mymanethdr.timestamp_frac);
		}
		else if(difftime(&old_timestamp, &old_timestamp_frac, &mymanethdr.timestamp, &mymanethdr.timestamp_frac)==NEW || difftime(&old_timestamp, &old_timestamp_frac, &mymanethdr.timestamp, &mymanethdr.timestamp_frac)==OUT_OF_ORDER){		
			// This is a NEW packet from an existing mac. Update the entry
			add_or_update_timestamp_entry(mymanethdr.original_source, mymanethdr.timestamp, mymanethdr.timestamp_frac);
		}
		else{
			// This is neither a new mac entry, nor a new packet from an existing mac entry. 
			// Possibly a duplicate packet. Do not update our timestamps.
		}

		// Reduce the Number of Remaining Hops for this packet
		mymanethdr.hops_remaining = mymanethdr.hops_remaining - 1;

		if(!(global_fwd_disable) && (!global_manifold_disable) && (distance_from_me != 0) && (mymanethdr.hops_remaining != 0)
				&& (memcmp(mymanethdr.final_destination, netdev->dev_addr, 6) != 0)) { 
			if(routing_decision(distance_from_me, mymanethdr.orig_distance, mymanethdr.distance, old_timestamp, old_timestamp_frac, mymanethdr.timestamp, mymanethdr.timestamp_frac) || 
				(distance_from_me == 0xFF)) {

				if(skb->pkt_type == PACKET_BROADCAST){

					if(bcast_fwd_threshold > 0)
					  p_fwd_deno = bcast_fwd_threshold;
					else
					  p_fwd_deno = 1;
					rand_value = random32()%INT_MAX;
					if(rand_value < (unsigned int)(1*(float)(INT_MAX))/(p_fwd_deno)) {
						mymanethdr.distance = distance_from_me;
						wdl_modify_and_transmit((skb_copy(skb, GFP_ATOMIC)), netdev, g_broadcast_mac, netdev->dev_addr, mymanethdr);
					}
				}
				else {
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
				}
			}
		}

		if(skb->pkt_type == PACKET_BROADCAST){
			flag = 1;
			// Add or Update this MAC entry to our Stat List
			add_or_update_stat_entry(mymanethdr.original_source, 0, mymanethdr.session_id, mymanethdr.final_destination);
			netif_rx(skb); 
			// This is originally a Broadcast packet. For E.g. Heart-Beats
		}
		if((flag != 1) && (skb != NULL)){
			kfree_skb(skb);
		}

		return 0;
	}
	else{	// Packet is neither broadcast nor unicast. In the case of MyMANET, it appears to be be a garbled packet. Free the skb.
		// Add or Update this MAC entry to our Stat List
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


//
// This function is called by the network layer for packet transmission after the module is loaded. 
// The mechanism of how this function gets called is explained before the hello_init() function.
//
// skb_original points to the start of the ethernet header of the packet. The flow of this function
// is as follows:
//
// ->	Create a MyMANET header object - mymanethdr
//
// ->	Copy the Ethernet header fields from the packet into the relevant fields in mymanethdr. This is 
//	necessary to reconstruct the original Ethernt header after the packet has been received by some node.
//
// ->	This function is called in two scenarios - (1) during transmission and (2) during forwarding
//
// ->	If it is coming from the forwarding channel, do no change it. Directly call the device
//	hard_start_xmit function (remember we had backed it up in the hello_init() function), which
//	then actually transmits the packet and return.
//
// ->	If control comes here, the packet is coming from the transmission channel. Add more room 
//	between the network header and the MAC header so as to fit the mymanethdr. Size of the room
//	should be exactly equal to the size of mymanethdr. 
//
// ->	Suck out the Ethernet header. 
//
// ->	Update remaining fields in the mymanethdr that are necessary for routing such as effective distance, 
//	ttl or hop count, original source mac, etc. Push the mymanethdr into the packet so that it goes 
//	and sits just after the network/IP header
//
// ->	Change the destination MAC of the Ethernet header to broadcast mac FF:FF:FF:FF:FF:FF (to know why this 
//	is done, you would need to undersand how the routing works in LifeNet). Then change the 
//	packet type to [33][33]. Push back the Ethernet header into the packet so that it goes and sits
//	just after the mymanethdr.
//
// ->	Transmit the packet. Transmission involves a bit of manipulation. In LifeNet, every packet is always
//	transmitted to is broadcast MAC address. Unlike unicast packets, the MAC layer does not guarantee
//	successful packet delivery for broadcast packets. Usually after transmission of a unicast packet, the 
//	MAC layer waits for a MAC-level acknowledgement from the destination. If the waiting times out and the
//	ack still does not come, the MAC repeats the unicast transmission. The maximum retry limit is be default
//	set to 7 for most cards. This retry mechanism is heavily optimized at the MAC and guarantees successful
//	MAC level packet delivery for most packets. However, for broadcast packets things are different. 
//	There is no intended single destination for broadcast traffic. The MAC does not have a notion of 
//	a singular destination for broadcast traffic and hence it does not wait and check if the packet has been
//	delivered to the destination. It simply transmits the packet and returns the control to the caller. This
//	lack of guarantee of successful delivery at the MAC layer inherently induces some packet loss. To reduce
//	this inherent packet loss, we call the device hard_start_xmit function more than once for a single packet.
//	The logic is, by looking at the effective distance of the final destination, we try to estimate/guess the 
//	amount of inherent packet loss at the MAC level and call repeat transmissions accordingly.
//
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

	if((device_hard_start_xmit_backup != NULL) && (netdev != NULL)){

		memcpy(mymanethdr.final_destination, skb_original->data, 6);
		memcpy(mymanethdr.original_source, skb_original->data + 6, 6);
		memcpy(mymanethdr.eth_type_original, skb_original->data + 12, 2);

		/*CHECK whether this is a Manifold packet
		 *coming from the reception path */
		if((mymanethdr.eth_type_original[0] == eth_type_manifold[0]) && (mymanethdr.eth_type_original[1] == eth_type_manifold[1])){

		/*This is a Manifold packet coming from our Reception Path.
			 * Dont touch this packet. Just send it as it is.
			 * */
			/*Actual Transmission happens here*/
			if(netdev->flags & IFF_UP)
			{
				ret = (*device_hard_start_xmit_backup)(skb_original, netdev);
				return ret;				
			}
			else{
				printk(KERN_ALERT "\n\nDevice was DOWN !! \n\n");
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
			memcpy(skb_push(skb, 2), eth_type_manifold, 2 );
			memcpy(skb_push(skb, 6), netdev->dev_addr, 6);
			memcpy(skb_push(skb, 6), g_broadcast_mac, 6);

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
						if(prob_fwd > 2)
						{
							for(i = 0; i < prob_fwd && prob_fwd >= 0; i++)
							{
								//Directly calling hard_start_xmit fails for some kernels
								dev_queue_xmit(skb_copy(skb, GFP_ATOMIC));
								//wdl_hard_start_xmit(skb_copy(skb, GFP_ATOMIC), netdev);
							}
						}
						else
						{
							dev_queue_xmit(skb_copy(skb, GFP_ATOMIC));
							dev_queue_xmit(skb_copy(skb, GFP_ATOMIC));
						}
					}
					else
					{
						dev_queue_xmit(skb_copy(skb, GFP_ATOMIC));
						dev_queue_xmit(skb_copy(skb, GFP_ATOMIC));						
					}
				}

				
				return (*device_hard_start_xmit_backup)(skb, netdev);
			}
			else{
				printk(KERN_ALERT "\n\nDevice was DOWN !! \n\n");
				return NETDEV_TX_BUSY;	
			}
		}
	}
	kfree_skb(skb_original);
	return 0;
}



//
// This function is called everytime the module is unloaded
//
// -> 	Reinstate the device transmit function (hard_start_xmit) or (ndo_start_xmit)
//
// -> 	Delete the newly registered packet type
//
// -> 	Delete proc file system entries
//
// -> 	Free any memory held up by the module
//

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

	printk(KERN_ALERT "\nByee from LifeNet! \n");
}

// 
// This function initializes the kernel module into the linux kernel.
// The kernel module, sits at layer 2.5, meaning between the network or IP
// layer and the data link layer. This function initializes the kernel 
// module accordingly into the linux networking stack. This insertion of 
// a new layer is transparent to the higher layers such as transport and 
// application layers.
//
// ->	Register the new packet type. Usually most packets on WiFi have [08][00]
//	as their packet type. Packets traversing the kernel module are assigned
//	a new packet type [34][34]. This packet type is defined in the main header
//	file. It also registers the receive function, which the kernel calls each time 
//	after it receives packets of type [34][34].
//
// ->	Create files in the proc file-system. These files are used to export
//	data from the kernel space to the user space and vice versa.
//
// ->	Initialize timer and other data structures used in the module code.
//
// ->	Replace the lower level transmit function (called hard_start_xmit) of the device
//	with a new function wdl_hard_start_xmit. This new function modifies the packet, 
//	inserts the header information required for routing the packet at layer 2.5 and 
//	then calls the hard_start_xmit internally. This means that after the module is 
//	loaded, as packets come down from the higher layers to the lower layers for 
//	transmission, whenever the network layer calls the data link layer's hard_start_xmit 
//	function, in actuality, the new wdl_hard_start_xmit of the newly loaded module gets 
//	called. Thus the new layer between IP and data link layer.
//	For newer kernels 2.6.30+ the architecture of linux networking is revamped. This 
//	change does not allow one to singularly replace the hard_start_xmit function as it
//	exists no more. A new structure has been defined called netdev_ops for each device. 
//	This structure stores a pointer to the transmit function of the device called as 
//	ndo_start_xmit. The new structure netdev_ops is defined as a constant, so its 
//	attributes, one of which is the ndo_start_xmit cannot be replaced. Hence, we clone
//	the netdev_ops structure and then replace its ndo_start_xmit function with 
//	wdl_hard_start_xmit function
//

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

	printk(KERN_ALERT "Hello, world. the current process is: %s pid is : %d module param is: %s NEW_KERNEL : %d MYMANET_ALPHA is : %d MYMANET_BETA : %d: MYMANET_STORE_PATH : %d: MANIFOLD_HEADER_SIZE : %d \n", current->comm, current->pid, device_name, NEW_KERNEL, mymanet_alpha, mymanet_beta, MYMANET_STORE_PATH, MANIFOLD_HEADER_SIZE);

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

