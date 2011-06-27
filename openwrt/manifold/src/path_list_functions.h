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

#ifndef PATH_LIST_FUNCTIONS_H
#define PATH_LIST_FUNCTIONS_H

struct path_list {
  
	uint8_t id;
	
	uint8_t src_mac[6];
	uint8_t hop1_mac[6];
	uint8_t hop2_mac[6];
	uint8_t hop3_mac[6];
	
	uint8_t curr_session_id;
	uint8_t prev_session_id;
	uint8_t curr_num_packets;
	uint8_t prev_num_packets;
	
	struct path_list *next;
	struct path_list *prev;
  
};

extern int g_path_list_count;
extern struct path_list *path_head;

int add_or_update_path_entry(uint8_t mac0[6], uint8_t mac1[6], uint8_t mac2[6], uint8_t mac3[6], uint32_t session_id);
int free_entire_path_list(void);
uint8_t search_and_update_path(uint8_t mac0[6], uint8_t mac1[6], uint8_t mac2[6], uint8_t mac3[6], uint32_t session_id);

#endif  /*End of path_list_functions.h*/