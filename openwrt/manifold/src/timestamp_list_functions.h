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

#ifndef TIMESTAMP_LIST_FUNCTIONS_H
#define TIMESTAMP_LIST_FUNCTIONS_H

struct timestamp_list{
	int id;
	uint8_t mac[6];
	struct timeval timestamp;
	struct timestamp_list *next;
	struct timestamp_list *prev;
};

extern int g_timestamp_list_count;

int add_or_update_timestamp_entry(uint8_t mac[6], uint32_t timestamp, uint32_t timestamp_frac);
int free_entire_timestamp_list(void);
uint32_t search_for_timestamp(uint8_t mac[6]);
uint32_t search_for_timestamp_frac(uint8_t mac[6]);
uint8_t search_and_update_timestamp(uint8_t mac[6], uint32_t new_timestamp, uint32_t timestamp_frac);

#endif 	/*End of timestamp_list_functions.h*/
