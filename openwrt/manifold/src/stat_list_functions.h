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

#ifndef STAT_LIST_FUNCTIONS_H
#define STAT_LIST_FUNCTIONS_H

#define STATLIST_EXPIRY_INTERVAL 30

struct stat_list{
        int id;
        uint8_t mac[6];
	uint32_t session_id;
	uint32_t tx_session_id;
	uint32_t fwd_session_id;	
	int num_rx;
	int num_rx_bcast;
	int num_tx;
	int num_fwd;
	uint32_t last_session_id;
	uint32_t last_tx_session_id;
	uint32_t last_fwd_session_id;	
	int last_num_rx;
	int last_num_rx_bcast;
	int last_num_tx;
	int last_num_fwd;
	struct timeval last_recv_time;
        struct stat_list *next;
        struct stat_list *prev;
};

extern int g_stat_list_count;
extern struct stat_list *stat_head;

int add_or_update_stat_entry(uint8_t mac[6], uint8_t tx_rx_flag, uint32_t session_id, uint8_t dest_mac[6]);
int free_entire_stat_list(void);
uint8_t search_and_update_stat(uint8_t mac[6], uint8_t tx_rx_flag, uint32_t session_id, uint8_t dest_mac[6]);
int collect_garbage_entries(int dummy_to_avoid_compile_warning);
int check_ignore_mac(uint8_t mac[6]);

#endif  /*End of stat_list_functions.h*/

