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

#ifndef DISTANCE_LIST_FUNCTIONS_H
#define DISTANCE_LIST_FUNCTIONS_H

struct distance_list{
	int id;
	uint8_t final_destination[6];
	uint8_t distance;
	struct distance_list *next;
	struct distance_list *prev;
};

extern int g_distance_list_count;

int add_or_update_entry(uint8_t final_destination[6], int distance);
int print_distance_list(void);
int free_entire_distance_list(void);
uint8_t search_for_distance(uint8_t final_destination[6]);
uint8_t search_and_update_distance(uint8_t final_destination[6], uint8_t new_distance);
void remove_distance_entry(uint8_t mac[6]);

#endif 	/*End of distance_list_functions.h*/
