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

#include<linux/timer.h>
#include"stat_list_functions.h"
#define MANIFOLD_INTERVAL 30

/*Extern Declaration*/
extern int g_manifold_session_transmitted_count;
extern int g_manifold_last_session_transmitted_count;
extern int g_per_session_dup_cnt;
extern int g_last_per_session_dup_cnt;
/*END: Extern Declaration*/

/*Session ID. This is incremented mod INT_MAX periodically*/
int g_session_id=0;

/*This stores the previous session id. Note that we report the statistics of the previous, completed session.*/
int g_last_session_id=0;

/*Timer Function: Increments the session ID periodically*/
struct timer_list g_session_timer;

void timer_func(void)
{
	collect_garbage_entries(1);
  
	/*Save the statistics of the previous(complete) session. This will be displayed in the proc entries.*/
	g_last_session_id = g_session_id;
	g_manifold_last_session_transmitted_count = g_manifold_session_transmitted_count; 

	g_last_per_session_dup_cnt = g_per_session_dup_cnt;
	g_per_session_dup_cnt = 0;

	/*Prepare for the next session.*/
	g_session_id = (g_session_id + 1) % 255;	
	g_manifold_session_transmitted_count = 0;	/*Clear the Manifold transmitted count for this new session*/

	printk(KERN_ALERT "\nI am in the Timer Function!! session id is now: %d  Dup %d in last session \n", g_session_id, g_last_per_session_dup_cnt);
	mod_timer(&g_session_timer, jiffies + (MANIFOLD_INTERVAL * HZ));
	return;
}

void initialize_session_timer(void)
{

	/*Timer*/
	g_manifold_last_session_transmitted_count = 0;
	g_manifold_session_transmitted_count = 0;
	g_per_session_dup_cnt = 0;
	g_last_per_session_dup_cnt = 0;

	init_timer(&g_session_timer);
	g_session_timer.function = (void *)timer_func;
	g_session_timer.data = 0;
	g_session_timer.expires = jiffies + (MANIFOLD_INTERVAL * HZ);
	add_timer(&g_session_timer);
	/*END: Timer*/
}

