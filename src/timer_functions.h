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

#ifndef TIMER_FUNCTIONS_H
#define TIMER_FUNCTIONS_H


/*Session ID. This is incremented mod INT_MAX periodically*/
extern int g_session_id;

/*This is the Session ID of the previous, complete session*/
extern int g_last_session_id;

/*Timer Function: Increments the session ID periodically*/
extern struct timer_list g_session_timer;

void timer_func(void);
void initialize_session_timer(void);
#endif /*End of timer_functions.h*/
